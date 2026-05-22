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
    CraDeque *deque, deque2;

    deque = cra_alloc(CraDeque);
    assert_always(cra_deque_init_with_size(int, deque, 65));
    assert_always(deque->count == 0);
    assert_always(deque->itemsize == sizeof(int));
    assert_always(deque->narray == 8);
    assert_always(deque->array != NULL);
    assert_always(cra_deque_init(double, &deque2));
    assert_always(deque2.count == 0);
    assert_always(deque2.itemsize == sizeof(double));
    assert_always(deque2.narray == 8);
    assert_always(deque2.array != NULL);

    cra_deque_uninit(deque);
    assert_always(memcmp(deque, &(CraDeque){ 0 }, sizeof(CraDeque)) == 0);
    cra_deque_uninit(&deque2);
    assert_always(memcmp(&deque2, &(CraDeque){ 0 }, sizeof(CraDeque)) == 0);
    cra_dealloc(deque);

    CRA_DEQUE_INITIALIZABLE_PARAM_DEF(param, int);
    assert_always(cra_initializable_init(CRA_DEQUE_INITIALIZABLE_I, &deque2, 513, &param));
    assert_always(deque2.count == 0);
    assert_always(deque2.itemsize == sizeof(int));
    assert_always(deque2.narray == 16);
    assert_always(deque2.array != NULL);

    cra_initializable_uninit(CRA_DEQUE_INITIALIZABLE_I, &deque2);
    assert_always(memcmp(&deque2, &(CraDeque){ 0 }, sizeof(CraDeque)) == 0);
}

void
test_insert(void)
{
    int       val, i;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    assert_always(cra_deque_insert(deque, 0, &(int){ 1 })); // insert head (empty)
    assert_always(deque->count == 1);
    cra_deque_clear(deque);
    assert_always(cra_deque_insert(deque, deque->count, &(int){ 1 })); // insert tail (empty, same to insert head)
    assert_always(deque->count == 1);

    assert_always(cra_deque_insert(deque, deque->count, &(int){ 4 })); // insert tail (not empty)
    assert_always(deque->count == 2);
    assert_always(cra_deque_insert(deque, 1, &(int){ 3 })); // insert middle (not empty)
    assert_always(deque->count == 3);
    assert_always(cra_deque_insert(deque, 0, &(int){ 0 })); // insert head (not empty)
    assert_always(deque->count == 4);
    assert_always(cra_deque_insert(deque, deque->count, &(int){ 5 })); // insert tail (not empty)
    assert_always(deque->count == 5);
    assert_always(cra_deque_insert(deque, 2, &(int){ 2 })); // insert middle (not empty)
    assert_always(deque->count == 6);

    assert_always(!cra_deque_insert(deque, 10000, &(int){ 1 }));
    assert_always(deque->count == 6);

    i = 0;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");
    assert_always(i == 6);

    // insert left

    cra_deque_clear(deque);
    assert_always(deque->count == 0);
    for (i = 0; i < 400; i++)
        assert_always(cra_deque_insert(deque, 0, &(int){ i + 1 }));
    assert_always(deque->count == 400);
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        i--;
    }
    assert_always(i == 0);

    // insert close to left

    cra_deque_clear(deque);
    assert_always(deque->count == 0);
    cra_deque_append(deque, &(int){ 1000 });
    cra_deque_append(deque, &(int){ 2000 });
    cra_deque_append(deque, &(int){ 3000 });
    for (i = 0; i < 50; i++)
        assert_always(cra_deque_insert(deque, 1, &(int){ i + 1 }));
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
        assert_always(cra_deque_insert(deque, deque->count / 2, &(int){ i }));
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

    cra_deque_clear(deque);
    assert_always(deque->count == 0);
    for (i = 0; i < 400; i++)
        assert_always(cra_deque_insert(deque, deque->count, &(int){ i + 1 }));
    assert_always(deque->count == 400);
    CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        i--;
    }
    assert_always(i == 0);

    // insert close to right

    cra_deque_clear(deque);
    assert_always(deque->count == 0);
    cra_deque_append(deque, &(int){ 1000 });
    cra_deque_append(deque, &(int){ 2000 });
    cra_deque_append(deque, &(int){ 3000 });
    for (i = 0; i < 50; i++)
        assert_always(cra_deque_insert(deque, deque->count - 1, &(int){ i + 1 }));
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
        assert_always(cra_deque_insert(deque, deque->count / 2 + 1, &(int){ i + 1 }));
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

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_push(void)
{
    int       val, i;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    assert_always(cra_deque_append(deque, &(int){ 100 }));
    assert_always(deque->count == 1);
    cra_deque_clear(deque);
    assert_always(cra_deque_prepend(deque, &(int){ 200 }));
    assert_always(deque->count == 1);
    cra_deque_clear(deque);

    for (i = 0; i < 1000; i++)
        assert_always(cra_deque_append(deque, &i));
    assert_always(deque->count == 1000);

    assert_always(cra_deque_prepend(deque, &(int){ -1 }));
    assert_always(cra_deque_prepend(deque, &(int){ -2 }));
    assert_always(cra_deque_insert(deque, 4, &(int){ 4000 }));
    assert_always(cra_deque_insert(deque, 4, &(int){ 40000 }));
    CraTwoVals mvals;
    mvals.val1_ref = &(int){ 11 };
    assert_always(cra_appendable_append(CRA_DEQUE_APPENDABLE_I, deque, &mvals));
    mvals.val1_ref = &(int){ 12 };
    assert_always(cra_appendable_append(CRA_DEQUE_APPENDABLE_I, deque, &mvals));
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

    cra_deque_uninit(deque);
    cra_deque_init(int, deque);

    for (i = 0; i < 1000; i++)
        assert_always(cra_deque_prepend(deque, &i));
    assert_always(deque->count == 1000);

    i = 999;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        // printf("%d  ", val);
        assert_always(i-- == val);
    }
    // printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_pop_at(void)
{
    int       val, val2, i;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    assert_always(!cra_deque_pop_at(deque, 0, &val));
    assert_always(!cra_deque_remove_at(deque, 0));
    assert_always(!cra_deque_pop_at(deque, 10, &val));
    assert_always(!cra_deque_remove_at(deque, 10));

    for (i = 0; i < 1000; i++)
        cra_deque_append(deque, &(int){ i + 1 });
    assert_always(deque->count == 1000);

    // pop left
    for (i = 0; i < 100; i++)
    {
        assert_always(cra_deque_pop_at(deque, 0, &val));
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
        assert_always(cra_deque_get(deque, deque->count / 2, &val2));
        assert_always(cra_deque_pop_at(deque, deque->count / 2, &val));
        assert_always(val == val2);
        assert_always(cra_deque_get(deque, deque->count / 2, &val2));
        assert_always(val != val2);
        // printf("%d ", val);
    }
    assert_always(deque->count == 800);
    i = 101;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        if (i == 501)
            i = 601;
        assert_always(val == i++);
        // printf("%d ", val);
    }
    assert_always(i == 1001);

    // pop right

    val2 = 1000;
    for (i = 0; i < 100; i++)
    {
        assert_always(cra_deque_pop_at(deque, deque->count - 1, &val));
        assert_always(val == val2--);
        // printf("%d ", val);
    }
    assert_always(deque->count == 700);
    i = 101;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        if (i == 501)
            i = 601;
        assert_always(val == i++);
        // printf("%d ", val);
    }
    assert_always(i == 901);

    // pop close to right

    for (i = 0; i < 100; i++)
    {
        assert_always(cra_deque_get(deque, deque->count / 2 + 1, &val2));
        assert_always(cra_deque_pop_at(deque, deque->count / 2 + 1, &val));
        assert_always(val == val2);
        assert_always(cra_deque_get(deque, deque->count / 2 + 1, &val2));
        assert_always(val != val2);
        // printf("%d ", val);
    }
    assert_always(deque->count == 600);
    i = 101;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        if (i == 402)
            i = 602;
        assert_always(val == i++);
        // printf("%d ", val);
    }
    assert_always(i == 901);

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_pop(void)
{
    int       val, i;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    assert_always(!cra_deque_pop_back(deque, &i));
    assert_always(!cra_deque_pop_front(deque, &i));
    assert_always(!cra_deque_pop_at(deque, deque->count, &i));
    assert_always(!cra_deque_pop_at(deque, deque->count - 1, &i));
    assert_always(!cra_deque_remove_at(deque, 0));
    assert_always(!cra_deque_remove_at(deque, deque->count));
    assert_always(!cra_deque_remove_at(deque, deque->count - 1));

    for (i = 1; i <= 10000; i++)
        cra_deque_append(deque, &i);
    assert_always(deque->count == 10000);

    assert_always(cra_deque_pop_back(deque, &val) && val == 10000);
    assert_always(cra_deque_pop_front(deque, &val) && val == 1);

    cra_deque_append(deque, &(int){ 200000 });
    cra_deque_prepend(deque, &(int){ -100 });

    assert_always(cra_deque_pop_back(deque, &val) && val == 200000);
    assert_always(cra_deque_pop_front(deque, &val) && val == -100);

    i = 9999;
    while (deque->count > 0)
    {
        assert_always(cra_deque_pop_back(deque, &val));
        assert_always(val == i--);
    }

    assert_always(!cra_deque_remove_back(deque));
    assert_always(!cra_deque_remove_front(deque));

    for (i = 1; i <= 10000; i++)
        cra_deque_prepend(deque, &i);

    i = 10000;
    while (deque->count > 0)
    {
        assert_always(cra_deque_pop_front(deque, &val));
        assert_always(val == i--);
    }

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_set(void)
{
    int       val, i;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    for (i = 0; i < 10000; i++)
        assert_always(cra_deque_append(deque, &i));

    assert_always(cra_deque_set(deque, 3, &(int){ 30000 }));
    assert_always(cra_deque_get(deque, 3, &val) && val == 30000);
    assert_always(cra_deque_get_and_set(deque, 6, &(int){ 60000 }, &val) && val == 6);
    assert_always(cra_deque_get(deque, 6, &val) && val == 60000);
    assert_always(!cra_deque_set(deque, 100000, &(int){ 1000 }));
    assert_always(!cra_deque_get_and_set(deque, 100000, &(int){ 1000 }, &val));

    i = 0;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        // printf("%d  ", val);
        if (i == 3)
            i = 30000;
        else if (i == 30001)
            i = 4;
        else if (i == 6)
            i = 60000;
        else if (i == 60001)
            i = 7;
        assert_always(i++ == val);
    }
    // printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_get(void)
{
    CraDeque deque;
    int      i, val, val2, *pval;

    cra_deque_init_with_size(int, &deque, 10000);
    for (i = 0; i < 10000; i++)
        cra_deque_prepend(&deque, &i);

    i = 0;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, &deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(cra_deque_get(&deque, i, &val2));
        assert_always(!!(pval = cra_deque_get_ref(&deque, i)));
        assert_always(val == *pval && val == val2);
        // printf("%d  ", val);
        i++;
    }
    // printf("\n");

    cra_deque_uninit(&deque);
}

void
test_peek(void)
{
    int       val, *pval;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init_with_size(int, deque, 200);

    for (int i = 0; i < 200; i++)
    {
        if (i % 2 == 0)
            cra_deque_append(deque, &i);
        else
            cra_deque_prepend(deque, &i);
    }

    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        printf("%d  ", val);
    }
    printf("\n");

    assert_always(cra_deque_peek_back(deque, &val) && val == 198);
    assert_always((pval = cra_deque_peek_back_ref(deque)) && val == *pval);

    assert_always(cra_deque_peek_front(deque, &val) && val == 199);
    assert_always((pval = cra_deque_peek_front_ref(deque)) && val == *pval);

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_reverse(void)
{
    int       val, i;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    for (i = 0; i < 2003; i++)
        cra_deque_append(deque, &i);

    // printf("before reverse: ");
    i = 0;
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i++ == val);
        // printf("%d ", val);
    }
    // printf("\n");

    assert_always(cra_deque_reverse(deque));

    // printf("after  reverse: ");
    i = 0;
    CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(val));
        assert_always(i++ == val);
        // printf("%d ", val);
    }
    // printf("\n");

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_foreach(void)
{
    int       i, val;
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    // foreach(empty)
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals) assert_always(false);

    for (i = 0; i < 10; i++)
        cra_deque_append(deque, &i);

    i = 0;
    printf("foreach        : ");
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        printf("%d  ", val);
        i++;
    }
    printf("\n");

    i = 9;
    printf("foreach reverse: ");
    CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals)
    {
        memcpy(&val, vals.val1_ref, sizeof(int));
        assert_always(i == val);
        printf("%d  ", val);
        i--;
    }
    printf("\n");

    cra_deque_clear(deque);

    // foreach(empty)
    CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals) assert_always(false);
    CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals) assert_always(false);

    cra_deque_uninit(deque);
    cra_dealloc(deque);
}

void
test_test(void)
{
    CraDeque *deque = cra_alloc(CraDeque);
    cra_deque_init(int, deque);

    int i, j, n, v;
    srand((unsigned int)time(NULL));
    for (i = 0; i < 10; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_deque_append(deque, &j);

        n = deque->count == 0 ? 0 : (rand() + 1) % deque->count;
        for (j = 0; j < n; j++)
        {
            cra_deque_pop_front(deque, &v);
            assert_always(v == j);
        }

        for (; cra_deque_pop_front(deque, &v); j++)
            assert_always(v == j);
    }
    assert_always(deque->count == 0);

    for (i = 0; i < 100; i++)
    {
        n = (rand() + 1) % 10000;
        for (j = 0; j < n; j++)
            cra_deque_prepend(deque, &j);

        n = deque->count == 0 ? 0 : (rand() + 1) % deque->count;
        for (j = 0; j < n; j++)
        {
            cra_deque_pop_back(deque, &v);
            assert_always(v == j);
        }

        for (; cra_deque_pop_back(deque, &v); j++)
            assert_always(v == j);
    }
    assert_always(deque->count == 0);

    int idx, last_idx;
    int check[10000];
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
        CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
        {
            memcpy(&v, vals.val1_ref, sizeof(v));
            assert_always(v == check[j++]);
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
    test_foreach();
    test_test();

    cra_memory_leak_report();
    return 0;
}
