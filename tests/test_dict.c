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
    assert_always(cra_dict_init(int, float, dict1, cra_hash_int_p, cra_cmp_int_p));
    assert_always(dict1->buckets);
    assert_always(dict1->entries);
    assert_always(dict1->next == 0);
    assert_always(dict1->count == 0);
    assert_always(dict1->capacity == CRA_DICT_DEAFULT_CAPACITY);
    assert_always(dict1->freelist == -1);
    assert_always(dict1->key_size == sizeof(int));
    assert_always(dict1->val_size == sizeof(float));
    assert_always(dict1->key_offset % 2 == 0);
    assert_always(dict1->val_offset % 2 == 0);
    assert_always(dict1->entry_size > dict1->val_offset && dict1->entry_size % 2 == 0);

    assert_always(cra_dict_init_with_size(float, int64_t, &dict2, 100, cra_hash_float_p, cra_cmp_int_p));
    assert_always(dict2.buckets);
    assert_always(dict2.entries);
    assert_always(dict2.next == 0);
    assert_always(dict2.count == 0);
    assert_always(dict2.capacity >= 100);
    assert_always(dict2.freelist == -1);
    assert_always(dict2.key_size == sizeof(float));
    assert_always(dict2.val_size == sizeof(int64_t));
    assert_always(dict2.key_offset % 2 == 0);
    assert_always(dict2.val_offset % 2 == 0);
    assert_always(dict2.entry_size > dict2.val_offset && dict2.entry_size % 2 == 0);

    cra_dict_uninit(dict1);
    assert_always(memcmp(dict1, &(CraDict){ 0 }, sizeof(CraDict)) == 0);
    cra_dict_uninit(&dict2);
    assert_always(memcmp(&dict2, &(CraDict){ 0 }, sizeof(CraDict)) == 0);
    cra_dealloc(dict1);

    CRA_DICT_INITIALIZABLE_PARAM_DEF(param, double, char[3], 0, cra_hash_double_p, cra_cmp_double_p);
    assert_always(cra_initializable_init(CRA_DICT_INITIALIZABLE_I, &dict2, &param));
    assert_always(dict2.buckets);
    assert_always(dict2.entries);
    assert_always(dict2.next == 0);
    assert_always(dict2.count == 0);
    assert_always(dict2.capacity == CRA_DICT_DEAFULT_CAPACITY);
    assert_always(dict2.freelist == -1);
    assert_always(dict2.key_size == param.key_size);
    assert_always(dict2.val_size == param.val_size);
    assert_always(dict2.key_offset % 2 == 0);
    assert_always(dict2.val_offset % 2 == 0);
    assert_always(dict2.entry_size > dict2.val_offset && dict2.entry_size % 2 == 0);

    cra_initializable_uninit(CRA_DICT_INITIALIZABLE_I, &dict2);
    assert_always(memcmp(&dict2, &(CraDict){ 0 }, sizeof(CraDict)) == 0);
}

void
test_add(void)
{
    int   key;
    float val, *pval;

    CraDict *dict = cra_alloc(CraDict);
    cra_dict_init_with_size(int, float, dict, 1000, cra_hash_int_p, cra_cmp_int_p);

    for (int i = 0; i < 1000; i++)
        assert_always(cra_dict_add(dict, &i, &(float){ i + .5f }));
    assert_always(dict->count == 1000);

    int i = 0;
    CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals)
    {
        memcpy(&key, vals.val1_ref, sizeof(key));
        memcpy(&val, vals.val2_ref, sizeof(val));
        assert_always(i == key && cra_cmp_float(val, i + .5f) == 0);
        i++;
    }
    assert_always(i == 1000);

    assert_always(!cra_dict_add(dict, &(int){ 3 }, &(float){ 3000.5f }));
    assert_always(cra_dict_put(dict, &(int){ 3 }, &(float){ 30000.5f }));
    assert_always(!!(pval = cra_dict_get_ref(dict, &(int){ 3 })) && cra_cmp_float(*pval, 30000.5f) == 0);
    assert_always(cra_dict_put_and_return_v(dict, &(int){ 3 }, &(float){ 3.5f }, &val) &&
                  cra_cmp_float(val, 30000.5f) == 0);

    CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals)
    {
        memcpy(&key, vals.val1_ref, sizeof(key));
        memcpy(&val, vals.val2_ref, sizeof(val));
        // printf("{k: %d, v: %f}\n", key, val);
        assert_always(cra_cmp_float(val, key + .5f) == 0);
    }

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_remove(void)
{
    float    val;
    CraDict *dict = cra_alloc(CraDict);
    cra_dict_init(int, float, dict, cra_hash_int_p, cra_cmp_int_p);

    assert_always(!cra_dict_remove(dict, &(int){ 0 }));

    for (int i = 0; i < 1000; i++)
        cra_dict_add(dict, &i, &(float){ i + .5f });

    assert_always(cra_dict_pop(dict, &(int){ 30 }, &val) && val == 30.5f);
    assert_always(!cra_dict_remove(dict, &(int){ 30 }));
    assert_always(dict->count == 999);

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
        assert_always(cra_cmp_float(val, i + .5f) == 0);
    }

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_get(void)
{
    int      key;
    float    val, *pval;
    CraDict *dict = cra_alloc(CraDict);
    cra_dict_init(int, float, dict, cra_hash_int_p, cra_cmp_int_p);

    for (int i = -5000; i <= 5000; i++)
        cra_dict_add(dict, &i, &(float){ i + .5f });

    CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals)
    {
        memcpy(&key, vals.val1_ref, sizeof(key));
        memcpy(&val, vals.val2_ref, sizeof(val));
        // printf("{k: %d, v: %f}\n", key, val);
        assert_always(cra_cmp_float(val, key + .5f) == 0);
    }

    for (int i = -5000; i <= 5000; i++)
    {
        assert_always(cra_dict_get(dict, &i, &val));
        assert_always(cra_cmp_float(val, i + .5f) == 0);
        assert_always(!!(pval = cra_dict_get_ref(dict, &i)));
        assert_always(cra_cmp_float(*pval, i + .5f) == 0);
    }

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_foreach(void)
{
    int      key;
    double   val;
    CraDict *dict = cra_alloc(CraDict);
    cra_dict_init(int, double, dict, cra_hash_int_p, cra_cmp_int_p);

    // foreach(empty)
    CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_DICT_ITERABLE_I, dict, vals) assert_always(false);

    for (int i = 0; i < 10; i++)
        cra_dict_add(dict, &i, &(double){ i });

    printf("foreach        : ");
    CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals)
    {
        memcpy(&key, vals.val1_ref, sizeof(key));
        memcpy(&val, vals.val2_ref, sizeof(val));
        assert_always(cra_cmp_double(val, (double)key) == 0);
        printf("{%d: %.2lf}  ", key, val);
    }
    printf("\n");

    printf("foreach reverse: ");
    CRA_FOREACH_REVERSE(CRA_DICT_ITERABLE_I, dict, vals)
    {
        memcpy(&key, vals.val1_ref, sizeof(key));
        memcpy(&val, vals.val2_ref, sizeof(val));
        assert_always(cra_cmp_double(val, (double)key) == 0);
        printf("{%d: %.2lf}  ", key, val);
    }
    printf("\n");

    cra_dict_clear(dict);

    // foreach(empty)
    CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_DICT_ITERABLE_I, dict, vals) assert_always(false);

    cra_dict_uninit(dict);
    cra_dealloc(dict);
}

void
test_test(void)
{
    CraDict *dict = cra_alloc(CraDict);
    cra_dict_init(int, int, dict, cra_hash_int_p, cra_cmp_int_p);

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 100000;
        for (j = 0; j < n; j++)
            cra_dict_put(dict, &j, &(int){ j + 100 });

        n = (rand() + 1) % dict->count;
        for (j = 0; j < n; j++)
        {
            cra_dict_pop(dict, &j, &v);
            assert_always(v == j + 100);
        }

        for (; cra_dict_pop(dict, &j, &v); j++)
            assert_always(v == j + 100);
    }
    assert_always(dict->count == 0);

    int  idx, key, val;
    int *check = (int *)cra_malloc(sizeof(int) * 1000000);
    bzero(check, sizeof(sizeof(int) * 1000000));
    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 1000000;
        for (j = 0; j < n; j++)
        {
            idx = rand() % 1000000;
            cra_dict_put(dict, &idx, &j);
            check[idx] = j;
        }
        CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals)
        {
            memcpy(&key, vals.val1_ref, sizeof(key));
            memcpy(&val, vals.val2_ref, sizeof(val));
            assert_always(val == check[key]);
        }

        while (dict->count > 0)
        {
            idx = rand() % 1000000;
            if (cra_dict_pop(dict, &idx, &val))
                assert_always(check[idx] == val);
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
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
