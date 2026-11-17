#include "crabase/cra_timewheel.h"
#include "crabase/cra_malloc.h"

// short name

#define TICK_MS     CRA_TIMER_TICK_MS
#define BASE_BITS   CRA_TIMER_BASE_BITS
#define BASE_SIZE   CRA_TIMER_BASE_SIZE
#define BASE_MASK   (BASE_SIZE - 1)
#define UPPER_BITS  CRA_TIMER_UPPER_BITS
#define UPPER_SIZE  CRA_TIMER_UPPER_SIZE
#define UPPER_MASK  (UPPER_SIZE - 1)
#define UPPER_MAX   CRA_TIMER_UPPER_MAX
#define SLOT_NUM    CRA_TIMER_SLOT_NUM
#define BITMAP_SIZE CRA_TIMER_BITMAP_SIZE

struct CraTimerNode
{
    CraTimerNode *next;

    uint32_t ticks;
    int32_t  time_ms;
    void    *user_data;
    CRA_TIMER_FN_DEF((*cb));
};

static_assert(TICK_MS > 0, "TICK_MS must be greater than 0");

static inline CraTimerNode *
get_free_node(CraTimewheel *wheel)
{
    assert(wheel);
    if (wheel->list)
    {
        CraTimerNode *node = wheel->list;
        wheel->list = node->next;
        return node;
    }
    return cra_malloc(sizeof(CraTimerNode));
}

static inline void
put_free_node(CraTimewheel *wheel, CraTimerNode *node)
{
    assert(node);
    assert(wheel);
    node->next = wheel->list;
    wheel->list = node;
}

static inline void
free_all_free_nodes(CraTimewheel *wheel)
{
    assert(wheel);
    for (CraTimerNode *temp, *node = wheel->list; node;)
    {
        temp = node;
        node = node->next;
        cra_free(temp);
    }
    wheel->list = NULL;
}

static inline void
link_head(CraTimerNode **phead, CraTimerNode *node)
{
    assert(node);
    assert(phead);
    node->next = *phead;
    *phead = node;
}

static inline CraTimerNode *
clear_head(CraTimerNode **phead)
{
    assert(phead);
    CraTimerNode *node = *phead;
    *phead = NULL;
    return node;
}

static inline void
free_nodes(CraTimerNode *head)
{
    for (CraTimerNode *temp; head;)
    {
        temp = head;
        head = head->next;

        temp->cb(NULL, temp->time_ms, temp->user_data);
        cra_free(temp);
    }
}

static inline void
bitmap_set(uint64_t *bitmap, int layer, int index)
{
    int idx = layer == 0 ? index : index + (1ull << (BASE_BITS + (layer - 1) * UPPER_BITS));
    bitmap[idx >> 6] |= (1ull << (idx & 63));
}

static inline void
bitmap_clr(uint64_t *bitmap, int layer, int index)
{
    int idx = layer == 0 ? index : index + (1ull << (BASE_BITS + (layer - 1) * UPPER_BITS));
    bitmap[idx >> 6] &= ~(1ull << (idx & 63));
}

static inline int
bitmap_ctzll(uint64_t val)
{
#if defined(CRA_COMPILER_GCC) || defined(CRA_COMPILER_CLANG)
    return __builtin_ctzll(val);
#elif defined(CRA_COMPILER_MSVC)
    DWORD i;
    _BitScanForward64(&i, val);
    return (int)i;
#else
    int i = 0;
    while (!(val & 1))
    {
        val >>= 1;
        ++i;
    }
    return i;
#endif
}

static inline int32_t
get_next_timeout(CraTimewheel *wheel)
{
    if (wheel->ntimers == 0)
        return -1;

    uint32_t  min_ticks = UINT32_MAX;
    uint64_t *bitmap = wheel->bitmap;
    for (int i = 0; i < 4; ++i)
    {
        uint64_t val = bitmap[i];
        while (val)
        {
            int idx = bitmap_ctzll(val);
            assert(wheel->base[(i << 6) + idx]);
            uint32_t ticks = wheel->base[(i << 6) + idx]->ticks - wheel->ticks;
            if (ticks < min_ticks)
                min_ticks = ticks;
            val &= val - 1;
        }
    }
    bitmap += 4;
    for (int i = 0; i < (int)BITMAP_SIZE - 4; ++i)
    {
        uint64_t val = bitmap[i];
        while (val)
        {
            int idx = bitmap_ctzll(val);
            assert(wheel->upper[i][idx]);
            uint32_t ticks = wheel->upper[i][idx]->ticks - wheel->ticks;
            if (ticks < min_ticks)
                min_ticks = ticks;
            val &= val - 1;
        }
    }

    return (int32_t)(min_ticks * TICK_MS);
}

// ===========================================

void
cra_timewheel_init(CraTimewheel *wheel)
{
    assert(wheel);
    memset(wheel, 0, sizeof(CraTimewheel));
}

void
cra_timewheel_uninit(CraTimewheel *wheel)
{
    assert(wheel);
    uint64_t *bitmap = wheel->bitmap;
    for (int i = 0; i < 4; ++i)
    {
        uint64_t val = bitmap[i];
        while (val)
        {
            int idx = bitmap_ctzll(val);
            assert(wheel->base[(i << 6) + idx]);
            free_nodes(wheel->base[(i << 6) + idx]);
            val &= val - 1;
        }
    }
    bitmap += 4;
    for (int i = 0; i < (int)BITMAP_SIZE - 4; ++i)
    {
        uint64_t val = bitmap[i];
        while (val)
        {
            int idx = bitmap_ctzll(val);
            assert(wheel->upper[i][idx]);
            free_nodes(wheel->upper[i][idx]);
            val &= val - 1;
        }
    }
    free_all_free_nodes(wheel);
}

static void
cra_timewheel_add_node(CraTimewheel *wheel, CraTimerNode *node)
{
    if (wheel->ntimers == UINT32_MAX)
    {
        node->cb(NULL, node->time_ms, node->user_data);
        put_free_node(wheel, node);
        return;
    }
    ++wheel->ntimers;

    int      index;
    uint32_t ticks = node->ticks - wheel->ticks;
    if (ticks < BASE_SIZE)
    {
        index = node->ticks & BASE_MASK;
        link_head(&wheel->base[index], node);
        if (wheel->base[index] == node)
            bitmap_set(wheel->bitmap, 0, index);
    }
    else
    {
        int      i = 0;
        uint32_t size = BASE_SIZE << UPPER_BITS;
        for (; i < UPPER_MAX - 1 && ticks >= size; ++i)
            size <<= UPPER_BITS;
        index = (int)((node->ticks >> (BASE_BITS + i * UPPER_BITS)) & UPPER_MASK);
        link_head(&wheel->upper[i][index], node);
        if (wheel->upper[i][index] == node)
            bitmap_set(wheel->bitmap, i + 1, index);
    }
}

void
cra_timewheel_add(CraTimewheel *wheel, int32_t delay_ms, void *user_data, CRA_TIMER_FN_DEF(cb))
{
    assert(cb);
    assert(wheel);
    assert(delay_ms > 0);

    CraTimerNode *timer = get_free_node(wheel);
    if (!timer)
    {
        cb(NULL, delay_ms, user_data); // add failed, tell external handle it
        return;
    }

    timer->ticks = wheel->ticks + delay_ms / TICK_MS;
    timer->user_data = user_data;
    timer->time_ms = delay_ms;
    timer->cb = cb;

    cra_timewheel_add_node(wheel, timer);
}

static void
cra_timewheel_tick(CraTimewheel *wheel)
{
    int           index;
    CraTimerNode *timer;
    uint32_t      size = BASE_SIZE;
    uint32_t      current = ++wheel->ticks;
    uint32_t      ticks = current >> BASE_BITS;
    for (int i = 0; i < 4 && (current & (size - 1)) == 0; ++i, size <<= UPPER_BITS, ticks >>= UPPER_BITS)
    {
        index = (int)(ticks & UPPER_MASK);
        if (index == 0 || !(timer = clear_head(&wheel->upper[i][index])))
            continue;

        bitmap_clr(wheel->bitmap, i + 1, index);
        for (CraTimerNode *temp; timer;)
        {
            temp = timer;
            timer = timer->next;
            --wheel->ntimers;
            cra_timewheel_add_node(wheel, temp);
        }
    }

    index = current & BASE_MASK;
    if (!!(timer = clear_head(&wheel->base[index])))
    {
        bitmap_clr(wheel->bitmap, 0, index);
        for (CraTimerNode *temp; timer;)
        {
            temp = timer;
            timer = timer->next;
            --wheel->ntimers;
            temp->cb(wheel, temp->time_ms, temp->user_data);
            put_free_node(wheel, temp);
        }
    }
}

int32_t
cra_timewheel_update(CraTimewheel *wheel, uint64_t now_ms)
{
    assert(wheel);
    assert(now_ms > 0);

    if (wheel->last_ms == 0)
        wheel->last_ms = now_ms;
    if (now_ms < wheel->last_ms)
        now_ms = wheel->last_ms;

    int32_t  diff = 0;
    uint32_t ticks = (uint32_t)((now_ms - wheel->last_ms) / TICK_MS);
    if (ticks > 0)
    {
        // wheel->last_ms = now_ms;
        wheel->last_ms += ticks * TICK_MS;
        diff = (int32_t)(now_ms - wheel->last_ms);

        while (ticks--)
            cra_timewheel_tick(wheel);
    }

    return get_next_timeout(wheel) - diff;
}
