/**
 * @file collections_performance.c
 * @author Cracal
 * @brief performance
 * @version 0.1
 * @date 2025-07-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "cra_time.h"
#include "cra_malloc.h"
#include "collections/cra_alist.h"
#include "collections/cra_llist.h"
#include "collections/cra_deque.h"
#include "collections/cra_dict.h"

void test_alist_performance(int sizes[])
{
    int val, *valptr;
    unsigned long start_ms, end_ms;
    CraAList list;
    cra_alist_init0(int, &list, sizes[0], false, NULL);
    srand((unsigned int)time(NULL));

    printf("\n=========================================================\n\n");

    for (int i = 0; sizes[i] != 0; i++)
    {
        printf("test alist[%d]:\n", sizes[i]);

        // clear
        cra_alist_clear(&list);

        // append
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_append(&list, &j);
        end_ms = cra_tick_ms();
        printf("\tappend:        %lums.\n", end_ms - start_ms);

        cra_alist_clear(&list);

        // prepend
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_prepend(&list, &j);
        end_ms = cra_tick_ms();
        printf("\tprepend:       %lums.\n", end_ms - start_ms);

        cra_alist_clear(&list);

        // insert
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_insert(&list, rand() % (list.count + 1), &j);
        end_ms = cra_tick_ms();
        printf("\tinsert:        %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_get(&list, rand() % list.count, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // get miss
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_get(&list, rand() + (list.count + 1), &val);
        end_ms = cra_tick_ms();
        printf("\tget(miss):     %lums.\n", end_ms - start_ms);

        // iter
        start_ms = cra_tick_ms();
        for (CraAListIter it = cra_alist_iter_init(&list); cra_alist_iter_next(&it, (void **)&valptr);)
            ;
        end_ms = cra_tick_ms();
        printf("\titer:          %lums.\n", end_ms - start_ms);

        // sort
        start_ms = cra_tick_ms();
        cra_alist_sort(&list, (cra_compare_fn)cra_compare_int_p);
        end_ms = cra_tick_ms();
        printf("\tsort:          %lums.\n", end_ms - start_ms);

        // remove back
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_remove_back(&list);
        end_ms = cra_tick_ms();
        printf("\tremove back:   %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_alist_insert(&list, rand() % (list.count + 1), &j);

        // remove front
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_remove_front(&list);
        end_ms = cra_tick_ms();
        printf("\tremove front:  %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_alist_insert(&list, rand() % (list.count + 1), &j);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_remove_at(&list, rand() % list.count);
        end_ms = cra_tick_ms();
        printf("\tremove random: %lums.\n", end_ms - start_ms);
    }

    cra_alist_uninit(&list);
}

void test_llist_performance(int sizes[])
{
    int val, *valptr;
    unsigned long start_ms, end_ms;
    CraLList list;
    cra_llist_init0(int, &list, false, NULL);
    srand((unsigned int)time(NULL));

    printf("\n=========================================================\n\n");

    for (int i = 0; sizes[i] != 0; i++)
    {
        printf("test llist[%d]:\n", sizes[i]);

        // clear
        cra_llist_clear(&list);

        // append
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_append(&list, &j);
        end_ms = cra_tick_ms();
        printf("\tappend:        %lums.\n", end_ms - start_ms);

        cra_llist_clear(&list);

        // prepend
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_prepend(&list, &j);
        end_ms = cra_tick_ms();
        printf("\tprepend:       %lums.\n", end_ms - start_ms);

        cra_llist_clear(&list);

        // insert
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_insert(&list, rand() % (list.count + 1), &j);
        end_ms = cra_tick_ms();
        printf("\tinsert:        %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_get(&list, rand() % list.count, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // get miss
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_get(&list, rand() + (list.count + 1), &val);
        end_ms = cra_tick_ms();
        printf("\tget(miss):     %lums.\n", end_ms - start_ms);

        // iter
        start_ms = cra_tick_ms();
        for (CraLListIter it = cra_llist_iter_init(&list); cra_llist_iter_next(&it, (void **)&valptr);)
            ;
        end_ms = cra_tick_ms();
        printf("\titer:          %lums.\n", end_ms - start_ms);

        // sort
        start_ms = cra_tick_ms();
        cra_llist_sort(&list, (cra_compare_fn)cra_compare_int_p);
        end_ms = cra_tick_ms();
        printf("\tsort:          %lums.\n", end_ms - start_ms);

        // remove back
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_remove_back(&list);
        end_ms = cra_tick_ms();
        printf("\tremove back:   %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_llist_insert(&list, rand() % (list.count + 1), &j);

        // remove front
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_remove_front(&list);
        end_ms = cra_tick_ms();
        printf("\tremove front:  %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_llist_insert(&list, rand() % (list.count + 1), &j);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_remove_at(&list, rand() % list.count);
        end_ms = cra_tick_ms();
        printf("\tremove random: %lums.\n", end_ms - start_ms);
    }

    cra_llist_uninit(&list);
}

void test_deque_performance(int sizes[])
{
    int val, *valptr;
    unsigned long start_ms, end_ms;
    CraDeque deque;
    cra_deque_init0(int, &deque, CRA_DEQUE_INFINITE, false, NULL);
    srand((unsigned int)time(NULL));

    printf("\n=========================================================\n\n");

    for (int i = 0; sizes[i] != 0; i++)
    {
        printf("test deque[%d]:\n", sizes[i]);

        // clear
        cra_deque_clear(&deque);

        // append
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_push(&deque, &j);
        end_ms = cra_tick_ms();
        printf("\tappend:        %lums.\n", end_ms - start_ms);

        cra_deque_clear(&deque);

        // prepend
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_push_left(&deque, &j);
        end_ms = cra_tick_ms();
        printf("\tprepend:       %lums.\n", end_ms - start_ms);

        cra_deque_clear(&deque);

        // insert
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_insert(&deque, rand() % (deque.count + 1), &j);
        end_ms = cra_tick_ms();
        printf("\tinsert:        %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_get(&deque, rand() % deque.count, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // get miss
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_get(&deque, rand() + (deque.count + 1), &val);
        end_ms = cra_tick_ms();
        printf("\tget(miss):     %lums.\n", end_ms - start_ms);

        // iter
        start_ms = cra_tick_ms();
        for (CraDequeIter it = cra_deque_iter_init(&deque); cra_deque_iter_next(&it, (void **)&valptr);)
            ;
        end_ms = cra_tick_ms();
        printf("\titer:          %lums.\n", end_ms - start_ms);

        // // sort
        // start_ms = cra_tick_ms();
        // cra_deque_sort(&deque, (cra_compare_fn)cra_compare_int_p);
        // end_ms = cra_tick_ms();
        // printf("\tsort:          %lums.\n", end_ms - start_ms);

        // remove back
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_pop(&deque, NULL);
        end_ms = cra_tick_ms();
        printf("\tremove back:   %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_deque_insert(&deque, rand() % (deque.count + 1), &j);

        // remove front
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_pop_left(&deque, NULL);
        end_ms = cra_tick_ms();
        printf("\tremove front:  %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_deque_insert(&deque, rand() % (deque.count + 1), &j);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_remove_at(&deque, rand() % deque.count);
        end_ms = cra_tick_ms();
        printf("\tremove random: %lums.\n", end_ms - start_ms);
    }

    cra_deque_uninit(&deque);
}

void test_dict_performance(int sizes[])
{
    int *keyptr;
    size_t val, *valptr;
    unsigned long start_ms, end_ms;
    CraDict dict;
    cra_dict_init0(int, size_t, &dict, false, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p, NULL, NULL);
    srand((unsigned int)time(NULL));

    printf("\n=========================================================\n\n");

    for (int i = 0; sizes[i] != 0; i++)
    {
        printf("test dict[%d]:\n", sizes[i]);

        // clear
        cra_dict_clear(&dict);

        // insert
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_dict_put(&dict, &(int){rand()}, &(size_t){j});
        end_ms = cra_tick_ms();
        printf("\tput:           %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_dict_get(&dict, &(int){rand()}, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // iter
        start_ms = cra_tick_ms();
        for (CraDictIter it = cra_dict_iter_init(&dict); cra_dict_iter_next(&it, (void **)&keyptr, (void **)&valptr);)
            ;
        end_ms = cra_tick_ms();
        printf("\titer:          %lums.\n", end_ms - start_ms);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_dict_remove(&dict, &(int){rand()});
        end_ms = cra_tick_ms();
        printf("\tremove:        %lums.\n", end_ms - start_ms);
    }

    cra_dict_uninit(&dict);
}

int main(void)
{
    //              千     万     十万  百万  千万
    int sizes[] = {1000, 10000, 100000, 0, 10000000, 0};
    test_alist_performance(sizes);
    test_llist_performance(sizes);
    test_deque_performance(sizes);
    sizes[3] = 1000000;
    test_dict_performance(sizes);

    cra_memory_leak_report();
    return 0;
}
