/**
 * @file test-dict.c
 * @author Cracal
 * @brief test dictionary
 * @version 0.1
 * @date 2024-10-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "collections/cra_dict.h"
#include "cra_assert.h"
#include "cra_malloc.h"
#include <time.h>

void
test_new_delete(void)
{
    CraDict *dict1, dict2;

    dict1 = cra_alloc(CraDict);
    assert_always(dict1 != NULL);
    cra_dict_init0(int, float, dict1, true, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);
    cra_dict_init0(float, int64_t, &dict2, false, (cra_hash_fn)cra_hash_float_p, (cra_compare_fn)cra_compare_int_p);

    cra_dict_uninit(dict1);
    cra_dealloc(dict1);
    cra_dict_uninit(&dict2);
}

void
test_add(void)
{
    CraDictIter it;
    int        *keyptr;
    float      *valptr;
    CraDict    *dict = cra_alloc(CraDict);
    cra_dict_init_size0(int, float, dict, 1000, true, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);

    for (int i = 0; i < 1000; i++)
        assert_always(cra_dict_add(dict, &i, &(float){ i + .5f }));

    int i = 0;
    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, (void **)&keyptr, (void **)&valptr); i++)
        assert_always(i == *keyptr && (i + .5f) == *valptr);

    assert_always(!cra_dict_add(dict, &(int){ 3 }, &(float){ 3000.5f }));
    assert_always(cra_dict_put0(dict, &(int){ 3 }, &(float){ 30000.5f }));
    assert_always(cra_dict_get_ptr(dict, &(int){ 3 }, (void **)&valptr) && cra_compare_float(*valptr, 30000.5f) == 0);
    assert_always(cra_dict_put0(dict, &(int){ 3 }, &(float){ 3.5f }));

    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, (void **)&keyptr, (void **)&valptr);)
    {
        // printf("{k: %d, v: %f}\n", *keyptr, *valptr);
        assert_always(cra_compare_float(*valptr, *keyptr + .5f) == 0);
    }

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_remove(void)
{
    float    val;
    CraDict *dict = cra_alloc(CraDict);
    cra_dict_init0(int, float, dict, true, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);

    assert_always(!cra_dict_remove(dict, &(int){ 0 }));

    for (int i = 0; i < 1000; i++)
        cra_dict_add(dict, &i, &(float){ i + .5f });

    assert_always(cra_dict_pop(dict, &(int){ 30 }, NULL, &val) && val == 30.5f);

    for (int i = 0; i < 80; i++)
    {
        if (i == 30)
            continue;
        assert_always(cra_dict_remove(dict, &i));
    }
    assert_always(dict->count == 920);
    for (int i = 80; i < 1000; i++)
    {
        assert_always(cra_dict_get(dict, &i, &val));
        assert_always(cra_compare_float(val, i + .5f) == 0);
    }

    printf("\n");

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_get(void)
{
    CraDictIter it;
    CraDict    *dict = cra_alloc(CraDict);
    cra_dict_init0(int, float, dict, true, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);

    for (int i = -5000; i <= 5000; i++)
        cra_dict_add(dict, &i, &(float){ i + .5f });

    int   *keyptr;
    float *valptr;
    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, (void **)&keyptr, (void **)&valptr);)
    {
        // printf("{k: %d, v: %f}\n", *keyptr, *valptr);
        assert_always(cra_compare_float(*valptr, *keyptr + .5f) == 0);
    }

    float val;
    for (int i = -5000; i <= 5000; i++)
    {
        assert_always(cra_dict_get(dict, &i, &val));
        assert_always(cra_compare_float(val, i + .5f) == 0);
        assert_always(cra_dict_get_ptr(dict, &i, (void **)&valptr));
        assert_always(cra_compare_float_p(valptr, &(float){ i + .5f }) == 0);
    }

    cra_dict_uninit(dict);
    cra_dealloc(dict);
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
    CraDict *dict;
    CraDict *dict2;

    dict = cra_alloc(CraDict);
    cra_dict_init0(int, double, dict, true, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);

    for (int i = 0; i < 1000; i++)
        cra_dict_add(dict, &i, &(double){ i + .5 });

    dict2 = cra_dict_clone(dict, NULL, NULL);
    assert_always(dict2 != NULL);
    assert_always(dict2->count == dict->count);

    double *valptr1, *valptr2;
    for (int i = 0; i < 1000; i++)
    {
        cra_dict_get_ptr(dict, &i, (void **)&valptr1);
        cra_dict_get_ptr(dict2, &i, (void **)&valptr2);
        assert_always(*valptr1 == *valptr2);
    }

    cra_dict_uninit(dict2);
    cra_dealloc(dict2);
    cra_dict_uninit(dict);
    cra_dealloc(dict);

    dict = cra_alloc(CraDict);
    cra_dict_init0(int, A_s *, dict, true, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);

    A_s *as;
    for (int i = 0; i < 1000; i++)
    {
        as = (A_s *)cra_malloc(sizeof(A_s));
        as->i = i + 1;
        as->f = i + 1.5f;
        cra_dict_add(dict, &i, &as);
    }

    dict2 = cra_dict_clone(dict, NULL, copy_as);
    assert_always(dict2 != NULL);
    assert_always(dict2->count == dict->count);

    A_s *valas1, *valas2, **valasptr1, **valasptr2;
    for (int i = 0; i < 1000; i++)
    {
        cra_dict_get_ptr(dict, &i, (void **)&valasptr1);
        cra_dict_get_ptr(dict2, &i, (void **)&valasptr2);
        assert_always((*valasptr1)->i == (*valasptr2)->i);
        assert_always((*valasptr1)->f == (*valasptr2)->f);
        assert_always(*valasptr1 != *valasptr2);
        cra_dict_get(dict, &i, &valas1);
        cra_dict_get(dict2, &i, &valas2);
        assert_always(valas1->i == valas2->i);
        assert_always(valas1->f == valas2->f);
        assert_always(valas1 != valas2);
    }
    CraDictIter it;
    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, NULL, (void **)&valasptr1);)
        cra_free(*valasptr1);
    for (cra_dict_iter_init(dict2, &it); cra_dict_iter_next(&it, NULL, (void **)&valasptr1);)
        cra_free(*valasptr1);

    cra_dict_uninit(dict2);
    cra_dealloc(dict2);
    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_foreach(void)
{
    CraDict *dict = cra_alloc(CraDict);
    cra_dict_init0(int, int, dict, false, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);

    int        *keyptr;
    int        *valptr;
    CraDictIter it;

    printf("foreach(    empty): ");
    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, (void **)&keyptr, (void **)&valptr);)
    {
        printf("{%d: %d} ", *keyptr, *valptr);
        assert_always(false);
    }
    printf("\n");

    for (int i = 0; i < 10; i++)
        cra_dict_add(dict, &i, &(int){ i + 100 });

    printf("foreach(not empty): ");
    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, (void **)&keyptr, (void **)&valptr);)
    {
        printf("{%d: %d} ", *keyptr, *valptr);
    }
    printf("\n");

    cra_dict_clear(dict);

    printf("foreach(    empty): ");
    for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, (void **)&keyptr, (void **)&valptr);)
    {
        printf("{%d: %d} ", *keyptr, *valptr);
        assert_always(false);
    }
    printf("\n");

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_test(void)
{
    CraDictIter it;
    CraDict    *dict = cra_alloc(CraDict);
    cra_dict_init0(int, int, dict, true, (cra_hash_fn)cra_hash_int_p, (cra_compare_fn)cra_compare_int_p);

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 100000;
        for (j = 0; j < n; j++)
            cra_dict_put0(dict, &j, &(int){ j + 100 });

        n = (rand() + 1) % dict->count;
        for (j = 0; j < n; j++)
        {
            cra_dict_pop(dict, &j, NULL, &v);
            assert_always(v == j + 100);
        }

        for (; cra_dict_pop(dict, &j, NULL, &v); j++)
            assert_always(v == j + 100);
    }
    assert_always(dict->count == 0);

    int  idx, *pk, *pv;
    int *check = (int *)cra_malloc(sizeof(int) * 1000000);
    bzero(check, sizeof(sizeof(int) * 1000000));
    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 1000000;
        for (j = 0; j < n; j++)
        {
            idx = rand() % 1000000;
            cra_dict_put0(dict, &idx, &j);
            check[idx] = j;
        }
        for (cra_dict_iter_init(dict, &it); cra_dict_iter_next(&it, (void **)&pk, (void **)&pv);)
        {
            assert_always(*pv == check[*pk]);
        }

        while (cra_dict_get_count(dict) > 0)
        {
            idx = rand() % 1000000;
            if (cra_dict_pop(dict, &idx, NULL, &j))
                assert_always(check[idx] == j);
        }
    }
    assert_always(dict->count == 0);
    cra_free(check);

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

int
main(void)
{
    test_new_delete();
    test_add();
    test_remove();
    test_get();
    test_clone();
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
