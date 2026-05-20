#include "collections/cra_collects.h"
#include "cra_assert.h"
#include "threads/cra_thrdpool.h"
#include <time.h>

void
test_compare(void)
{
#define ITEM(suffix, type)                                                \
    assert_always(cra_cmp_##suffix(1, 1) == 0);                           \
    assert_always(cra_cmp_##suffix(0, 1) < 0);                            \
    assert_always(cra_cmp_##suffix(1, 0) > 0);                            \
    assert_always(cra_cmp_##suffix##_p(&(type){ 1 }, &(type){ 1 }) == 0); \
    assert_always(cra_cmp_##suffix##_p(&(type){ 0 }, &(type){ 1 }) < 0);  \
    assert_always(cra_cmp_##suffix##_p(&(type){ 1 }, &(type){ 0 }) > 0)

    ITEM(int, int);
    ITEM(uint, unsigned int);
    ITEM(ssize, ssize_t);
    ITEM(size, size_t);

    ITEM(int8, int8_t);
    ITEM(int16, int16_t);
    ITEM(int32, int32_t);
    ITEM(int64, int64_t);

    ITEM(uint8, uint8_t);
    ITEM(uint16, uint16_t);
    ITEM(uint32, uint32_t);
    ITEM(uint64, uint64_t);

    assert_always(cra_cmp_float(1.33f, 1.33f) == 0);
    assert_always(cra_cmp_float(1.28f, 1.33f) < 0);
    assert_always(cra_cmp_float(1.33f, 1.28f) > 0);
    assert_always(cra_cmp_float_p(&(float){ 1.33f }, &(float){ 1.33f }) == 0);
    assert_always(cra_cmp_float_p(&(float){ 1.32f }, &(float){ 1.33f }) < 0);
    assert_always(cra_cmp_float_p(&(float){ 1.33f }, &(float){ 1.28f }) > 0);

    assert_always(cra_cmp_double(1.33, 1.33) == 0);
    assert_always(cra_cmp_double(1.28, 1.33) < 0);
    assert_always(cra_cmp_double(1.33, 1.28) > 0);
    assert_always(cra_cmp_double_p(&(double){ 1.33 }, &(double){ 1.33 }) == 0);
    assert_always(cra_cmp_double_p(&(double){ 1.32 }, &(double){ 1.33 }) < 0);
    assert_always(cra_cmp_double_p(&(double){ 1.33 }, &(double){ 1.28 }) > 0);

    assert_always(cra_cmp_string("BC", "BC") == 0);
    assert_always(cra_cmp_string("ABC", "BC") < 0);
    assert_always(cra_cmp_string("BC", "ABC") > 0);
    assert_always(cra_cmp_string_p((const char **)&(char *){ "BC" }, (const char **)&(char *){ "BC" }) == 0);
    assert_always(cra_cmp_string_p((const char **)&(char *){ "ABC" }, (const char **)&(char *){ "BC" }) < 0);
    assert_always(cra_cmp_string_p((const char **)&(char *){ "BC" }, (const char **)&(char *){ "ABC" }) > 0);

#undef ITEM
}

void
test_hash(void)
{
    cra_hash_t hash1, hash2, hash3;

    srand((unsigned int)time(NULL));

#define ITEM(suffix, type)                                             \
    for (int i = 0; i < 1000; i++)                                     \
    {                                                                  \
        type _tmp, _val = (type)rand();                                \
        hash1 = cra_hash_##suffix(_val);                               \
        hash2 = cra_hash_##suffix(_val);                               \
        _tmp = _val + (type)2.5;                                       \
        if (cra_cmp_##suffix(_tmp, _val) == 0)                         \
            _tmp = _val * (type)2.5;                                   \
        hash3 = cra_hash_##suffix(_tmp);                               \
        assert_always(hash1 == hash2);                                 \
        assert_always(hash1 != hash3 || (hash1 == -2 && hash3 == -2)); \
        hash1 = cra_hash_##suffix##_p(&(type){ _val });                \
        hash2 = cra_hash_##suffix##_p(&(type){ _val });                \
        hash3 = cra_hash_##suffix##_p(&(type){ _tmp });                \
        assert_always(hash1 == hash2);                                 \
        assert_always(hash1 != hash3 || (hash1 == -2 && hash3 == -2)); \
    }

    ITEM(int, int)
    ITEM(uint, unsigned int)
    ITEM(ssize, ssize_t)
    ITEM(size, size_t)

    ITEM(int8, int8_t)
    ITEM(int16, int16_t)
    ITEM(int32, int32_t)
    ITEM(int64, int64_t)

    ITEM(uint8, uint8_t)
    ITEM(uint16, uint16_t)
    ITEM(uint32, uint32_t)
    ITEM(uint64, uint64_t)

    ITEM(float, float)
    ITEM(double, double)

    char *s1 = "hello world", *s2 = "good afternoon";
    hash1 = cra_hash_string1(s1);
    hash2 = cra_hash_string1(s1);
    hash3 = cra_hash_string1(s2);
    assert_always(hash1 == hash2);
    assert_always(hash1 != hash3 || (hash1 == -2 && hash3 == -2));
    hash1 = cra_hash_string1_p((const char **)&(char *){ s1 });
    hash2 = cra_hash_string1_p((const char **)&(char *){ s1 });
    hash3 = cra_hash_string1_p((const char **)&(char *){ s2 });
    assert_always(hash1 == hash2);
    assert_always(hash1 != hash3 || (hash1 == -2 && hash3 == -2));

    hash1 = cra_hash_string2(s1);
    hash2 = cra_hash_string2(s1);
    hash3 = cra_hash_string2(s2);
    assert_always(hash1 == hash2);
    assert_always(hash1 != hash3 || (hash1 == -2 && hash3 == -2));
    hash1 = cra_hash_string2_p((const char **)&(char *){ s1 });
    hash2 = cra_hash_string2_p((const char **)&(char *){ s1 });
    hash3 = cra_hash_string2_p((const char **)&(char *){ s2 });
    assert_always(hash1 == hash2);
    assert_always(hash1 != hash3 || (hash1 == -2 && hash3 == -2));

#undef ITEM
}

static void
test_str_hashcode(const CraThrdPoolArgs0 *args)
{
    cra_hash_t hash1, hash2;
    char      *str = "hello world fdslkfjsodfjsdlkjdoi";
    for (int i = 0; i < 10000; i++)
    {
        hash1 = cra_hash_string1(str);
        hash2 = cra_hash_string1(str);
        assert_always(hash1 == hash2);

        hash1 = cra_hash_string2(str);
        hash2 = cra_hash_string2(str);
        assert_always(hash1 == hash2);
    }
    printf("test done. tid: %lu.\n", (unsigned long)args->tid);
}

void
test_hash_str_dyn_init_hashcode(void)
{
    CraThrdPool pool;
    cra_thrdpool_init(&pool, 4, 4);

    for (int i = 0; i < 4; i++)
        cra_thrdpool_add_task0(&pool, test_str_hashcode);

    cra_thrdpool_wait(&pool);

    cra_thrdpool_uninit(&pool);
}

int
main(void)
{
    test_compare();
    test_hash();
    test_hash_str_dyn_init_hashcode();
    return 0;
}
