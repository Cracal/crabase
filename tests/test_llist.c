/**
 * @file test-llist.c
 * @author Cracal
 * @brief test linked list
 * @version 0.1
 * @date 2024-10-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "cra_malloc.h"
#include "collections/cra_llist.h"

static void _print_int(void *pi) { printf("val: %d\n", *(int *)pi); }

void test_new_delete(void)
{
    CraLList list, *list2;

    cra_llist_init0(int, &list, true, NULL);
    list2 = cra_alloc(CraLList);
    cra_llist_init0(int, list2, false, NULL);

    cra_llist_uninit(&list);
    cra_llist_uninit(list2);
    cra_dealloc(list2);
}

void test_add(void)
{
    CraLList *list;
    int *valptr;

    list = cra_alloc(CraLList);
    assert_always(list != NULL);
    cra_llist_init0(int, list, true, _print_int);

    for (int i = 0; i < 10; i++)
        assert_always(cra_llist_append(list, &i));

    assert_always(cra_llist_prepend(list, &(int){-1}));
    assert_always(cra_llist_prepend(list, &(int){-2}));
    assert_always(cra_llist_insert(list, 4, &(int){400}));
    assert_always(cra_llist_insert(list, 4, &(int){4000}));
    assert_always(cra_llist_append(list, &(int){11}));
    assert_always(cra_llist_append(list, &(int){12}));

    assert_always(!cra_llist_insert(list, 10000, &(int){1}));

    for (CraLListIter it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

void test_remove(void)
{
    int *valptr;
    CraLList *list = cra_alloc(CraLList);
    assert_always(list != NULL);
    cra_llist_init0(int, list, true, NULL);

    assert_always(!cra_llist_remove_back(list));
    assert_always(!cra_llist_remove_front(list));
    assert_always(!cra_llist_remove_at(list, 100));

    for (int i = 0; i < 100; i++)
        assert_always(cra_llist_append(list, &i));

    for (int i = 0; i < 100; i++)
        assert_always(cra_llist_remove_back(list));

    for (int i = 1; i <= 100; i++)
        assert_always(cra_llist_append(list, &i));

    int val;
    assert_always(cra_llist_pop(list, 70 - 1, &val) && val == 70);
    assert_always(cra_llist_pop(list, 60 - 1, &val) && val == 60);
    assert_always(cra_llist_remove_at(list, 38 - 1));
    assert_always(cra_llist_remove_at(list, 34 - 1));
    assert_always(cra_llist_pop_front(list, &val) && val == 1);
    assert_always(cra_llist_pop_front(list, &val) && val == 2);
    assert_always(cra_llist_remove_front(list));
    assert_always(cra_llist_remove_front(list));
    assert_always(cra_llist_pop_back(list, &val) && val == 100);
    assert_always(cra_llist_pop_back(list, &val) && val == 99);
    assert_always(cra_llist_remove_back(list));
    assert_always(cra_llist_remove_back(list));

    for (CraLListIter it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

static bool remove_even(void *val, void *arg)
{
    CRA_UNUSED_VALUE(arg);
    return (*(int *)val) % 2 == 0;
}

static bool remove_all(void *val, void *arg)
{
    CRA_UNUSED_VALUE(val);
    CRA_UNUSED_VALUE(arg);
    return true;
}

void test_remove_match(void)
{
    int *valptr;
    size_t rmcnt;
    CraLList *list;

    list = cra_alloc(CraLList);
    cra_llist_init0(int, list, true, _print_int);

    for (int i = 0; i < 10; i++)
        cra_llist_append(list, &i);

    rmcnt = cra_llist_remove_match(list, remove_even, NULL);
    printf("removed %zu elements.\n", rmcnt);
    assert_always(rmcnt == 5);

    for (CraLListIter it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    rmcnt = cra_llist_remove_match(list, remove_all, NULL);
    printf("removed %zu elements.\n", rmcnt);
    assert_always(rmcnt == 5);

    cra_llist_uninit(list);
    cra_dealloc(list);
}

void test_set(void)
{
    int val;
    int *valptr;
    CraLList *list = cra_alloc(CraLList);
    cra_llist_init0(int, list, true, NULL);
    assert_always(list != NULL);

    for (int i = 0; i < 10; i++)
        assert_always(cra_llist_append(list, &i));

    assert_always(cra_llist_set(list, 3, &(int){300}));
    assert_always(cra_llist_set_and_pop_old(list, 6, &(int){600}, &val) && val == 6);
    assert_always(!cra_llist_set(list, 100, &(int){1000}));
    assert_always(!cra_llist_set_and_pop_old(list, 100, &(int){1000}, &val));

    for (CraLListIter it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

void test_get(void)
{
    CraLList list;
    int val, *valptr1, *valptr2;

    cra_llist_init0(int, &list, false, NULL);
    for (int i = 0; i < 10; i++)
        cra_llist_prepend(&list, &i);

    size_t i = 0;
    for (CraLListIter it = cra_llist_iter_init(&list); cra_llist_iter_next(&it, (void **)&valptr1); i++)
    {
        assert_always(cra_llist_get(&list, i, &val));
        assert_always(cra_llist_get_ptr(&list, i, (void **)&valptr2));
        assert_always(val == *valptr1 && *valptr1 == *valptr2);
        printf("%d  ", val);
    }
    printf("\n");

    cra_llist_uninit(&list);
}

void test_reverse(void)
{
    int *valptr;
    CraLList *list = cra_alloc(CraLList);
    cra_llist_init0(int, list, true, NULL);
    for (int i = 0; i < 10; i++)
        cra_llist_append(list, &i);

    printf("before reverse: ");
    for (CraLListIter it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_llist_reverse(list);
    printf("after  reverse: ");
    for (CraLListIter it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

typedef struct
{
    int i;
    float f;
} A_s;

static void copy_as(const void *from, void *to)
{
    A_s *v = *(A_s **)from;
    A_s *ret = cra_malloc(sizeof(A_s));
    ret->i = v->i;
    ret->f = v->f;
    *(void **)to = ret;
}

static void free_as(void *val) { cra_free(*(void **)val); }

void test_clone(void)
{
    CraLList *list1, *list2;

    list1 = cra_alloc(CraLList);
    cra_llist_init0(int, list1, false, NULL);
    for (int i = 0; i < 8; i++)
        cra_llist_append(list1, &i);

    list2 = cra_llist_clone(list1, NULL);
    assert_always(list2 != NULL);
    assert_always(list1->count == list2->count);

    int val1, val2;
    for (size_t i = 0; i < list1->count; i++)
    {
        assert_always(cra_llist_get(list1, i, &val1));
        assert_always(cra_llist_get(list2, i, &val2));
        assert_always(val1 == val2);
    }

    cra_llist_uninit(list1);
    cra_dealloc(list1);
    cra_llist_uninit(list2);
    cra_dealloc(list2);

    // ===================

    A_s *as1, *as2;
    list1 = cra_alloc(CraLList);
    cra_llist_init0(A_s *, list1, true, free_as);
    for (int i = 0; i < 8; i++)
    {
        as1 = (A_s *)cra_malloc(sizeof(A_s));
        as1->i = i + 1;
        as1->f = i + .5f;
        cra_llist_append(list1, &as1);
    }

    list2 = cra_llist_clone(list1, copy_as);
    assert_always(list2 != NULL);
    assert_always(list1->count == list2->count);

    for (size_t i = 0; i < list1->count; i++)
    {
        // assert_always(cra_llist_get(list1, i, &as1));
        // assert_always(cra_llist_get(list2, i, &as2));
        // assert_always(as1->i == as2->i);
        // assert_always(as1->f == as2->f);
        assert_always(cra_llist_get_ptr(list1, i, (void *)&as1));
        assert_always(cra_llist_get_ptr(list2, i, (void *)&as2));
        assert_always((*(A_s **)as1)->i == (*(A_s **)as2)->i);
        assert_always((*(A_s **)as1)->f == (*(A_s **)as2)->f);
    }

    cra_llist_uninit(list1);
    cra_dealloc(list1);
    cra_llist_uninit(list2);
    cra_dealloc(list2);
}

static int comare_int1(const void *a, const void *b) { return *(int *)a - *(int *)b; }
static int comare_int2(const void *a, const void *b) { return *(int *)b - *(int *)a; }

void test_sort(void)
{
    int *valptr;
    CraLList list, *list2;

    cra_llist_init0(int, &list, false, NULL);

    assert_always(cra_llist_append(&list, &(int){8}));
    assert_always(cra_llist_append(&list, &(int){7}));
    assert_always(cra_llist_append(&list, &(int){1}));
    assert_always(cra_llist_append(&list, &(int){3}));
    assert_always(cra_llist_append(&list, &(int){0}));
    assert_always(cra_llist_append(&list, &(int){9}));
    assert_always(cra_llist_append(&list, &(int){6}));
    assert_always(cra_llist_append(&list, &(int){4}));
    assert_always(cra_llist_append(&list, &(int){5}));
    assert_always(cra_llist_append(&list, &(int){2}));

    printf("before sort: ");
    for (CraLListIter it = cra_llist_iter_init(&list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    list2 = cra_llist_clone(&list, NULL);
    cra_llist_sort(list2, comare_int1);
    printf("after  sort(ASC) : ");
    for (CraLListIter it = cra_llist_iter_init(list2); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");
    cra_llist_uninit(list2);
    cra_dealloc(list2);

    list2 = cra_llist_clone(&list, NULL);
    cra_llist_sort(list2, comare_int2);
    printf("after  sort(DESC): ");
    for (CraLListIter it = cra_llist_iter_init(list2); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");
    cra_llist_uninit(list2);
    cra_dealloc(list2);

    // add sort

    cra_llist_clear(&list);
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){8}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){7}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){1}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){3}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){0}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){9}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){6}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){4}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){5}));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){2}));
    printf("add sort(ASC) : ");
    for (CraLListIter it = cra_llist_iter_init(&list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_llist_clear(&list);
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){8}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){7}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){1}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){3}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){0}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){9}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){6}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){4}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){5}));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){2}));
    printf("add sort(DESC): ");
    for (CraLListIter it = cra_llist_iter_init(&list); cra_llist_iter_next(&it, (void **)&valptr);)
        printf("%d  ", *valptr);
    printf("\n");

    cra_llist_uninit(&list);
}

void test_foreach(void)
{
    CraLList *list = cra_alloc(CraLList);
    cra_llist_init0(int, list, false, NULL);

    int *valptr;
    CraLListIter it;

    printf("无元素时遍历: ");
    for (it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
    }
    printf("\n");

    for (int i = 0; i < 10; i++)
        cra_llist_append(list, &i);

    printf("有元素时遍历: ");
    for (it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
    }
    printf("\n");

    cra_llist_clear(list);

    printf("无元素时遍历: ");
    for (it = cra_llist_iter_init(list); cra_llist_iter_next(&it, (void **)&valptr);)
    {
        printf("%d ", *valptr);
    }
    printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

int main(void)
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

    cra_memory_leak_report(stdout);
    return 0;
}
