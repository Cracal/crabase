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
    NewCraDeque *deque, deque2;

    deque = cra_alloc(NewCraDeque);
    assert_always(newcra_deque_init_with_size(int, deque, NEWCRA_DEQUE_ITEM_COUNT + 1));
    assert_always(deque->count == 0);
    assert_always(deque->itemsize == sizeof(int));
    assert_always(deque->list.count == 1);
    assert_always(deque->list.nfreelist == 1);
    assert_always(newcra_deque_init(double, &deque2));
    assert_always(deque2.count == 0);
    assert_always(deque2.itemsize == sizeof(double));
    assert_always(deque2.list.count == 1);
    assert_always(deque2.list.nfreelist == 0);

    newcra_deque_uninit(deque);
    assert_always(memcmp(deque, &(NewCraDeque){ 0 }, sizeof(NewCraDeque)) == 0);
    newcra_deque_uninit(&deque2);
    assert_always(memcmp(&deque2, &(NewCraDeque){ 0 }, sizeof(NewCraDeque)) == 0);
    cra_dealloc(deque);

    CRA_DEQUE_INITIALIZABLE_PARAM_DEF(param, int, 100);
    assert_always(cra_initializable_init(CRA_DEQUE_INITIALIZABLE_I, &deque2, &param));
    assert_always(deque2.count == 0);
    assert_always(deque2.itemsize == sizeof(int));
    assert_always(deque2.list.count == 1);
    assert_always(deque2.list.nfreelist == 1);

    cra_initializable_uninit(CRA_DEQUE_INITIALIZABLE_I, &deque2);
    assert_always(memcmp(&deque2, &(NewCraDeque){ 0 }, sizeof(NewCraDeque)) == 0);
}

void
test_insert(void)
{
    int          val, i;
    NewCraDeque *deque = cra_alloc(NewCraDeque);
    newcra_deque_init(int, deque);

    assert_always(newcra_deque_insert(deque, 0, &(int){ 0 }));
    assert_always(deque->count == 1);
    assert_always(newcra_deque_insert(deque, 1, &(int){ 2 }));
    assert_always(deque->count == 2);
    assert_always(newcra_deque_insert(deque, 1, &(int){ 1 }));
    assert_always(deque->count == 3);

    assert_always(!newcra_deque_insert(deque, 10000, &(int){ 1 }));
    assert_always(deque->count == 3);

    i = 0;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");
    assert_always(i == 3);

    // insert left

    newcra_deque_clear(deque);
    assert_always(deque->count == 0);
    for (i = 0; i < 400; i++)
        assert_always(newcra_deque_insert(deque, 0, &(int){ i + 1 }));
    assert_always(deque->count == 400);
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        i--;
    }
    assert_always(i == 0);

    // insert close to left

    newcra_deque_clear(deque);
    assert_always(deque->count == 0);
    newcra_deque_push(deque, &(int){ 1000 });
    newcra_deque_push(deque, &(int){ 2000 });
    newcra_deque_push(deque, &(int){ 3000 });
    for (i = 0; i < 50; i++)
        assert_always(newcra_deque_insert(deque, 1, &(int){ i + 1 }));
    assert_always(deque->count == 53);
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        if (val != 1000 && val != 2000 && val != 3000)
            assert_always(i-- == val);
        // printf("%d  ", val);
    }
    assert_always(i == 0);
    for (i = 100; i < 450; i++)
        assert_always(newcra_deque_insert(deque, deque->count / 2, &(int){ i }));
    assert_always(deque->count == 403);
    int check_ints[] = {
        1000, 50,  49,  48,  47,  46,   45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32,  31,  30,
        29,   28,  27,  26,  100, 102,  104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134,
        136,  138, 140, 142, 144, 146,  148, 150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178,
        180,  182, 184, 186, 188, 190,  192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222,
        224,  226, 228, 230, 232, 234,  236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 256, 258, 260, 262, 264, 266,
        268,  270, 272, 274, 276, 278,  280, 282, 284, 286, 288, 290, 292, 294, 296, 298, 300, 302, 304, 306, 308, 310,
        312,  314, 316, 318, 320, 322,  324, 326, 328, 330, 332, 334, 336, 338, 340, 342, 344, 346, 348, 350, 352, 354,
        356,  358, 360, 362, 364, 366,  368, 370, 372, 374, 376, 378, 380, 382, 384, 386, 388, 390, 392, 394, 396, 398,
        400,  402, 404, 406, 408, 410,  412, 414, 416, 418, 420, 422, 424, 426, 428, 430, 432, 434, 436, 438, 440, 442,
        444,  446, 448, 449, 447, 445,  443, 441, 439, 437, 435, 433, 431, 429, 427, 425, 423, 421, 419, 417, 415, 413,
        411,  409, 407, 405, 403, 401,  399, 397, 395, 393, 391, 389, 387, 385, 383, 381, 379, 377, 375, 373, 371, 369,
        367,  365, 363, 361, 359, 357,  355, 353, 351, 349, 347, 345, 343, 341, 339, 337, 335, 333, 331, 329, 327, 325,
        323,  321, 319, 317, 315, 313,  311, 309, 307, 305, 303, 301, 299, 297, 295, 293, 291, 289, 287, 285, 283, 281,
        279,  277, 275, 273, 271, 269,  267, 265, 263, 261, 259, 257, 255, 253, 251, 249, 247, 245, 243, 241, 239, 237,
        235,  233, 231, 229, 227, 225,  223, 221, 219, 217, 215, 213, 211, 209, 207, 205, 203, 201, 199, 197, 195, 193,
        191,  189, 187, 185, 183, 181,  179, 177, 175, 173, 171, 169, 167, 165, 163, 161, 159, 157, 155, 153, 151, 149,
        147,  145, 143, 141, 139, 137,  135, 133, 131, 129, 127, 125, 123, 121, 119, 117, 115, 113, 111, 109, 107, 105,
        103,  101, 25,  24,  23,  22,   21,  20,  19,  18,  17,  16,  15,  14,  13,  12,  11,  10,  9,   8,   7,   6,
        5,    4,   3,   2,   1,   2000, 3000
    };
    i = 0;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(check_ints[i++] == val);
        // printf("%d ", val);
    }
    assert_always(i == 403);

    // insert right

    newcra_deque_clear(deque);
    assert_always(deque->count == 0);
    for (i = 0; i < 400; i++)
        assert_always(newcra_deque_insert(deque, deque->count, &(int){ i + 1 }));
    assert_always(deque->count == 400);
    CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        i--;
    }
    assert_always(i == 0);

    // insert close to right

    newcra_deque_clear(deque);
    assert_always(deque->count == 0);
    newcra_deque_push(deque, &(int){ 1000 });
    newcra_deque_push(deque, &(int){ 2000 });
    newcra_deque_push(deque, &(int){ 3000 });
    for (i = 0; i < 50; i++)
        assert_always(newcra_deque_insert(deque, deque->count - 1, &(int){ i + 1 }));
    assert_always(deque->count == 53);
    CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        if (val != 1000 && val != 2000 && val != 3000)
            assert_always(i-- == val);
        // printf("%d  ", val);
    }
    assert_always(i == 0);
    for (i = 100; i < 450; i++)
        assert_always(newcra_deque_insert(deque, deque->count / 2 + 1, &(int){ i + 1 }));
    assert_always(deque->count == 403);
    int check_ints2[] = {
        1000, 2000, 1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
        21,   22,   23,  24,  25,  101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133,
        135,  137,  139, 141, 143, 145, 147, 149, 151, 153, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177,
        179,  181,  183, 185, 187, 189, 191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221,
        223,  225,  227, 229, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251, 253, 255, 257, 259, 261, 263, 265,
        267,  269,  271, 273, 275, 277, 279, 281, 283, 285, 287, 289, 291, 293, 295, 297, 299, 301, 303, 305, 307, 309,
        311,  313,  315, 317, 319, 321, 323, 325, 327, 329, 331, 333, 335, 337, 339, 341, 343, 345, 347, 349, 351, 353,
        355,  357,  359, 361, 363, 365, 367, 369, 371, 373, 375, 377, 379, 381, 383, 385, 387, 389, 391, 393, 395, 397,
        399,  401,  403, 405, 407, 409, 411, 413, 415, 417, 419, 421, 423, 425, 427, 429, 431, 433, 435, 437, 439, 441,
        443,  445,  447, 449, 450, 448, 446, 444, 442, 440, 438, 436, 434, 432, 430, 428, 426, 424, 422, 420, 418, 416,
        414,  412,  410, 408, 406, 404, 402, 400, 398, 396, 394, 392, 390, 388, 386, 384, 382, 380, 378, 376, 374, 372,
        370,  368,  366, 364, 362, 360, 358, 356, 354, 352, 350, 348, 346, 344, 342, 340, 338, 336, 334, 332, 330, 328,
        326,  324,  322, 320, 318, 316, 314, 312, 310, 308, 306, 304, 302, 300, 298, 296, 294, 292, 290, 288, 286, 284,
        282,  280,  278, 276, 274, 272, 270, 268, 266, 264, 262, 260, 258, 256, 254, 252, 250, 248, 246, 244, 242, 240,
        238,  236,  234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 200, 198, 196,
        194,  192,  190, 188, 186, 184, 182, 180, 178, 176, 174, 172, 170, 168, 166, 164, 162, 160, 158, 156, 154, 152,
        150,  148,  146, 144, 142, 140, 138, 136, 134, 132, 130, 128, 126, 124, 122, 120, 118, 116, 114, 112, 110, 108,
        106,  104,  102, 26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
        45,   46,   47,  48,  49,  50,  3000
    };
    i = 0;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(check_ints2[i++] == val);
        // printf("%d ", val);
    }
    assert_always(i == 403);

    newcra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_push(void)
{
    int          val, i;
    NewCraDeque *deque = cra_alloc(NewCraDeque);
    newcra_deque_init(int, deque);

    assert_always(newcra_deque_push(deque, &(int){ 100 }));
    assert_always(deque->count == 1);
    newcra_deque_clear(deque);
    assert_always(newcra_deque_push_left(deque, &(int){ 200 }));
    assert_always(deque->count == 1);
    newcra_deque_clear(deque);

    for (i = 0; i < 1000; i++)
        assert_always(newcra_deque_push(deque, &i));
    assert_always(deque->count == 1000);

    assert_always(newcra_deque_push_left(deque, &(int){ -1 }));
    assert_always(newcra_deque_push_left(deque, &(int){ -2 }));
    assert_always(newcra_deque_insert(deque, 4, &(int){ 4000 }));
    assert_always(newcra_deque_insert(deque, 4, &(int){ 40000 }));
    assert_always(newcra_deque_push(deque, &(int){ 11 }));
    assert_always(newcra_deque_push(deque, &(int){ 12 }));
    assert_always(deque->count == 1006);

    i = -2;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
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
        assert_always(i++ == val);
    }
    // printf("\n");

    newcra_deque_clear(deque);

    for (i = 0; i < 1000; i++)
        assert_always(newcra_deque_push_left(deque, &i));

    i = 999;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        // printf("%d  ", val);
        assert_always(i-- == val);
    }
    // printf("\n");

    newcra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_pop_at(void)
{
    int          val, val2, i, j;
    NewCraDeque *deque = cra_alloc(NewCraDeque);
    newcra_deque_init(int, deque);

    assert_always(!newcra_deque_pop_at(deque, 0, &val));
    assert_always(!newcra_deque_remove_at(deque, 0));
    assert_always(!newcra_deque_pop_at(deque, 10, &val));
    assert_always(!newcra_deque_remove_at(deque, 10));

    for (i = 0; i < 1000; i++)
        newcra_deque_push(deque, &(int){ i + 1 });
    assert_always(deque->count == 1000);

    // pop left
    for (i = 0; i < 100; i++)
    {
        assert_always(newcra_deque_pop_at(deque, 0, &val));
        assert_always(val == i + 1);
        // printf("%d ", val);
    }
    assert_always(deque->count == 900);
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(val == ++i);
        // printf("%d ", val);
    }
    assert_always(i == 1000);

    // pop close to left

    for (i = 0; i < 100; i++)
    {
        assert_always(newcra_deque_get(deque, deque->count / 2, &val2));
        assert_always(newcra_deque_pop_at(deque, deque->count / 2, &val));
        assert_always(val == val2);
        assert_always(newcra_deque_get(deque, deque->count / 2, &val2));
        assert_always(val != val2);
        // printf("%d ", val);
    }
    assert_always(deque->count == 800);
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(val <= 500 || val > 600);
        // printf("%d ", val);
    }

    // while (deque->count > 0)
    // {
    //     j = rand() % deque->count;
    //     assert_always(newcra_deque_pop_at(deque, j, &val));
    //     assert_always(j == val);
    // }

    CRA_UNUSED_VALUE(j);
    newcra_deque_uninit(deque);
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
    A_s *ret = (A_s *)cra_malloc(sizeof(A_s));
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
        as = (A_s *)cra_malloc(sizeof(A_s));
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

        n = deque->count == 0 ? 0 : (rand() + 1) % deque->count;
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

        n = deque->count == 0 ? 0 : (rand() + 1) % deque->count;
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
    test_insert();
    test_push();
    test_pop_at();
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
