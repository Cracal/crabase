/**
 * @file test-deque.c
 * @author Cracal
 * @brief test double-ended queue
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "collections/cra_deque.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include <time.h>

void
test_new_delete(void)
{
    CraDeque deque, *deque2;

    cra_deque_init0(int, &deque, CRA_DEQUE_INFINITE, true);
    deque2 = cra_alloc(CraDeque);
    cra_deque_init0(int, deque2, 0, false);

    cra_deque_uninit(&deque);
    cra_deque_uninit(deque2);
    cra_dealloc(deque2);
}

void
test_insert_and_pop_at(void)
{
    CraDequeIter it;
    int         *valptr, i;
    CraDeque    *deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, CRA_DEQUE_INFINITE, true);

    assert_always(cra_deque_insert(deque, 0, &(int){ 0 }));
    assert_always(cra_deque_insert(deque, 1, &(int){ 2 }));
    assert_always(cra_deque_insert(deque, 1, &(int){ 1 }));

    i = 0;
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)(void **)&valptr); i++)
        assert_always(i == *valptr);
    printf("\n");

    for (i = 0; i < 100; i++)
        assert_always(cra_deque_insert(deque, 1, &(int){ i + 10 }));
    cra_deque_push(deque, &(int){ 100 });
    cra_deque_push_left(deque, &(int){ -100 });

    int val;
    assert_always(cra_deque_pop_at(deque, 0, &val) && val == -100);
    assert_always(cra_deque_pop_at(deque, deque->count - 1, &val) && val == 100);

    i = 0;
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)(void **)&valptr);)
    {
        printf("%d  ", *valptr);
        if (i == 1)
            i = 109;
        else if (i == 9)
            i = 1;
        assert_always(i == *valptr);
        if (i < 3)
            i++;
        else
            i--;
    }
    printf("\n");

    for (i = 109; i >= 10; i--)
        assert_always(cra_deque_pop_at(deque, 1, &val) && val == i);

    assert_always(cra_deque_pop_at(deque, 0, &val) && val == 0);
    assert_always(cra_deque_pop_at(deque, 0, &val) && val == 1);
    assert_always(cra_deque_pop_at(deque, 0, &val) && val == 2);
    assert_always(deque->count == 0);

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_push(void)
{
    CraDequeIter it;
    int         *valptr, i;
    CraDeque    *deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, CRA_DEQUE_INFINITE, true);

    assert_always(cra_deque_push(deque, &(int){ 100 }) && cra_deque_peek(deque, &i) && i == 100);
    cra_deque_clear(deque);
    assert_always(cra_deque_push_left(deque, &(int){ 200 }) && cra_deque_peek(deque, &i) && i == 200);
    cra_deque_clear(deque);
    assert_always(cra_deque_insert(deque, 0, &(int){ 300 }) && cra_deque_peek(deque, &i) && i == 300);
    cra_deque_clear(deque);

    for (i = 0; i < 1000; i++)
        assert_always(cra_deque_push(deque, &i));

    assert_always(cra_deque_push_left(deque, &(int){ -1 }));
    assert_always(cra_deque_push_left(deque, &(int){ -2 }));
    assert_always(cra_deque_insert(deque, 4, &(int){ 4000 }));
    assert_always(cra_deque_insert(deque, 4, &(int){ 40000 }));
    assert_always(cra_deque_push(deque, &(int){ 11 }));
    assert_always(cra_deque_push(deque, &(int){ 12 }));

    assert_always(!cra_deque_insert(deque, 10000, &(int){ 1 }));

    i = -2;
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr); i++)
    {
        // printf("%d  ", *valptr);
        if (i == 2)
            i = 40000;
        else if (i == 40001)
            i = 4000;
        else if (i == 4001)
            i = 2;
        else if (i == 1000)
            i = 11;
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_deque_clear(deque);

    for (i = 0; i < 1000; i++)
        assert_always(cra_deque_push_left(deque, &i));

    i = 999;
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr); i--)
    {
        // printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);

    // over que max

    printf("\n");

    deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, 6, false);

    for (i = 0; i < 20; i++)
        assert_always(cra_deque_push_left(deque, &i));
    assert_always(deque->count == 6);

    printf("\n");

    cra_deque_clear(deque);

    printf("\n");

    for (i = 0; i < 20; i++)
        assert_always(cra_deque_push(deque, &i));
    assert_always(deque->count == 6);

    printf("\n");

    cra_deque_clear(deque);

    for (i = 0; i < 20; i++)
        assert_always(cra_deque_insert(deque, CRA_MIN(deque->count, (size_t)i), &i));
    assert_always(deque->count == 6);

    printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_pop(void)
{
    int       val, i;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, CRA_DEQUE_INFINITE, true);

    assert_always(!cra_deque_pop(deque, &i));
    assert_always(!cra_deque_pop_left(deque, &i));
    assert_always(!cra_deque_pop_at(deque, deque->count - 1, &i));
    assert_always(!cra_deque_remove_at(deque, 0));
    assert_always(!cra_deque_remove_at(deque, deque->count - 1));

    for (i = 1; i <= 10000; i++)
        cra_deque_push(deque, &i);

    assert_always(cra_deque_pop(deque, &val) && val == 10000);
    assert_always(cra_deque_pop_left(deque, &val) && val == 1);

    cra_deque_push(deque, &(int){ 200000 });
    cra_deque_push_left(deque, &(int){ -100 });

    assert_always(cra_deque_pop(deque, &val) && val == 200000);
    assert_always(cra_deque_pop_left(deque, &val) && val == -100);

    i = 9999;
    while (deque->count > 0)
    {
        assert_always(cra_deque_pop(deque, &val));
        assert_always(val == i--);
    }

    assert_always(!cra_deque_pop(deque, NULL));
    assert_always(!cra_deque_pop_left(deque, NULL));

    for (i = 1; i <= 10000; i++)
        cra_deque_push_left(deque, &i);

    i = 10000;
    while (deque->count > 0)
    {
        assert_always(cra_deque_pop_left(deque, &val));
        assert_always(val == i--);
    }

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_set(void)
{
    CraDequeIter it;
    int          val, i;
    int         *valptr;
    CraDeque    *deque = cra_alloc(CraDeque);
    assert_always(deque != NULL);
    cra_deque_init0(int, deque, CRA_DEQUE_INFINITE, true);

    for (i = 0; i < 10000; i++)
        assert_always(cra_deque_push(deque, &i));

    assert_always(cra_deque_set(deque, 3, &(int){ 30000 }));
    assert_always(cra_deque_get(deque, 3, &val) && val == 30000);
    assert_always(cra_deque_set_and_pop_old(deque, 6, &(int){ 60000 }, &val) && val == 6);
    assert_always(cra_deque_get(deque, 6, &val) && val == 60000);
    assert_always(!cra_deque_set(deque, 100000, &(int){ 1000 }));
    assert_always(!cra_deque_set_and_pop_old(deque, 100000, &(int){ 1000 }, &val));

    i = 0;
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr); i++)
    {
        // printf("%d  ", *valptr);
        if (i == 3)
            i = 30000;
        else if (i == 30001)
            i = 4;
        else if (i == 6)
            i = 60000;
        else if (i == 60001)
            i = 7;
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_get(void)
{
    CraDequeIter it;
    CraDeque     deque;
    int          val, *valptr1, *valptr2;

    cra_deque_init0(int, &deque, 10000, false);
    for (int i = 0; i < 10000; i++)
        cra_deque_push_left(&deque, &i);

    size_t i = 0;
    for (cra_deque_iter_init(&deque, &it); cra_deque_iter_next(&it, (void **)&valptr1); i++)
    {
        assert_always(cra_deque_get(&deque, i, &val));
        assert_always(cra_deque_get_ptr(&deque, i, (void **)&valptr2));
        assert_always(val == *valptr1 && *valptr1 == *valptr2);
        // printf("%d  ", val);
    }
    // printf("\n");

    cra_deque_uninit(&deque);
}

void
test_peek(void)
{
    CraDequeIter it;
    int          val, *valptr;
    CraDeque    *deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, 200, false);

    for (int i = 0; i < 200; i++)
    {
        if (i % 2 == 0)
            cra_deque_push(deque, &i);
        else
            cra_deque_push_left(deque, &i);
    }

    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    assert_always(cra_deque_peek(deque, &val) && val == 198);
    assert_always(cra_deque_peek_ptr(deque, (void **)(void **)&valptr) && val == *valptr);

    assert_always(cra_deque_peek_left(deque, &val) && val == 199);
    assert_always(cra_deque_peek_left_ptr(deque, (void **)(void **)&valptr) && val == *valptr);

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_reverse(void)
{
    CraDequeIter it;
    int         *valptr, i;
    CraDeque    *deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, CRA_DEQUE_INFINITE, true);

    for (i = 0; i < 2000; i++)
        cra_deque_push(deque, &i);

    // printf("before reverse: ");
    i = 0;
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr); i++)
    {
        // printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_deque_reverse(deque);

    // printf("after  reverse: ");
    i = 1999;
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr); i--)
    {
        // printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

typedef struct
{
    int   i;
    float f;
} A_s;

static void
copy_as(const void *from, void *to)
{
    A_s *v = *(A_s **)from;
    A_s *ret = cra_malloc(sizeof(A_s));
    ret->i = v->i;
    ret->f = v->f;
    *(void **)to = ret;
}

void
test_clone(void)
{
    CraDeque *que;
    CraDeque *deque;

    deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, 10000, false);

    for (int i = 0; i < 1000; i++)
        cra_deque_push_left(deque, &i);

    // int *valptr;
    // for (CraDequeIter it = cra_deque_iter_init(deque); cra_deque_iter_next(&it, (void**)&valptr);)
    //     printf("%d  ", *valptr);
    // printf("\n");

    que = cra_deque_clone(deque, NULL);
    assert_always(que != NULL);
    assert_always(que->count == deque->count);

    int val1, val2, *valptr1, *valptr2;
    while (que->count > 0)
    {
        cra_deque_peek_ptr(deque, (void **)(void **)&valptr1);
        cra_deque_peek_ptr(que, (void **)(void **)&valptr2);
        assert_always(*valptr1 == *valptr2);
        cra_deque_pop(deque, &val1);
        cra_deque_pop(que, &val2);
        assert_always(val1 == val2);
    }

    cra_deque_uninit(que);
    cra_dealloc(que);
    cra_deque_uninit(deque);
    cra_dealloc(deque);

    deque = cra_alloc(CraDeque);
    cra_deque_init0(A_s *, deque, 10000, false);

    A_s *as;
    for (int i = 0; i < 1000; i++)
    {
        as = cra_malloc(sizeof(A_s));
        as->i = i + 1;
        as->f = i + 1.5f;
        cra_deque_push(deque, &as);
    }

    // A_s **asptr;
    // for (CraDequeIter it = cra_deque_iter_init(deque); cra_deque_iter_next(&it, (void **)&asptr);)
    //     printf("{i: %d, f: %f}  ", (*asptr)->i, (*asptr)->f);
    // printf("\n");

    que = cra_deque_clone(deque, copy_as);
    assert_always(que != NULL);
    assert_always(que->count == deque->count);

    A_s  *valas1, *valas2;
    A_s **valasptr1, **valasptr2;
    while (que->count > 0)
    {
        cra_deque_peek_ptr(deque, (void **)&valasptr1);
        cra_deque_peek_ptr(que, (void **)&valasptr2);
        assert_always((*valasptr1)->i == (*valasptr2)->i);
        assert_always((*valasptr1)->f == (*valasptr2)->f);
        assert_always(*valasptr1 != *valasptr2);
        cra_deque_pop_left(deque, &valas1);
        cra_deque_pop_left(que, &valas2);
        cra_dealloc(valas1);
        cra_dealloc(valas2);
    }

    cra_deque_uninit(que);
    cra_dealloc(que);
    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_foreach(void)
{
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, 100, false);

    int         *valptr;
    CraDequeIter it;

    printf("foreach(    empty): ");
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
        assert_always(false);
    }
    printf("\n");

    for (int i = 0; i < 10; i++)
        cra_deque_push(deque, &i);

    printf("foreach(not empty): ");
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
    }
    printf("\n");

    cra_deque_clear(deque);

    printf("foreach(    empty): ");
    for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
        assert_always(false);
    }
    printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_test(void)
{
    CraDequeIter it;
    CraDeque    *deque = cra_alloc(CraDeque);
    cra_deque_init0(int, deque, CRA_DEQUE_INFINITE, true);

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 10; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_deque_push(deque, &j);

        n = (rand() + 1) % deque->count;
        for (j = 0; j < n; j++)
        {
            cra_deque_pop_left(deque, &v);
            assert_always(v == j);
        }

        for (; cra_deque_pop_left(deque, &v); j++)
            assert_always(v == j);
    }
    assert_always(deque->count == 0);

    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_deque_push_left(deque, &j);

        n = (rand() + 1) % deque->count;
        for (j = 0; j < n; j++)
        {
            cra_deque_pop(deque, &v);
            assert_always(v == j);
        }

        for (; cra_deque_pop(deque, &v); j++)
            assert_always(v == j);
    }
    assert_always(deque->count == 0);

    int *pv;
    int  idx, last_idx;
    int  check[10000];
    bzero(check, sizeof(check));
    for (i = 0; i < 100; i++)
    {
        last_idx = 0;
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
        {
            idx = deque->count == 0 ? 0 : (rand() % deque->count);
            cra_deque_insert(deque, idx, &j);
            if (idx > last_idx)
            {
                last_idx = idx;
            }
            else
            {
                memmove(check + idx + 1, check + idx, (last_idx - idx) * sizeof(int));
                if (deque->count > 1)
                    last_idx++;
            }
            check[idx] = j;
        }
        j = 0;
        for (cra_deque_iter_init(deque, &it); cra_deque_iter_next(&it, (void **)&pv); j++)
        {
            assert_always(*pv == check[j]);
        }

        while (true)
        {
            idx = deque->count == 0 ? 0 : (rand() % deque->count);
            if (!cra_deque_pop_at(deque, idx, &j))
                break;

            assert_always(check[idx] == j);
            memmove(check + idx, check + idx + 1, (last_idx - idx + 1) * sizeof(int));
            last_idx--;
        }
    }
    assert_always(deque->count == 0);

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

int
main(void)
{
    test_new_delete();
    test_insert_and_pop_at();
    test_push();
    test_pop();
    test_set();
    test_get();
    test_peek();
    test_reverse();
    test_clone();
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
