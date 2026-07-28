#include "cra_assert.h"
#include "cra_malloc.h"
#include "cra_refcnt.h"
#include "threads/cra_thrdpool.h"

struct TestObj
{
    int   i;
    char *s;
};

int flag = 0;

static void
testobj_init(struct TestObj *obj)
{
    obj->i = 100;
    obj->s = cra_malloc(100);
    assert_always(obj->s != NULL);
    memcpy(obj->s, "hello", 6);

    flag = 1;
}

static void
testobj_uninit(struct TestObj *obj)
{
    cra_free(obj->s);

    flag = 2;
}

void
test_ref_make(void)
{
    CraRef         *ref, *ref2;
    struct TestObj *obj;

    ref = cra_ref_make(sizeof(struct TestObj), (cra_ref_fn)testobj_uninit);
    assert_always(ref != NULL);
    assert_always(ref->cnt == 1);

    assert_always(flag == 0);
    obj = cra_ref_get_ptr_uncheck(ref);
    assert_always(obj != NULL);
    testobj_init(obj);
    assert_always(flag == 1);

    cra_ref_ref(ref);
    assert_always(ref->cnt == 2);

    assert_always(cra_ref_unref(ref) == false);
    assert_always(ref->cnt == 1);

    cra_ref_ref(ref);
    cra_ref_ref(ref);
    cra_ref_ref(ref);
    cra_ref_ref(ref);
    assert_always(ref->cnt == 5);

    ref2 = ref;
    cra_ref_unref_clear(&ref2);
    assert_always(ref2 == NULL);
    assert_always(ref->cnt == 4);
    cra_ref_unref(ref);
    cra_ref_unref(ref);
    assert_always(cra_ref_unref(ref) == false);
    assert_always(ref->cnt == 1);
    assert_always(flag == 1);
    assert_always(cra_ref_unref(ref) == true);
    assert_always(flag == 2);

    // cra_ref_unref(ref); // error: dangling pointer
}

void
test_ref_take(void)
{
    CraRef         *ref, *ref2;
    struct TestObj *obj, *obj2;

    obj = cra_alloc(struct TestObj);
    assert_always(obj != NULL);
    testobj_init(obj);
    assert_always(flag == 1);

    ref = cra_ref_take(obj, (cra_ref_fn)testobj_uninit, cra_dealloc);
    assert_always(ref != NULL);
    assert_always(ref->cnt == 1);

    obj2 = (struct TestObj *)cra_ref_get_ptr_uncheck(ref);
    assert_always(obj2 == obj);

    cra_ref_ref(ref);
    assert_always(ref->cnt == 2);

    assert_always(cra_ref_unref(ref) == false);
    assert_always(ref->cnt == 1);

    cra_ref_ref(ref);
    cra_ref_ref(ref);
    cra_ref_ref(ref);
    cra_ref_ref(ref);
    assert_always(ref->cnt == 5);

    ref2 = ref;
    cra_ref_unref_clear(&ref2);
    assert_always(ref2 == NULL);
    assert_always(ref->cnt == 4);
    cra_ref_unref(ref);
    cra_ref_unref(ref);
    assert_always(cra_ref_unref(ref) == false);
    assert_always(ref->cnt == 1);
    assert_always(flag == 1);
    assert_always(cra_ref_unref(ref) == true);
    assert_always(flag == 2);

    // cra_ref_unref(ref); // error: dangling pointer
}

#if 1 // intrusive ref

struct TestRefObj
{
    int        i;
    CraRefHead ref;
    char      *s;
};

static void
testrefobj_init(struct TestRefObj *obj)
{
    obj->i = 100;
    obj->s = cra_malloc(100);
    assert_always(obj->s != NULL);
    memcpy(obj->s, "hello world", 12);

    cra_ref_head_init(&obj->ref);

    flag = 1;
}

static void
testrefobj_uninit(struct TestRefObj *obj)
{
    assert_always(obj->ref.cnt == 0);
    cra_free(obj->s);
    obj->s = NULL;
    obj->i = 0;

    flag = 2;
}

static void
testrefobj_ref(struct TestRefObj *obj)
{
    cra_ref_head_ref(&obj->ref);
}

static void
testrefobj_unref(struct TestRefObj *obj)
{
    if (cra_ref_head_unref(&obj->ref))
        testrefobj_uninit(obj);
}

void
test_ref_intrusive(void)
{
    struct TestRefObj obj;

    testrefobj_init(&obj);
    assert_always(flag == 1);
    assert_always(obj.ref.cnt == 1);

    testrefobj_ref(&obj);
    assert_always(obj.ref.cnt == 2);

    testrefobj_unref(&obj);
    assert_always(obj.ref.cnt == 1);

    testrefobj_ref(&obj);
    testrefobj_ref(&obj);
    testrefobj_ref(&obj);
    testrefobj_ref(&obj);
    assert_always(obj.ref.cnt == 5);

    testrefobj_unref(&obj);
    testrefobj_unref(&obj);
    testrefobj_unref(&obj);
    testrefobj_unref(&obj);
    testrefobj_unref(&obj);
    assert_always(obj.ref.cnt == 0);
    assert_always(obj.s == NULL);
    assert_always(obj.i == 0);
    assert_always(flag == 2);
}

#endif // end intrusive ref

void
test_weak_ref_make(void)
{
    CraWeakRef     *ref, *ref2;
    struct TestObj *obj, *obj2;

    ref = cra_weak_ref_make(sizeof(struct TestObj), (cra_ref_fn)testobj_uninit);
    assert_always(ref != NULL);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 1);

    obj = (struct TestObj *)cra_weak_ref_get_ptr_uncheck(ref);
    assert_always(obj != NULL);
    testobj_init(obj);
    assert_always(flag == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    // weak ref holder acquire object
    obj2 = (struct TestObj *)cra_weak_ref_acquire(ref);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 2);
    assert_always(obj2 != NULL);
    assert_always(obj2 == obj);
    // strong ref holder get object directly
    obj = (struct TestObj *)cra_weak_ref_get_ptr_uncheck(ref);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 2);
    assert_always(obj != NULL);
    assert_always(obj == obj2);

    cra_weak_ref_release_clear_ptr(ref, (void **)&obj2);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 1);
    assert_always(obj2 == NULL);

    cra_weak_ref_unref(ref);
    assert_always(ref->weak_cnt == 1);
    assert_always(ref->cnt == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    ref2 = ref;
    cra_weak_ref_unref_clear(&ref2);
    assert_always(ref2 == NULL);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 1);

    assert_always(flag == 1);
    // destroy object & control block
    cra_weak_ref_release_clear(&ref);
    assert_always(ref == NULL);
    assert_always(flag == 2);

    // ===================================

    ref = cra_weak_ref_make(sizeof(struct TestObj), (cra_ref_fn)testobj_uninit);
    assert_always(ref != NULL);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 1);
    obj = (struct TestObj *)cra_weak_ref_get_ptr_uncheck(ref);
    testobj_init(obj);
    assert_always(flag == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    obj2 = (struct TestObj *)cra_weak_ref_acquire(ref);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 2);
    assert_always(obj2 != NULL);
    assert_always(obj2 == obj);
    assert_always(flag == 1);

    cra_weak_ref_release(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    assert_always(flag == 1);
    cra_weak_ref_release(ref);
    assert_always(flag == 2);
    assert_always(ref->cnt == 0); // destroy object
    assert_always(ref->weak_cnt == 1);

    obj = (struct TestObj *)cra_weak_ref_acquire(ref);
    assert_always(obj == NULL); // object is destroyed, acquire failed
    assert_always(ref->cnt == 0);
    assert_always(ref->weak_cnt == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 0);
    assert_always(ref->weak_cnt == 2);

    cra_weak_ref_unref(ref);
    assert_always(ref->cnt == 0);
    assert_always(ref->weak_cnt == 1);

    assert_always(cra_weak_ref_unref(ref) == true); // destroy control block
    // ref is a dangling pointer now
}

void
test_weak_ref_take(void)
{
    CraWeakRef     *ref, *ref2;
    struct TestObj *obj, *obj2;

    obj = cra_alloc(struct TestObj);
    assert_always(obj != NULL);
    testobj_init(obj);
    assert_always(flag == 1);

    ref = cra_weak_ref_take(obj, (cra_ref_fn)testobj_uninit, cra_dealloc);
    assert_always(ref != NULL);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    // weak ref holder acquire object
    obj2 = (struct TestObj *)cra_weak_ref_acquire(ref);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 2);
    assert_always(obj2 != NULL);
    assert_always(obj2 == obj);
    // strong ref holder get object directly
    obj = (struct TestObj *)cra_weak_ref_get_ptr_uncheck(ref);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 2);
    assert_always(obj != NULL);
    assert_always(obj == obj2);

    cra_weak_ref_release_clear_ptr(ref, (void **)&obj2);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 1);
    assert_always(obj2 == NULL);

    cra_weak_ref_unref(ref);
    assert_always(ref->weak_cnt == 1);
    assert_always(ref->cnt == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    ref2 = ref;
    cra_weak_ref_unref_clear(&ref2);
    assert_always(ref2 == NULL);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 1);

    assert_always(flag == 1);
    // destroy object & control block
    cra_weak_ref_release_clear(&ref);
    assert_always(ref == NULL);
    assert_always(flag == 2);

    // ===================================

    obj = cra_alloc(struct TestObj);
    assert_always(obj != NULL);
    testobj_init(obj);
    assert_always(flag == 1);

    ref = cra_weak_ref_take(obj, (cra_ref_fn)testobj_uninit, cra_dealloc);
    assert_always(ref != NULL);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    obj2 = (struct TestObj *)cra_weak_ref_acquire(ref);
    assert_always(ref->weak_cnt == 2);
    assert_always(ref->cnt == 2);
    assert_always(obj2 != NULL);
    assert_always(obj2 == obj);
    assert_always(flag == 1);

    cra_weak_ref_release(ref);
    assert_always(ref->cnt == 1);
    assert_always(ref->weak_cnt == 2);

    assert_always(flag == 1);
    cra_weak_ref_release(ref);
    assert_always(flag == 2);
    assert_always(ref->cnt == 0); // destroy object
    assert_always(ref->weak_cnt == 1);

    obj = (struct TestObj *)cra_weak_ref_acquire(ref);
    assert_always(obj == NULL); // object is destroyed, acquire failed
    assert_always(ref->cnt == 0);
    assert_always(ref->weak_cnt == 1);

    cra_weak_ref_ref(ref);
    assert_always(ref->cnt == 0);
    assert_always(ref->weak_cnt == 2);

    cra_weak_ref_unref(ref);
    assert_always(ref->cnt == 0);
    assert_always(ref->weak_cnt == 1);

    assert_always(cra_weak_ref_unref(ref) == true); // destroy control block
    // ref is a dangling pointer now
}

#if 1 // intrusive weak ref

struct TestWeakRefObj
{
    CraWeakRefHead head;
    float          f;
    char          *s;
};

static void
testweakrefobj_init(struct TestWeakRefObj *obj)
{
    cra_weak_ref_head_init(&obj->head);
    obj->f = 5.7f;
    obj->s = cra_malloc(100);
    assert_always(obj->s != NULL);
    memcpy(obj->s, "hello weak reference", 21);

    flag = 1;
}

static void
testweakrefobj_uninit(struct TestWeakRefObj *obj)
{
    assert_always(obj->head.cnt == 0);
    cra_free(obj->s);
    obj->s = NULL;
    obj->f = 0.0f;

    flag = 2;
}

static void
testweakrefobj_weak_ref(struct TestWeakRefObj *obj)
{
    cra_weak_ref_head_ref(&obj->head);
}

static void
testweakrefobj_weak_unref(struct TestWeakRefObj *obj)
{
    cra_weak_ref_head_unref(&obj->head);
    // no need to destroy weak ref control block,
    // it is the object's field
}

static struct TestWeakRefObj *
testweakrefobj_acquire(struct TestWeakRefObj *obj)
{
    if (cra_weak_ref_head_acquire(&obj->head))
        return obj;
    return NULL;
}

static void
testweakrefobj_release(struct TestWeakRefObj *obj)
{
    if (cra_weak_ref_head_release(&obj->head))
    {
        testweakrefobj_uninit(obj);

        assert_always(obj->head.weak_cnt > 0);
        testweakrefobj_weak_unref(obj); // IMPORTANT
    }
}

void
test_weak_ref_intrusive(void)
{
    struct TestWeakRefObj obj, *obj2;

    testweakrefobj_init(&obj);
    assert_always(flag == 1);
    assert_always(obj.head.cnt == 1);
    assert_always(obj.head.weak_cnt == 1);

    testweakrefobj_weak_ref(&obj);
    assert_always(obj.head.cnt == 1);
    assert_always(obj.head.weak_cnt == 2);

    obj2 = testweakrefobj_acquire(&obj);
    assert_always(obj.head.weak_cnt == 2);
    assert_always(obj.head.cnt == 2);
    assert_always(obj2 != NULL);
    assert_always(obj2 == &obj);

    testweakrefobj_release(obj2);
    assert_always(obj.head.cnt == 1);
    assert_always(obj.head.weak_cnt == 2);

    assert_always(flag == 1);
    testweakrefobj_release(&obj);
    assert_always(obj.head.cnt == 0);
    assert_always(obj.head.weak_cnt == 1);
    assert_always(flag == 2);

    testweakrefobj_weak_unref(&obj);
    assert_always(obj.head.cnt == 0);
    assert_always(obj.head.weak_cnt == 0);

    // ===================================

    testweakrefobj_init(&obj);
    assert_always(obj.head.cnt == 1);
    assert_always(obj.head.weak_cnt == 1);
    assert_always(flag == 1);

    testweakrefobj_release(&obj);
    assert_always(obj.head.cnt == 0);
    assert_always(obj.head.weak_cnt == 0);
    assert_always(flag == 2);
}

#endif // end intrusive weak ref

#if 1 // ref(multi threads)

cra_atomic_int32_t ref_multi_num = 0;

static void
ref_multi_check(void *pi)
{
    CRA_UNUSED(pi);
    cra_atomic_inc(&ref_multi_num, CRA_MO_RELAXED);
}

static void
ref_multi_task_func(const CraThrdPoolArgs1 *args)
{
    CraRef *ref = (CraRef *)args->arg1;

    // cra_msleep(20);

    cra_ref_unref(ref);
}

void
test_ref_multi(void)
{
    CraRef     *ref;
    CraThrdPool pool;

    ref = cra_ref_make(sizeof(int), ref_multi_check);
    assert_always(ref != NULL);

    cra_thrdpool_init(&pool, 8, CRA_THRDPOOL_TASK_INFINITE);

    for (int i = 0; i < 10000; i++)
    {
        cra_ref_ref(ref);
        cra_thrdpool_add_task1(&pool, ref_multi_task_func, ref);
    }

    cra_ref_unref(ref);

    cra_thrdpool_wait(&pool);

    printf("ref_multi_num = %d\n", ref_multi_num);
    assert_always(ref_multi_num == 1);

    cra_thrdpool_uninit(&pool);
}

#endif // end ref(multi threads)

#if 1 // weak ref(multi threads)

cra_atomic_int32_t weak_ref_multi_num = 0;
cra_atomic_int32_t weak_ref_multi_acquire_fail_num = 0;
cra_atomic_int32_t weak_ref_multi_acquire_success_num = 0;

static void
weak_ref_multi_check(void *pi)
{
    CRA_UNUSED(pi);
    cra_atomic_inc(&weak_ref_multi_num, CRA_MO_RELAXED);
}

static void
weak_ref_multi_task_func(const CraThrdPoolArgs1 *args)
{
    CraWeakRef *ref = (CraWeakRef *)args->arg1;

    if (cra_weak_ref_acquire(ref) == NULL)
    {
        cra_atomic_inc(&weak_ref_multi_acquire_fail_num, CRA_MO_RELAXED);
    }
    else
    {
        cra_atomic_inc(&weak_ref_multi_acquire_success_num, CRA_MO_RELAXED);

        // cra_msleep(20);

        cra_weak_ref_release(ref);
    }

    cra_weak_ref_unref(ref);
}

void
test_weak_ref_multi(void)
{
    CraWeakRef *ref;
    CraThrdPool pool;

    ref = cra_weak_ref_make(sizeof(int), weak_ref_multi_check);
    assert_always(ref != NULL);

    cra_thrdpool_init(&pool, 8, CRA_THRDPOOL_TASK_INFINITE);

    for (int i = 0; i < 1000; i++)
    {
        cra_weak_ref_ref(ref);
        cra_thrdpool_add_task1(&pool, weak_ref_multi_task_func, ref);
    }

    cra_weak_ref_release_clear(&ref);

    cra_thrdpool_wait(&pool);

    printf("weak_ref_multi_num = %d\n"
           "weak_ref_multi_acquire_fail_num = %d\n"
           "weak_ref_multi_acquire_success_num = %d\n",
           weak_ref_multi_num, weak_ref_multi_acquire_fail_num, weak_ref_multi_acquire_success_num);
    assert_always(weak_ref_multi_num == 1);

    cra_thrdpool_uninit(&pool);
}

#endif // end weak ref(multi threads)

int
main(void)
{
    test_ref_make();
    test_ref_take();
    test_ref_intrusive();

    test_weak_ref_make();
    test_weak_ref_take();
    test_weak_ref_intrusive();

    test_ref_multi();
    test_weak_ref_multi();

    cra_memory_leak_report();
    return 0;
}
