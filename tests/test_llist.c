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
#include "collections/cra_llist.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include <time.h>

void
test_list_node(void)
{
    CraLListNode *node = cra_llist_create_node(10);
    assert_always(node != NULL);
    cra_llist_destroy_node(node);

    CraLList list;
    cra_llist_init(int, &list);

    node = cra_llist_get_free_node(&list);
    assert_always(node != NULL);

    memcpy(node->val, &(int){ 100 }, sizeof(int));
    cra_llist_insert_node(&list, 0, node);
    assert_always(list.count == 1);

    cra_llist_remove_node(&list, node, false);
    assert_always(list.count == 0);
    assert_always(node->next == NULL && node->prev == NULL);

    cra_llist_insert_node(&list, 0, node);
    assert_always(list.count == 1);
    cra_llist_remove_node(&list, node, true);
    assert_always(list.count == 0);
    assert_always(list.nfreelist == 1);
    assert_always(list.free_list != NULL);
    node = NULL;
    node = cra_llist_get_free_node(&list);
    assert_always(node != NULL);
    assert_always(list.nfreelist == 0);
    assert_always(list.free_list == NULL);

    memcpy(node->val, &(int){ 1 }, sizeof(int));
    cra_llist_insert_node(&list, 0, node); // empty
    assert_always(list.count == 1);
    node = cra_llist_get_free_node(&list);
    memcpy(node->val, &(int){ 5 }, sizeof(int));
    cra_llist_insert_node(&list, 1, node); // tail
    assert_always(list.count == 2);
    node = cra_llist_get_free_node(&list);
    memcpy(node->val, &(int){ 3 }, sizeof(int));
    cra_llist_insert_node(&list, 1, node); // middle
    assert_always(list.count == 3);
    node = cra_llist_get_free_node(&list);
    memcpy(node->val, &(int){ 2 }, sizeof(int));
    cra_llist_insert_node(&list, 1, node); // middle clsoe to left
    assert_always(list.count == 4);
    node = cra_llist_get_free_node(&list);
    memcpy(node->val, &(int){ 4 }, sizeof(int));
    cra_llist_insert_node(&list, list.count - 1, node); // middle clsoe to right
    assert_always(list.count == 5);

    int val, i = 1;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        node = cra_llist_get_node(&list, i - 1);
        assert_always(node != NULL);
        memcpy(&val, node->val, sizeof(val));
        assert_always(i == val);
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i == val);
        printf("%d ", val);
        i++;
    }
    printf("\n");

    cra_llist_uninit(&list);
}

void
test_new_delete(void)
{
    CraLList *list, list2;

    list = cra_alloc(CraLList);
    assert(list);
    assert_always(cra_llist_init_with_size(int, list, 4));
    assert_always(list->count == 0);
    assert_always(list->head == NULL);
    assert_always(list->nfreelist == 4);
    assert_always(list->free_list != NULL);
    assert_always(list->itemsize == sizeof(int));
    assert_always(cra_llist_init(double, &list2));
    assert_always(list2.count == 0);
    assert_always(list2.head == NULL);
    assert_always(list2.nfreelist == 0);
    assert_always(list2.free_list == NULL);
    assert_always(list2.itemsize == sizeof(double));

    cra_llist_uninit(list);
    assert_always(memcmp(list, &(CraLList){ 0 }, sizeof(CraLList)) == 0);
    cra_dealloc(list);
    cra_llist_uninit(&list2);
    assert_always(memcmp(&list2, &(CraLList){ 0 }, sizeof(CraLList)) == 0);

    CRA_LLIST_INITIALIZABLE_PARAM_DEF(param, int);
    assert_always(cra_initializable_init(CRA_LLIST_INITIALIZABLE_I, &list2, 4, &param));
    assert_always(list2.count == 0);
    assert_always(list2.head == NULL);
    assert_always(list2.nfreelist == 4);
    assert_always(list2.free_list != NULL);
    assert_always(list2.itemsize == sizeof(int));

    cra_initializable_uninit(CRA_LLIST_INITIALIZABLE_I, &list2);
    assert_always(memcmp(&list2, &(CraLList){ 0 }, sizeof(CraLList)) == 0);
}

void
test_add(void)
{
    CraLList *list;
    int       val, i;

    list = cra_alloc(CraLList);
    assert_always(list != NULL);
    cra_llist_init(int, list);

    assert_always(cra_llist_append(list, &(int){ 100 }) && cra_llist_get(list, 0, &i) && i == 100);
    assert_always(list->count == 1);
    assert_always(list->head != NULL);
    assert_always(list->nfreelist == 0);
    assert_always(list->free_list == NULL);
    cra_llist_clear(list);
    assert_always(list->count == 0);
    assert_always(list->nfreelist == 1);
    assert_always(list->free_list != NULL);
    assert_always(cra_llist_prepend(list, &(int){ 200 }) && cra_llist_get(list, 0, &i) && i == 200);
    assert_always(list->count == 1);
    assert_always(list->head != NULL);
    assert_always(list->nfreelist == 0);
    assert_always(list->free_list == NULL);
    cra_llist_clear(list);
    assert_always(list->count == 0);
    assert_always(list->nfreelist == 1);
    assert_always(list->free_list != NULL);
    assert_always(cra_llist_insert(list, 0, &(int){ 300 }) && cra_llist_get(list, 0, &i) && i == 300);
    assert_always(list->count == 1);
    assert_always(list->head != NULL);
    assert_always(list->nfreelist == 0);
    assert_always(list->free_list == NULL);
    cra_llist_clear(list);
    assert_always(list->count == 0);
    assert_always(list->head == NULL);
    assert_always(list->nfreelist == 1);
    assert_always(list->free_list != NULL);

    for (i = 0; i < 1000; i++)
        assert_always(cra_llist_append(list, &i));
    assert_always(list->count == 1000);
    assert_always(list->nfreelist == 0);

    assert_always(cra_llist_prepend(list, &(int){ -1 }));
    assert_always(cra_llist_prepend(list, &(int){ -2 }));
    assert_always(cra_llist_insert(list, 4, &(int){ 4000 }));
    assert_always(cra_llist_insert(list, 4, &(int){ 40000 }));
    CraTwoVals vals_;
    vals_.val1_ref = &(int){ 11 };
    assert_always(cra_appendable_append(CRA_LLIST_APPENDABLE_I, list, &vals_));
    vals_.val1_ref = &(int){ 12 };
    assert_always(cra_appendable_append(CRA_LLIST_APPENDABLE_I, list, &vals_));
    assert_always(list->count == 1006);

    assert_always(!cra_llist_insert(list, 10000, &(int){ 1 }));
    assert_always(list->count == 1006);
    assert_always(list->nfreelist == 1);

    i = -2;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        // printf("%d  ", val);
        if (i == 2)
            i = 40000;
        else if (i == 40001)
            i = 4000;
        else if (i == 4001)
            i = 2;
        else if (i == 1000)
            i = 11;
        assert_always(i == val);
        i++;
    }
    // printf("\n");

    cra_llist_clear(list);
    assert_always(list->count == 0);
    assert_always(list->nfreelist == 1007);
    assert_always(list->free_list != NULL);

    for (i = 0; i < 1000; i++)
        assert_always(cra_llist_prepend(list, &i));
    assert_always(list->count == 1000);
    assert_always(list->nfreelist == 7);

    i = 999;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        // printf("%d  ", val);
        assert_always(i == val);
        i--;
    }
    // printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

void
test_remove(void)
{
    int       val, val2;
    CraLList *list = cra_alloc(CraLList);
    assert_always(list != NULL);
    cra_llist_init(int, list);

    assert_always(!cra_llist_remove_back(list));
    assert_always(list->count == 0);
    assert_always(!cra_llist_remove_front(list));
    assert_always(list->count == 0);
    assert_always(!cra_llist_remove_at(list, 100));
    assert_always(list->count == 0);

    for (int i = 0; i < 100; i++)
        assert_always(cra_llist_append(list, &i));
    assert_always(list->count == 100);
    assert_always(list->head != NULL);
    assert_always(list->nfreelist == 0);

    for (int i = 0; i < 100; i++)
        assert_always(cra_llist_remove_back(list));
    assert_always(list->count == 0);
    assert_always(list->head == NULL);
    assert_always(list->nfreelist == 100);
    assert_always(list->free_list != NULL);

    for (int i = 1; i <= 100; i++)
        assert_always(cra_llist_append(list, &i));
    assert_always(list->count == 100);
    assert_always(list->head != NULL);
    assert_always(list->nfreelist == 0);
    assert_always(list->free_list == NULL);

    assert_always(cra_llist_pop_at(list, 70 - 1, &val) && val == 70);
    assert_always(cra_llist_pop_at(list, 60 - 1, &val) && val == 60);
    cra_llist_get(list, 38 - 1, &val);
    assert_always(cra_llist_remove_at(list, 38 - 1) && cra_llist_get(list, 38 - 1, &val2) && val != val2);
    cra_llist_get(list, 34 - 1, &val);
    assert_always(cra_llist_remove_at(list, 34 - 1) && cra_llist_get(list, 34 - 1, &val2) && val != val2);
    assert_always(cra_llist_pop_front(list, &val) && val == 1);
    assert_always(cra_llist_pop_front(list, &val) && val == 2);
    cra_llist_get(list, 0, &val);
    assert_always(cra_llist_remove_front(list) && cra_llist_get(list, 0, &val2) && val != val2);
    cra_llist_get(list, 0, &val);
    assert_always(cra_llist_remove_front(list) && cra_llist_get(list, 0, &val2) && val != val2);
    assert_always(cra_llist_pop_back(list, &val) && val == 100);
    assert_always(cra_llist_pop_back(list, &val) && val == 99);
    cra_llist_get(list, list->count - 1, &val);
    assert_always(cra_llist_remove_back(list) && cra_llist_get(list, list->count - 1, &val2) && val != val2);
    cra_llist_get(list, list->count - 1, &val);
    assert_always(cra_llist_remove_back(list) && cra_llist_get(list, list->count - 1, &val2) && val != val2);
    assert_always(list->count == 88);
    assert_always(list->head != NULL);
    assert_always(list->nfreelist == 12);
    assert_always(list->free_list != NULL);

    CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        printf("%d  ", val);
    }
    printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

void
test_set(void)
{
    int       val, i;
    CraLList *list = cra_alloc(CraLList);
    assert_always(list != NULL);
    cra_llist_init(int, list);

    for (i = 0; i < 1000; i++)
        assert_always(cra_llist_append(list, &i));

    assert_always(cra_llist_set(list, 3, &(int){ 3000 }));
    assert_always(cra_llist_get(list, 3, &val) && val == 3000);
    assert_always(cra_llist_get_and_set(list, 6, &(int){ 6000 }, &val) && val == 6);
    assert_always(cra_llist_get(list, 6, &val) && val == 6000);
    assert_always(!cra_llist_set(list, 1000, &(int){ 10000 }));
    assert_always(!cra_llist_get_and_set(list, 1000, &(int){ 10000 }, &val));
    assert_always(list->count == 1000);
    assert_always(list->head != NULL);
    assert_always(list->nfreelist == 0);
    assert_always(list->free_list == NULL);

    i = 0;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        // printf("%d  ", val);
        if (i == 3)
            i = 3000;
        else if (i == 3001)
            i = 4;
        else if (i == 6)
            i = 6000;
        else if (i == 6001)
            i = 7;
        assert_always(i == val);
        i++;
    }
    // printf("\n");

    cra_llist_uninit(list);
    cra_dealloc(list);
}

void
test_get(void)
{
    CraLList list;
    int      val, val2, *pval;

    cra_llist_init(int, &list);
    for (int i = 0; i < 1000; i++)
        cra_llist_prepend(&list, &i);

    size_t i = 0;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        pval = cra_llist_get_ref(&list, i);
        assert_always(pval && val == *pval);
        assert_always(cra_llist_get(&list, i, &val2));
        assert_always(val2 == val);
        // printf("%d  ", val);
        i++;
    }
    // printf("\n");

    cra_llist_uninit(&list);
}

void
test_reverse(void)
{
    CraLList list, list2;
    int      val, val2, i;

    cra_llist_init_with_size(int, &list, 1000);
    cra_llist_init_with_size(int, &list2, 1000);

    for (i = 0; i < 1000; i++)
    {
        cra_llist_append(&list, &i);
        cra_llist_prepend(&list2, &i);
    }

    cra_llist_reverse(&list);

    i = 0;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        assert_always(cra_llist_get(&list2, i, &val2));
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(val == val2);
        i++;
    }

    cra_llist_uninit(&list);
    cra_llist_uninit(&list2);
}

static int
comare_int1(const int *a, const int *b)
{
    return *a - *b;
}

static int
comare_int2(const int *a, const int *b)
{
    return *b - *a;
}

void
test_sort(void)
{
    int      val, i;
    CraLList list;

    cra_llist_init(int, &list);

    assert_always(cra_llist_append(&list, &(int){ 8 }));
    assert_always(cra_llist_append(&list, &(int){ 7 }));
    assert_always(cra_llist_append(&list, &(int){ 1 }));
    assert_always(cra_llist_append(&list, &(int){ 3 }));
    assert_always(cra_llist_append(&list, &(int){ 0 }));
    assert_always(cra_llist_append(&list, &(int){ 9 }));
    assert_always(cra_llist_append(&list, &(int){ 6 }));
    assert_always(cra_llist_append(&list, &(int){ 4 }));
    assert_always(cra_llist_append(&list, &(int){ 5 }));
    assert_always(cra_llist_append(&list, &(int){ 2 }));

    printf("before sort     : ");
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        printf("%d  ", val);
    }
    printf("\n");

    cra_llist_sort(&list, comare_int1);
    printf("after sort(ASC) : ");
    i = 0;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");

    cra_llist_sort(&list, comare_int2);
    printf("after sort(DESC): ");
    i = 9;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i == val);
        printf("%d  ", val);
        i--;
    }
    printf("\n");

    // add sort

    cra_llist_clear(&list);
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 8 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 7 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 1 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 3 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 0 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 9 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 6 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 4 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 5 }));
    assert_always(cra_llist_add_sort(&list, comare_int1, &(int){ 2 }));
    printf("add sort(ASC)   : ");
    i = 0;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");

    cra_llist_clear(&list);
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 8 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 7 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 1 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 3 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 0 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 9 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 6 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 4 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 5 }));
    assert_always(cra_llist_add_sort(&list, comare_int2, &(int){ 2 }));
    printf("add sort(DESC)  : ");
    i = 9;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i == val);
        printf("%d  ", val);
        i--;
    }
    printf("\n");

    cra_llist_clear(&list);

    srand((unsigned int)time(NULL));
    for (i = 0; i < 10000; i++)
        cra_llist_append(&list, &(int){ rand() });

    cra_llist_sort(&list, comare_int1);

    i = 0;
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i <= val);
        i = val;
    }

    cra_llist_uninit(&list);
}

void
test_foreach(void)
{
    int       i, val;
    CraLList *list = cra_alloc(CraLList);
    cra_llist_init(int, list);

    // foreach(empty)
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_LLIST_ITERABLE_I, list, vals) assert_always(false);

    for (i = 0; i < 10; i++)
        cra_llist_append(list, &i);

    i = 0;
    printf("foreach        : ");
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");

    i = 9;
    printf("foreach reverse: ");
    CRA_FOREACH_REVERSE(CRA_LLIST_ITERABLE_I, list, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i == val);
        printf("%d  ", val);
        i--;
    }
    printf("\n");

    cra_llist_clear(list);

    // foreach(empty)
    CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_LLIST_ITERABLE_I, list, vals) assert_always(false);

    cra_llist_uninit(list);
    cra_dealloc(list);
}

void
test_test(void)
{
    CraLList *list = cra_alloc(CraLList);
    cra_llist_init(int, list);

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 10; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_llist_append(list, &j);

        n = list->count == 0 ? 0 : (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            cra_llist_pop_front(list, &v);
            assert_always(v == j);
        }

        for (; cra_llist_pop_front(list, &v); j++)
            assert_always(v == j);
    }
    assert_always(list->count == 0);

    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_llist_prepend(list, &j);

        n = list->count == 0 ? 0 : (rand() + 1) % list->count;
        for (j = 0; j < n; j++)
        {
            cra_llist_pop_back(list, &v);
            assert_always(v == j);
        }

        for (; cra_llist_pop_back(list, &v); j++)
            assert_always(v == j);
    }
    assert_always(list->count == 0);

    int idx, last_idx;
    int check[10000];
    bzero(check, sizeof(check));
    for (i = 0; i < 100; i++)
    {
        last_idx = 0;
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
        {
            idx = list->count == 0 ? 0 : (rand() % list->count);
            cra_llist_insert(list, idx, &j);
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
        CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals)
        {
            memcpy(&v, vals.val1_ref, sizeof(v));
            assert_always(v == check[j]);
            j++;
        }

        while (true)
        {
            idx = list->count == 0 ? 0 : (rand() % list->count);
            if (!cra_llist_pop_at(list, idx, &j))
                break;

            assert_always(check[idx] == j);
            memmove(check + idx, check + idx + 1, (last_idx - idx + 1) * sizeof(int));
            last_idx--;
        }
    }
    assert_always(list->count == 0);

    cra_llist_uninit(list);
    cra_dealloc(list);
}

int
main(void)
{
    test_list_node();
    test_new_delete();
    test_add();
    test_remove();
    test_set();
    test_get();
    test_reverse();
    test_sort();
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
