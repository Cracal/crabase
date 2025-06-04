#include "cra_malloc.h"
#include "cra_timewheel.h"
#include "collections/cra_llist.h"

#define CRA_TIMEWHEEL_FREE_TIMER_MAX 64

typedef struct
{
    bool active;
    unsigned int repeat;
    unsigned int timeout_ms;

    void *arg;
    cra_timer_fn on_timeout;

    CraLListNode *node; // 该timer所在的list结点
} CraTimer;
CRA_REFCNT_NAME(CraTimer, _CraTimerRc);

struct _CraTimewheel
{
    unsigned int tick_ms;
    unsigned int wheel_size;
    unsigned int current;
    CraLList **wheel_buckets; // [CraLList<CraTimerRc *>]
    CraLList *freenodelist;   // CraLList<CraLListNode<CraTimerRc *> *>
    CraTimewheel *upper_wheel;
};

#if 1 // free node list

static inline CraLList *cra_freenodelist_new(void)
{
    CraLList *list = cra_alloc(CraLList);
    cra_llist_init0(CraTimerRc *, list, false, NULL);
    return list;
}

static inline void cra_freenodelist_delete(CraLList *list)
{
    cra_llist_uninit(list);
    cra_dealloc(list);
}

static inline CraLListNode *cra_freenodelist_get(CraLList *list)
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

static inline void cra_freenodelist_put(CraLList *list, CraLListNode *node)
{
    if (list->count < CRA_TIMEWHEEL_FREE_TIMER_MAX)
    {
        bzero(node, sizeof(*node) + sizeof(CraTimerRc *));
        cra_llist_insert_node(list, 0, node);
    }
    else
        cra_llist_destroy_node(&node);
}

#endif // end free node list

#if 1 // timer

CraTimerRc *cra_timer_new(unsigned int repeat, unsigned int timeout_ms, void *arg, cra_timer_fn on_timeout, cra_timer_fn before_destroy)
{
    CraTimerRc *timer = cra_alloc(CraTimerRc);
    cra_refcnt_init(timer, true, true, (cra_uninit_fn)before_destroy);
    timer->o.active = true;
    timer->o.repeat = repeat;
    timer->o.timeout_ms = timeout_ms;
    timer->o.arg = arg;
    timer->o.on_timeout = on_timeout;
    timer->o.node = NULL;
    return timer;
}

bool cra_timer_is_active(CraTimerRc *timer) { return timer->o.active; }
void cra_timer_cancel(CraTimerRc *timer) { timer->o.active = false; }

unsigned int cra_timer_get_repeat(CraTimerRc *timer) { return timer->o.repeat; }
void cra_timer_set_repeat(CraTimerRc *timer, unsigned int repeat) { timer->o.repeat = repeat; }

unsigned int cra_timer_get_timeout(CraTimerRc *timer) { return timer->o.timeout_ms; }
void cra_timer_set_timeout(CraTimerRc *timer, unsigned int timeout_ms) { timer->o.timeout_ms = timeout_ms; }

void *cra_timer_get_arg(CraTimerRc *timer) { return timer->o.arg; }
void cra_timer_set_arg(CraTimerRc *timer, void *arg) { timer->o.arg = arg; }

#endif // end timer

#define CRA_TIMER_IN_NODE(_node) (*(CraTimerRc **)(_node)->val)

static CraTimewheel *__cra_timewheel_create(unsigned int tick_ms, unsigned int wheel_size, CraLList *freenodelist)
{
    CraTimewheel *wheel = cra_alloc(CraTimewheel);
    wheel->tick_ms = tick_ms;
    wheel->wheel_size = wheel_size;
    wheel->current = 0;
    wheel->wheel_buckets = (CraLList **)cra_calloc(wheel->wheel_size, sizeof(CraLList *));
    wheel->freenodelist = freenodelist;
    wheel->upper_wheel = NULL;
    return wheel;
}

static bool cra_timewheel_add_node_to(CraTimewheel *wheel, CraLListNode *timernode, unsigned int remain_timeout_ms)
{
    unsigned int bucket = ((wheel->current + remain_timeout_ms / wheel->tick_ms) % wheel->wheel_size);
    CraLList *list = wheel->wheel_buckets[bucket];
    if (!list)
    {
        list = cra_alloc(CraLList);
        cra_llist_init0(CraTimerRc *, list, false, NULL);
        wheel->wheel_buckets[bucket] = list;
    }
    CRA_TIMER_IN_NODE(timernode)->o.node = timernode;
    return cra_llist_insert_node(list, 0, timernode);
}

static bool cra_timewheel_add_node(CraTimewheel *wheel, CraLListNode *timernode)
{
    CraTimerRc *timer = CRA_TIMER_IN_NODE(timernode);
    if (timer->o.timeout_ms >= wheel->tick_ms * wheel->wheel_size)
    {
        if (!wheel->upper_wheel)
            wheel->upper_wheel = __cra_timewheel_create(wheel->tick_ms * wheel->wheel_size, wheel->wheel_size, wheel->freenodelist);
        return cra_timewheel_add_node(wheel->upper_wheel, timernode);
    }
    return cra_timewheel_add_node_to(wheel, timernode, timer->o.timeout_ms);
}

static void cra_timewheel_tick_inner(CraTimewheel *wheel, CraTimewheel *lower_wheel)
{
    if (++wheel->current == wheel->wheel_size)
    {
        wheel->current = 0;
        if (wheel->upper_wheel)
            cra_timewheel_tick_inner(wheel->upper_wheel, wheel);
    }

    CraLList *list;
    CraTimerRc *timer;
    CraLListNode *curr;

    list = wheel->wheel_buckets[wheel->current];
    if (!list)
        return;

    while (list->head)
    {
        curr = list->head;
        timer = CRA_TIMER_IN_NODE(curr);
        cra_llist_unlink_node(list, curr);

        if (!timer->o.active)
        {
        release_timer:
            cra_refcnt_unref0(timer);
            cra_freenodelist_put(wheel->freenodelist, curr);
            continue;
        }

        if (!lower_wheel)
        {
            timer->o.on_timeout(timer);

            if (--timer->o.repeat == 0)
                goto release_timer;

            cra_timewheel_add_node(wheel, curr);
        }
        else
        {
            cra_timewheel_add_node_to(lower_wheel, curr, timer->o.timeout_ms % wheel->tick_ms);
        }
    }
}

CraTimewheel *cra_timewheel_create(unsigned int tick_ms, unsigned int wheel_size)
{
    return __cra_timewheel_create(tick_ms, wheel_size, cra_freenodelist_new());
}

void cra_timewheel_destroy(CraTimewheel *wheel)
{
    if (wheel->upper_wheel)
        cra_timewheel_destroy(wheel->upper_wheel);
    for (unsigned int i = 0; i < wheel->wheel_size; ++i)
    {
        if (wheel->wheel_buckets[i])
        {
            cra_llist_uninit(wheel->wheel_buckets[i]);
            cra_dealloc(wheel->wheel_buckets[i]);
        }
    }
    if (!wheel->upper_wheel)
        cra_freenodelist_delete(wheel->freenodelist);
    cra_free(wheel->wheel_buckets);
    cra_free(wheel);
}

bool cra_timewheel_add(CraTimewheel *wheel, CraTimerRc *timer)
{
    CraLListNode *node = cra_freenodelist_get(wheel->freenodelist);
    CRA_TIMER_IN_NODE(node) = timer;
    if (cra_timewheel_add_node(wheel, node))
    {
        cra_refcnt_ref(timer);
        return true;
    }
    return false;
}

void cra_timewheel_tick(CraTimewheel *wheel)
{
    cra_timewheel_tick_inner(wheel, NULL);
}

unsigned int cra_timewheel_get_tick_time(CraTimewheel *wheel)
{
    return wheel->tick_ms;
}

unsigned int cra_timewheel_get_wheel_size(CraTimewheel *wheel)
{
    return wheel->wheel_size;
}
