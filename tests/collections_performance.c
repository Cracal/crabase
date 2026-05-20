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
#include "collections/cra_alist.h"
#include "collections/cra_deque.h"
#include "collections/cra_dict.h"
#include "collections/cra_llist.h"
#include "cra_malloc.h"
#include "cra_time.h"

static inline int
rand_large(void)
{
    uint32_t r = 0;
    r ^= (uint32_t)rand() << 20;
    r ^= (uint32_t)rand() << 5;
    r ^= (uint32_t)rand();
    return r;
}

void
test_alist_performance(int sizes[])
{
    int           val;
    CraAList      list;
    int           nloop;
    long long     sum;
    unsigned long start_ms, end_ms;

    assert_always(cra_alist_init_with_size(int, &list, sizes[0]));

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
            cra_alist_insert(&list, rand_large() % (list.count + 1), &j);
        end_ms = cra_tick_ms();
        printf("\tinsert:        %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_get(&list, rand_large() % list.count, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // get miss
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_get(&list, rand_large() + (list.count + 1), &val);
        end_ms = cra_tick_ms();
        printf("\tget(miss):     %lums.\n", end_ms - start_ms);

        // iter
        sum = 0;
        nloop = 0;
        start_ms = cra_tick_ms();
        CRA_FOREACH(CRA_ALIST_ITERABLE_I, &list, vals)
        {
            nloop++;
            sum += *(int *)vals.val1_ref;
        }
        end_ms = cra_tick_ms();
        printf("\titer:          %lums. loop times: %d. sum: %lld\n", end_ms - start_ms, nloop, sum);

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
            cra_alist_insert(&list, rand_large() % (list.count + 1), &j);

        // remove front
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_remove_front(&list);
        end_ms = cra_tick_ms();
        printf("\tremove front:  %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_alist_insert(&list, rand_large() % (list.count + 1), &j);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_alist_remove_at(&list, rand_large() % list.count);
        end_ms = cra_tick_ms();
        printf("\tremove random: %lums.\n", end_ms - start_ms);
    }

    cra_alist_uninit(&list);
}

void
test_llist_performance(int sizes[])
{
    int           val;
    CraLList      list;
    int           nloop;
    long long     sum;
    unsigned long start_ms, end_ms;

    assert_always(cra_llist_init(int, &list));

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
            cra_llist_insert(&list, rand_large() % (list.count + 1), &j);
        end_ms = cra_tick_ms();
        printf("\tinsert:        %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_get(&list, rand_large() % list.count, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // get miss
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_get(&list, rand_large() + (list.count + 1), &val);
        end_ms = cra_tick_ms();
        printf("\tget(miss):     %lums.\n", end_ms - start_ms);

        // iter
        sum = 0;
        nloop = 0;
        start_ms = cra_tick_ms();
        CRA_FOREACH(CRA_LLIST_ITERABLE_I, &list, vals)
        {
            nloop++;
            sum += *(int *)vals.val1_ref;
        }
        end_ms = cra_tick_ms();
        printf("\titer:          %lums. loop times: %d. sum: %lld\n", end_ms - start_ms, nloop, sum);

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
            cra_llist_insert(&list, rand_large() % (list.count + 1), &j);

        // remove front
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_remove_front(&list);
        end_ms = cra_tick_ms();
        printf("\tremove front:  %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_llist_insert(&list, rand_large() % (list.count + 1), &j);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_llist_remove_at(&list, rand_large() % list.count);
        end_ms = cra_tick_ms();
        printf("\tremove random: %lums.\n", end_ms - start_ms);
    }

    cra_llist_uninit(&list);
}

void
test_deque_performance(int sizes[])
{
    int           val;
    CraDeque      deque;
    int           nloop;
    long long     sum;
    unsigned long start_ms, end_ms;

    cra_deque_init(int, &deque);

    printf("\n=========================================================\n\n");

    for (int i = 0; sizes[i] != 0; i++)
    {
        printf("test deque[%d]:\n", sizes[i]);

        // clear
        cra_deque_clear(&deque);

        // append
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_append(&deque, &j);
        end_ms = cra_tick_ms();
        printf("\tappend:        %lums.\n", end_ms - start_ms);

        cra_deque_clear(&deque);

        // prepend
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_prepend(&deque, &j);
        end_ms = cra_tick_ms();
        printf("\tprepend:       %lums.\n", end_ms - start_ms);

        cra_deque_clear(&deque);

        // insert
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_insert(&deque, rand_large() % (deque.count + 1), &j);
        end_ms = cra_tick_ms();
        printf("\tinsert:        %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_get(&deque, rand_large() % deque.count, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // get miss
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_get(&deque, rand_large() + (deque.count + 1), &val);
        end_ms = cra_tick_ms();
        printf("\tget(miss):     %lums.\n", end_ms - start_ms);

        // iter
        sum = 0;
        nloop = 0;
        start_ms = cra_tick_ms();
        CRA_FOREACH(CRA_DEQUE_ITERABLE_I, &deque, vals)
        {
            nloop++;
            sum += *(int *)vals.val1_ref;
        }
        end_ms = cra_tick_ms();
        printf("\titer:          %lums. loop times: %d. sum: %lld\n", end_ms - start_ms, nloop, sum);

        // // sort
        // start_ms = cra_tick_ms();
        // cra_deque_sort(&deque, (cra_compare_fn)cra_compare_int_p);
        // end_ms = cra_tick_ms();
        // printf("\tsort:          %lums.\n", end_ms - start_ms);

        // remove back
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_remove_back(&deque);
        end_ms = cra_tick_ms();
        printf("\tremove back:   %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_deque_insert(&deque, rand_large() % (deque.count + 1), &j);

        // remove front
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_remove_front(&deque);
        end_ms = cra_tick_ms();
        printf("\tremove front:  %lums.\n", end_ms - start_ms);

        for (int j = 0; j < sizes[i]; j++)
            cra_deque_insert(&deque, rand_large() % (deque.count + 1), &j);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_deque_remove_at(&deque, rand_large() % deque.count);
        end_ms = cra_tick_ms();
        printf("\tremove random: %lums.\n", end_ms - start_ms);
    }

    cra_deque_uninit(&deque);
}

void
test_dict_performance(int sizes[])
{
    size_t        val;
    CraDict       dict;
    int           nloop;
    long long     sum;
    unsigned long start_ms, end_ms;

    assert_always(cra_dict_init(int, size_t, &dict, cra_hash_int_p, cra_cmp_int_p));

    printf("\n=========================================================\n\n");

    for (int i = 0; sizes[i] != 0; i++)
    {
        printf("test dict[%d]:\n", sizes[i]);

        // clear
        cra_dict_clear(&dict);

        // insert
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_dict_put(&dict, &(int){ rand_large() }, &(size_t){ j });
        end_ms = cra_tick_ms();
        printf("\tput:           %lums.\n", end_ms - start_ms);

        // get
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_dict_get(&dict, &(int){ rand_large() }, &val);
        end_ms = cra_tick_ms();
        printf("\tget:           %lums.\n", end_ms - start_ms);

        // iter
        sum = 0;
        nloop = 0;
        start_ms = cra_tick_ms();
        CRA_FOREACH(CRA_DICT_ITERABLE_I, &dict, vals)
        {
            nloop++;
            sum += *(size_t *)vals.val2_ref;
        }
        end_ms = cra_tick_ms();
        printf("\titer:          %lums. loop times: %d. sum: %lld\n", end_ms - start_ms, nloop, sum);

        // remove random
        start_ms = cra_tick_ms();
        for (int j = 0; j < sizes[i]; j++)
            cra_dict_remove(&dict, &(int){ rand_large() });
        end_ms = cra_tick_ms();
        printf("\tremove:        %lums.\n", end_ms - start_ms);
    }

    cra_dict_uninit(&dict);
}

int
main(void)
{
    //              千     万     十万  百万  千万
    int sizes[] = { 1000, 10000, 100000, 0, 10000000, 0 };

    srand((unsigned int)time(NULL));

    test_alist_performance(sizes);
    test_llist_performance(sizes);
    test_deque_performance(sizes);
    sizes[3] = 1000000;
    test_dict_performance(sizes);

    cra_memory_leak_report();
    return 0;
}
