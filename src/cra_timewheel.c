#include "cra_timewheel.h"
#include "collections/cra_llist.h"
#include "cra_malloc.h"

#define CRA_TIMEWHEEL_FREE_TIMER_MAX 512

#if 1 // free node list

static inline CraLList *
cra_freenodelist_new(void)
{
    CraLList *list = cra_alloc(CraLList);
    cra_llist_init0(CraTimer_base *, list, false);
    return list;
}

static inline void
cra_freenodelist_delete(CraLList *list)
{
    cra_llist_uninit(list);
    cra_dealloc(list);
}

static inline CraLListNode *
cra_freenodelist_get(CraLList *list)
{
    CraLListNode *node;
    if (list->count > 0)
    {
        node = list->head;
        cra_llist_unlink_node(list, node);
    }
    else
    {
        node = cra_llist_create_node(list->ele_size);
    }
    return node;
}

static inline void
cra_freenodelist_put(CraLList *list, CraLListNode *node)
{
    if (list->count < CRA_TIMEWHEEL_FREE_TIMER_MAX)
    {
        bzero(node->val, sizeof(CraTimer_base *));
        cra_llist_insert_node(list, 0, node);
    }
    else
        cra_llist_destroy_node(&node);
}

#endif // end free node list

void
cra_timer_base_init(CraTimer_base    *base,
                    uint32_t          repeat,
                    uint32_t          timeout_ms,
                    cra_timer_base_fn on_timeout,
                    cra_timer_base_fn on_remove_timer)
{
    assert(on_timeout != NULL);

    cra_timer_base_set_active(base);
    cra_timer_base_set_repeat(base, repeat);
    cra_timer_base_set_timeout(base, timeout_ms);
    base->on_timeout = on_timeout;
    base->on_remove_timer = on_remove_timer;
}

#define CRA_TIMER_IN_NODE(_node) (*(CraTimer_base **)(_node)->val)

static void
__cra_timewheel_init(CraTimewheel *wheel, uint32_t tick_ms, uint32_t wheel_size, CraLList *freenodelist)
{
    wheel->tick_ms = tick_ms;
    wheel->current = 0;
    wheel->wheel_size = wheel_size;
    wheel->wheel_buckets = cra_calloc(wheel->wheel_size, sizeof(CraLList *));
    wheel->freenodelist = freenodelist;
    wheel->upper_wheel = NULL;
}

static bool
cra_timewheel_add_node_to(CraTimewheel *wheel, CraLListNode *timernode, uint32_t remain_timeout_ms)
{
    remain_timeout_ms = CRA_MAX(remain_timeout_ms, wheel->tick_ms);
    uint32_t  bucket = ((wheel->current + remain_timeout_ms / wheel->tick_ms) % wheel->wheel_size);
    CraLList *list = wheel->wheel_buckets[bucket];
    if (!list)
    {
        list = cra_alloc(CraLList);
        cra_llist_init0(CraTimer_base *, list, false);
        wheel->wheel_buckets[bucket] = list;
    }
    return cra_llist_insert_node(list, 0, timernode);
}

static bool
cra_timewheel_add_node(CraTimewheel *wheel, CraLListNode *timernode)
{
    CraTimer_base *timer = CRA_TIMER_IN_NODE(timernode);
    if (timer->timeout_ms >= wheel->tick_ms * wheel->wheel_size)
    {
        if (!wheel->upper_wheel)
        {
            wheel->upper_wheel = cra_alloc(CraTimewheel);
            __cra_timewheel_init(
              wheel->upper_wheel, wheel->tick_ms * wheel->wheel_size, wheel->wheel_size, wheel->freenodelist);
        }
        return cra_timewheel_add_node(wheel->upper_wheel, timernode);
    }
    return cra_timewheel_add_node_to(wheel, timernode, timer->timeout_ms);
}

static void
cra_timewheel_tick_inner(CraTimewheel *wheel, CraTimewheel *lower_wheel)
{
    if (++wheel->current == wheel->wheel_size)
    {
        wheel->current = 0;
        if (wheel->upper_wheel)
            cra_timewheel_tick_inner(wheel->upper_wheel, wheel);
    }

    CraLList      *list;
    CraLListNode  *curr;
    CraTimer_base *timer;

    list = wheel->wheel_buckets[wheel->current];
    if (!list)
        return;

    while (list->head)
    {
        curr = list->head;
        timer = CRA_TIMER_IN_NODE(curr);
        cra_llist_unlink_node(list, curr);

        if (!timer->active)
        {
        release_timer:
            if (timer->on_remove_timer)
                timer->on_remove_timer(timer);
            cra_freenodelist_put(wheel->freenodelist, curr);
            continue;
        }

        if (!lower_wheel)
        {
            timer->on_timeout(timer);

            if (timer->active &&
                (timer->repeat == CRA_TIMER_INFINITE || (--timer->repeat > 0 && timer->repeat != CRA_TIMER_INFINITE)))
            {
                cra_timewheel_add_node(wheel, curr);
            }
            else
            {
                goto release_timer;
            }
        }
        else
        {
            cra_timewheel_add_node_to(lower_wheel, curr, timer->timeout_ms % wheel->tick_ms);
        }
    }
}

void
cra_timewheel_init(CraTimewheel *wheel, uint32_t tick_ms, uint32_t wheel_size)
{
    __cra_timewheel_init(wheel, tick_ms, wheel_size, cra_freenodelist_new());
}

void
cra_timewheel_uninit(CraTimewheel *wheel)
{
    if (wheel->upper_wheel)
    {
        cra_timewheel_uninit(wheel->upper_wheel);
        cra_dealloc(wheel->upper_wheel);
    }
    for (uint32_t i = 0; i < wheel->wheel_size; ++i)
    {
        if (wheel->wheel_buckets[i])
        {
            CraLListIter    it;
            CraTimer_base **timerptr;
            for (cra_llist_iter_init(wheel->wheel_buckets[i], &it); cra_llist_iter_next(&it, (void **)&timerptr);)
            {
                if ((*timerptr)->on_remove_timer)
                    (*timerptr)->on_remove_timer(*timerptr);
            }
            cra_llist_uninit(wheel->wheel_buckets[i]);
            cra_dealloc(wheel->wheel_buckets[i]);
        }
    }
    if (!wheel->upper_wheel)
        cra_freenodelist_delete(wheel->freenodelist);
    cra_free(wheel->wheel_buckets);
}

bool
cra_timewheel_add(CraTimewheel *wheel, CraTimer_base *timer)
{
    CraLListNode *node = cra_freenodelist_get(wheel->freenodelist);
    CRA_TIMER_IN_NODE(node) = timer;
    if (cra_timewheel_add_node(wheel, node))
        return true;
    return false;
}

void
cra_timewheel_tick(CraTimewheel *wheel)
{
    cra_timewheel_tick_inner(wheel, NULL);
}
