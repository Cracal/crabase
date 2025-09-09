/**
 * @file test-list.c
 * @author Cracal
 * @brief test array list
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "collections/cra_alist.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include <time.h>

void
test_new_delete(void)
{
    CraAList list, *list2;

    cra_alist_init_size0(int, &list, 4, true);
    list2 = cra_alloc(CraAList);
    cra_alist_init_size0(int, list2, 2, false);

    cra_alist_uninit(&list);
    cra_alist_uninit(list2);
    cra_dealloc(list2);
}

void
test_add(void)
{
    CraAListIter it;
    CraAList    *list;
    int         *valptr, i;

    list = cra_alloc(CraAList);
    assert_always(list != NULL);
    cra_alist_init0(int, list, true);

    assert_always(cra_alist_append(list, &(int){ 100 }) && cra_alist_get(list, 0, &i) && i == 100);
    cra_alist_clear(list);
    assert_always(cra_alist_prepend(list, &(int){ 200 }) && cra_alist_get(list, 0, &i) && i == 200);
    cra_alist_clear(list);
    assert_always(cra_alist_insert(list, 0, &(int){ 300 }) && cra_alist_get(list, 0, &i) && i == 300);
    cra_alist_clear(list);

    for (i = 0; i < 1000; i++)
        assert_always(cra_alist_append(list, &i));

    assert_always(cra_alist_prepend(list, &(int){ -1 }));
    assert_always(cra_alist_prepend(list, &(int){ -2 }));
    assert_always(cra_alist_insert(list, 4, &(int){ 4000 }));
    assert_always(cra_alist_insert(list, 4, &(int){ 40000 }));
    assert_always(cra_alist_append(list, &(int){ 11 }));
    assert_always(cra_alist_append(list, &(int){ 12 }));

    assert_always(!cra_alist_insert(list, 10000, &(int){ 1 }));

    i = -2;
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr); i++)
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

    cra_alist_clear(list);

    for (i = 0; i < 1000; i++)
        assert_always(cra_alist_prepend(list, &i));

    i = 999;
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr); i--)
    {
        // printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_remove(void)
{
    CraAListIter it;
    int         *valptr;
    CraAList    *list = cra_alloc(CraAList);
    assert_always(list != NULL);
    cra_alist_init_size0(int, list, 100, true);

    assert_always(!cra_alist_remove_back(list));
    assert_always(!cra_alist_remove_front(list));
    assert_always(!cra_alist_remove_at(list, 100));

    for (int i = 0; i < 100; i++)
        assert_always(cra_alist_append(list, &i));

    for (int i = 0; i < 100; i++)
        assert_always(cra_alist_remove_back(list));

    for (int i = 1; i <= 100; i++)
        assert_always(cra_alist_append(list, &i));

    int val, val2;
    assert_always(cra_alist_pop_at(list, 70 - 1, &val) && val == 70);
    assert_always(cra_alist_pop_at(list, 60 - 1, &val) && val == 60);
    cra_alist_get(list, 38 - 1, &val);
    assert_always(cra_alist_remove_at(list, 38 - 1) && cra_alist_get(list, 38 - 1, &val2) && val != val2);
    cra_alist_get(list, 34 - 1, &val);
    assert_always(cra_alist_remove_at(list, 34 - 1) && cra_alist_get(list, 34 - 1, &val2) && val != val2);
    assert_always(cra_alist_pop_front(list, &val) && val == 1);
    assert_always(cra_alist_pop_front(list, &val) && val == 2);
    cra_alist_get(list, 0, &val);
    assert_always(cra_alist_remove_front(list) && cra_alist_get(list, 0, &val2) && val != val2);
    cra_alist_get(list, 0, &val);
    assert_always(cra_alist_remove_front(list) && cra_alist_get(list, 0, &val2) && val != val2);
    assert_always(cra_alist_pop_back(list, &val) && val == 100);
    assert_always(cra_alist_pop_back(list, &val) && val == 99);
    cra_alist_get(list, list->count - 1, &val);
    assert_always(cra_alist_remove_back(list) && cra_alist_get(list, list->count - 1, &val2) && val != val2);
    cra_alist_get(list, list->count - 1, &val);
    assert_always(cra_alist_remove_back(list) && cra_alist_get(list, list->count - 1, &val2) && val != val2);

    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
}

static bool
remove_even(const void *val, void *arg)
{
    CRA_UNUSED_VALUE(arg);
    return (*(int *)val) % 2 == 0;
}

static bool
remove_all(const void *val, void *arg)
{
    CRA_UNUSED_VALUE(val);
    CRA_UNUSED_VALUE(arg);
    return true;
}

void
test_remove_match(void)
{
    int         *valptr;
    size_t       rmcnt;
    CraAList    *list;
    CraAListIter it;

    list = cra_alloc(CraAList);
    cra_alist_init0(int, list, true);

    for (int i = 0; i < 100; i++)
        cra_alist_append(list, &i);

    rmcnt = cra_alist_remove_match(list, remove_even, NULL);
    printf("removed %zu elements.\n", rmcnt);
    assert_always(rmcnt == 50);

    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr);)
    {
        // printf("%d  ", *valptr);
        assert_always(*valptr % 2 != 0);
    }
    // printf("\n");

    rmcnt = cra_alist_remove_match(list, remove_all, NULL);
    printf("removed %zu elements.\n", rmcnt);
    assert_always(rmcnt == 50);
    assert_always(cra_alist_get_count(list) == 0);

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_set(void)
{
    int          val, i;
    int         *valptr;
    CraAListIter it;
    CraAList    *list = cra_alloc(CraAList);
    assert_always(list != NULL);
    cra_alist_init0(int, list, true);

    for (i = 0; i < 1000; i++)
        assert_always(cra_alist_append(list, &i));

    assert_always(cra_alist_set(list, 3, &(int){ 3000 }));
    assert_always(cra_alist_get(list, 3, &val) && val == 3000);
    assert_always(cra_alist_set_and_pop_old(list, 6, &(int){ 6000 }, &val) && val == 6);
    assert_always(cra_alist_get(list, 6, &val) && val == 6000);
    assert_always(!cra_alist_set(list, 1000, &(int){ 10000 }));
    assert_always(!cra_alist_set_and_pop_old(list, 1000, &(int){ 10000 }, &val));

    i = 0;
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr); i++)
    {
        // printf("%d  ", *valptr);
        if (i == 3)
            i = 3000;
        else if (i == 3001)
            i = 4;
        else if (i == 6)
            i = 6000;
        else if (i == 6001)
            i = 7;
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_get(void)
{
    CraAListIter it;
    CraAList     list;
    int          val, *valptr1, *valptr2;

    cra_alist_init0(int, &list, false);
    for (int i = 0; i < 1000; i++)
        cra_alist_prepend(&list, &i);

    size_t i = 0;
    for (cra_alist_iter_init(&list, &it); cra_alist_iter_next(&it, (void **)&valptr1); i++)
    {
        assert_always(cra_alist_get(&list, i, &val));
        assert_always(cra_alist_get_ptr(&list, i, (void **)&valptr2));
        assert_always(val == *valptr1 && *valptr1 == *valptr2);
        // printf("%d  ", val);
    }
    // printf("\n");

    cra_alist_uninit(&list);
}

void
test_reverse(void)
{
    CraAListIter it;
    int         *valptr, i;
    CraAList    *list = cra_alloc(CraAList);
    cra_alist_init_size0(int, list, 1000, true);
    for (i = 0; i < 1000; i++)
        cra_alist_append(list, &i);

    // printf("before reverse: ");
    i = 0;
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr); i++)
    {
        // printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_alist_reverse(list);

    // printf("after  reverse: ");
    i = 999;
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr); i--)
    {
        // printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    // printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
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
    A_s *ret = (A_s *)cra_malloc(sizeof(A_s));
    ret->i = v->i;
    ret->f = v->f;
    *(void **)to = ret;
}

void
test_clone(void)
{
    CraAList *list1, *list2;

    list1 = cra_alloc(CraAList);
    cra_alist_init_size0(int, list1, 1000, false);
    for (int i = 0; i < 1000; i++)
        cra_alist_append(list1, &i);

    list2 = cra_alist_clone(list1, NULL);
    assert_always(list2 != NULL);
    assert_always(list1->count == list2->count);

    int val1, val2;
    for (size_t i = 0; i < list1->count; i++)
    {
        assert_always(cra_alist_get(list1, i, &val1));
        assert_always(cra_alist_get(list2, i, &val2));
        assert_always(val1 == val2);
    }

    cra_alist_uninit(list1);
    cra_alist_uninit(list2);
    cra_dealloc(list1);
    cra_dealloc(list2);

    // ===================

    A_s *as1, *as2;
    list1 = cra_alloc(CraAList);
    cra_alist_init0(A_s *, list1, true);
    for (int i = 0; i < 800; i++)
    {
        as1 = (A_s *)cra_malloc(sizeof(A_s));
        as1->i = i + 1;
        as1->f = i + .5f;
        cra_alist_append(list1, &as1);
    }

    list2 = cra_alist_clone(list1, copy_as);
    assert_always(list2 != NULL);
    assert_always(list1->count == list2->count);

    for (size_t i = 0; i < list1->count; i++)
    {
        assert_always(cra_alist_get(list1, i, &as1));
        assert_always(cra_alist_get(list2, i, &as2));
        assert_always(as1->i == as2->i);
        assert_always(as1->f == as2->f);
        assert_always(as1 != as2);

        assert_always(cra_alist_get_ptr(list1, i, (void *)&as1));
        assert_always(cra_alist_get_ptr(list2, i, (void *)&as2));
        assert_always((*(A_s **)as1)->i == (*(A_s **)as2)->i);
        assert_always((*(A_s **)as1)->f == (*(A_s **)as2)->f);
        assert_always(*(A_s **)as1 != *(A_s **)as2);
    }
    CraAListIter it;
    for (cra_alist_iter_init(list1, &it); cra_alist_iter_next(&it, (void **)&as1);)
        cra_free(*(void **)as1);
    for (cra_alist_iter_init(list2, &it); cra_alist_iter_next(&it, (void **)&as1);)
        cra_free(*(void **)as1);

    cra_alist_uninit(list1);
    cra_alist_uninit(list2);
    cra_dealloc(list1);
    cra_dealloc(list2);
}

static int
comare_int1(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

static int
comare_int2(const void *a, const void *b)
{
    return *(int *)b - *(int *)a;
}

void
test_sort(void)
{
    CraAListIter it;
    int         *valptr, i;
    CraAList     list, *list2;

    cra_alist_init_size0(int, &list, 10, false);

    assert_always(cra_alist_append(&list, &(int){ 8 }));
    assert_always(cra_alist_append(&list, &(int){ 7 }));
    assert_always(cra_alist_append(&list, &(int){ 1 }));
    assert_always(cra_alist_append(&list, &(int){ 3 }));
    assert_always(cra_alist_append(&list, &(int){ 0 }));
    assert_always(cra_alist_append(&list, &(int){ 9 }));
    assert_always(cra_alist_append(&list, &(int){ 6 }));
    assert_always(cra_alist_append(&list, &(int){ 4 }));
    assert_always(cra_alist_append(&list, &(int){ 5 }));
    assert_always(cra_alist_append(&list, &(int){ 2 }));

    printf("before sort     : ");
    for (cra_alist_iter_init(&list, &it); cra_alist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    list2 = cra_alist_clone(&list, NULL);
    cra_alist_sort(list2, comare_int1);
    printf("after sort(ASC) : ");
    i = 0;
    for (cra_alist_iter_init(list2, &it); cra_alist_iter_next(&it, (void **)&valptr); i++)
    {
        printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    printf("\n");
    cra_alist_uninit(list2);
    cra_dealloc(list2);

    list2 = cra_alist_clone(&list, NULL);
    cra_alist_sort(list2, comare_int2);
    printf("after sort(DESC): ");
    i = 9;
    for (cra_alist_iter_init(list2, &it); cra_alist_iter_next(&it, (void **)&valptr); i--)
    {
        printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    printf("\n");
    cra_alist_uninit(list2);
    cra_dealloc(list2);

    // add sort

    cra_alist_clear(&list);
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 8 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 7 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 1 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 3 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 0 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 9 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 6 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 4 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 5 }));
    assert_always(cra_alist_add_sort(&list, comare_int1, &(int){ 2 }));
    printf("add sort(ASC) : ");
    i = 0;
    for (cra_alist_iter_init(&list, &it); cra_alist_iter_next(&it, (void **)&valptr); i++)
    {
        printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    printf("\n");

    cra_alist_clear(&list);
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 8 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 7 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 1 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 3 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 0 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 9 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 6 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 4 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 5 }));
    assert_always(cra_alist_add_sort(&list, comare_int2, &(int){ 2 }));
    printf("add sort(DESC): ");
    i = 9;
    for (cra_alist_iter_init(&list, &it); cra_alist_iter_next(&it, (void **)&valptr); i--)
    {
        printf("%d  ", *valptr);
        assert_always(i == *valptr);
    }
    printf("\n");

    cra_alist_clear(&list);

    srand((unsigned int)time(NULL));
    for (i = 0; i < 10000; i++)
        cra_alist_append(&list, &(int){ rand() });

    cra_alist_sort(&list, comare_int1);

    i = 0;
    for (cra_alist_iter_init(&list, &it); cra_alist_iter_next(&it, (void **)&valptr);)
    {
        assert_always(i <= *valptr);
        i = *valptr;
    }

    cra_alist_uninit(&list);
}

void
test_foreach(void)
{
    CraAList *list = cra_alloc(CraAList);
    cra_alist_init0(int, list, false);

    CraAListIter it;
    int         *valptr;

    printf("foreach(    empty): ");
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
        assert_always(false);
    }
    printf("\n");

    for (int i = 0; i < 10; i++)
        cra_alist_append(list, &i);

    printf("foreach(not empty): ");
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
    }
    printf("\n");

    cra_alist_clear(list);

    printf("foreach(    empty): ");
    for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
        assert_always(false);
    }
    printf("\n");

    cra_alist_uninit(list);
    cra_dealloc(list);
}

void
test_test(void)
{
    CraAListIter it;
    CraAList    *list = cra_alloc(CraAList);
    cra_alist_init0(int, list, true);

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 10; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_alist_append(list, &j);

        n = (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            cra_alist_pop_front(list, &v);
            assert_always(v == j);
        }

        for (; cra_alist_pop_front(list, &v); j++)
            assert_always(v == j);
    }
    assert_always(list->count == 0);

    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_alist_prepend(list, &j);

        n = (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            cra_alist_pop_back(list, &v);
            assert_always(v == j);
        }

        for (; cra_alist_pop_back(list, &v); j++)
            assert_always(v == j);
    }
    assert_always(list->count == 0);

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
            idx = list->count == 0 ? 0 : (rand() % list->count);
            cra_alist_insert(list, idx, &j);
            if (idx > last_idx)
            {
                last_idx = idx;
            }
            else
            {
                memmove(check + idx + 1, check + idx, (last_idx - idx) * sizeof(int));
                if (list->count > 1)
                    last_idx++;
            }
            check[idx] = j;
        }
        j = 0;
        for (cra_alist_iter_init(list, &it); cra_alist_iter_next(&it, (void **)&pv); j++)
        {
            assert_always(*pv == check[j]);
        }

        while (true)
        {
            idx = list->count == 0 ? 0 : (rand() % list->count);
            if (!cra_alist_pop_at(list, idx, &j))
                break;

            assert_always(check[idx] == j);
            memmove(check + idx, check + idx + 1, (last_idx - idx + 1) * sizeof(int));
            last_idx--;
        }
    }
    assert_always(list->count == 0);

    cra_alist_uninit(list);
    cra_dealloc(list);
}

int
main(void)
{
    test_new_delete();
    test_add();
    test_remove();
    test_remove_match();
    test_set();
    test_get();
    test_reverse();
    test_clone();
    test_sort();
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
