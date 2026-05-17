/**
 * @file cra_deque.h
 * @author Cracal
 * @brief 双端队列
 * @version 0.1
 * @date 2024-10-15
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __CRA_DEQUE_H__
#define __CRA_DEQUE_H__
#include "cra_llist.h"

#if 1
typedef struct _CraDeque
{
    bool     zero_memory;
    size_t   ele_size;
    size_t   count;
    size_t   que_max;
    size_t   left_idx;
    size_t   right_idx;
    CraLList list;
} CraDeque;

typedef struct _CraDequeIter
{
    size_t        index;
    CraLListNode *curr;
    CraDeque     *deque;
} CraDequeIter;

#define CRA_DEQUE_INFINITE SIZE_MAX

CRA_API void
cra_deque_iter_init(CraDeque *deque, CraDequeIter *it);

CRA_API bool
cra_deque_iter_next(CraDequeIter *it, void **retvalptr);

CRA_API bool
cra_deque_init(CraDeque *deque, size_t element_size, size_t que_max, bool zero_memory);
#define cra_deque_init0(_TVal, _deque, _que_max, _zero_memory)    \
    cra_deque_init(_deque, sizeof(_TVal), _que_max, _zero_memory)

CRA_API void
cra_deque_uninit(CraDeque *deque);

static inline size_t
cra_deque_get_count(CraDeque *deque)
{
    return deque->count;
}

CRA_API void
cra_deque_clear(CraDeque *deque);

CRA_API bool
cra_deque_insert(CraDeque *deque, size_t index, void *val);

CRA_API bool
cra_deque_push(CraDeque *deque, void *val);

CRA_API bool
cra_deque_push_left(CraDeque *deque, void *val);

CRA_API bool
cra_deque_remove_at(CraDeque *deque, size_t index);

CRA_API bool
cra_deque_pop_at(CraDeque *deque, size_t index, void *retval);

CRA_API bool
cra_deque_pop(CraDeque *deque, void *retval);

CRA_API bool
cra_deque_pop_left(CraDeque *deque, void *retval);

CRA_API bool
cra_deque_set(CraDeque *deque, size_t index, void *newval);

CRA_API bool
cra_deque_set_and_pop_old(CraDeque *deque, size_t index, void *newval, void *retoldval);

CRA_API bool
cra_deque_get(CraDeque *deque, size_t index, void *retval);

CRA_API bool
cra_deque_get_ptr(CraDeque *deque, size_t index, void **retvalptr);

CRA_API bool
cra_deque_peek(CraDeque *deque, void *retval);

CRA_API bool
cra_deque_peek_ptr(CraDeque *deque, void **retvalptr);

CRA_API bool
cra_deque_peek_left(CraDeque *deque, void *retval);

CRA_API bool
cra_deque_peek_left_ptr(CraDeque *deque, void **retvalptr);

CRA_API void
cra_deque_reverse(CraDeque *deque);

CRA_API CraDeque *
cra_deque_clone(CraDeque *deque, cra_deep_copy_val_fn deep_copy_val);
#endif

#define NEWCRA_DEQUE_ITEM_COUNT         64
#define NEWCRA_DEQUE_ITEM_INDEX(_index) ((_index) & (NEWCRA_DEQUE_ITEM_COUNT - 1)) // index % NEWCRA_DEQUE_ITEM_COUNT
#define NEWCRA_DEQUE_CHECK_VAL(_deque, _val) assert(sizeof(*(_val)) == (_deque)->itemsize)

typedef struct NewCraDeque NewCraDeque;
struct NewCraDeque
{
    CraLList list;
    size_t   count;
    size_t   left_idx;
    size_t   right_idx;
    size_t   itemsize;
};

CRA_API bool
newcra_deque_init_with_size(NewCraDeque *deque, size_t itemsize, size_t init_capacity);
// bool init_with_size<T>(NewCraDeque *deque, size_t init_capacity)
#define newcra_deque_init_with_size(_T, _deque, _init_capacity)     \
    newcra_deque_init_with_size(_deque, sizeof(_T), _init_capacity)
// bool init<T>(NewCraDeque *deque)
#define newcra_deque_init(_T, _deque) newcra_deque_init_with_size(_T, _deque, 0)

CRA_API void
newcra_deque_uninit(NewCraDeque *deque);

CRA_API void
newcra_deque_clear(NewCraDeque *deque);

CRA_API bool
newcra_deque_insert(NewCraDeque *deque, size_t index, void *val);
CRA_API bool
newcra_deque_push_left(NewCraDeque *deque, void *val);
CRA_API bool
newcra_deque_push(NewCraDeque *deque, void *val);
// bool insert(NewCraDeque *deque, size_t index, T *val)
#define newcra_deque_insert(_deque, _index, _val)                                     \
    (NEWCRA_DEQUE_CHECK_VAL(_deque, _val), newcra_deque_insert(_deque, _index, _val))
// bool push_front(NewCraDeque *deque, T *val)
#define newcra_deque_push_left(_deque, _val)                                     \
    (NEWCRA_DEQUE_CHECK_VAL(_deque, _val), newcra_deque_push_left(_deque, _val))
// bool push_back(NewCraDeque *deque, T *val)
#define newcra_deque_push(_deque, _val) (NEWCRA_DEQUE_CHECK_VAL(_deque, _val), newcra_deque_push(_deque, _val))

CRA_API bool
newcra_deque_pop_at(NewCraDeque *deque, size_t index, void *retval);
CRA_API bool
newcra_deque_pop_left(NewCraDeque *deque, void *retval);
CRA_API bool
newcra_deque_pop(NewCraDeque *deque, void *retval);
// bool pop_at(NewCraDeque *deque, size_t index, out T *retval)
#define newcra_deque_pop_at(_deque, _index, _retval)                                        \
    (NEWCRA_DEQUE_CHECK_VAL(_deque, _retval), newcra_deque_pop_at(_deque, _index, _retval))
// bool pop_front(NewCraDeque *deque, out T *retval)
#define newcra_deque_pop_left(_deque, _retval)                                        \
    (NEWCRA_DEQUE_CHECK_VAL(_deque, _retval), newcra_deque_pop_left(_deque, _retval))
// bool pop_back(NewCraDeque *deque, out T *retval)
#define newcra_deque_pop(_deque, _retval) (NEWCRA_DEQUE_CHECK_VAL(_deque, _retval), newcra_deque_pop(_deque, _retval))

// bool remove_at(NewCraDeque *deque, size_t index)
#define newcra_deque_remove_at(_deque, _index) (newcra_deque_pop_at)(_deque, _index, NULL)
// bool remove_left(NewCraDeque *deque)
#define newcra_deque_remove_left(_deque)       (newcra_deque_pop_left)(_deque, NULL)
// bool remove(NewCraDeque *deque)
#define newcra_deque_remove(_deque)            (newcra_deque_pop)(_deque, NULL)

CRA_API void *
newcra_deque_get_ref(NewCraDeque *deque, size_t index);

static inline bool
newcra_deque_get(NewCraDeque *deque, size_t index, void *retval)
{
    void *pval = newcra_deque_get_ref(deque, index);
    if (pval && retval)
        memcpy(retval, pval, deque->itemsize);
    return pval != NULL;
}

// ====================================== interfaces ======================================

// initializable

typedef struct CraDequeInitializableParam
{
    size_t itemsize;
    size_t init_capacity;
} CraDequeInitializableParam;
#define CRA_DEQUE_INITIALIZABLE_PARAM_INIT(_T, _init_capacity) { sizeof(_T), _init_capacity }
#define CRA_DEQUE_INITIALIZABLE_PARAM_DECL(_var_name)          CraDequeInitializableParam _var_name
#define CRA_DEQUE_INITIALIZABLE_PARAM_DEF(_var_name, _T, _init_capacity)                                   \
    CRA_DEQUE_INITIALIZABLE_PARAM_DECL(_var_name) = CRA_DEQUE_INITIALIZABLE_PARAM_INIT(_T, _init_capacity)

CRA_API CRA_INITIALIZABLE_DEF(cra_g_deque_initializable_i);
#define CRA_DEQUE_INITIALIZABLE_I (&cra_g_deque_initializable_i)

// appendable

CRA_API CRA_APPENDABLE_DEF(cra_g_deque_appendable_i);
#define CRA_DEQUE_APPENDABLE_I (&cra_g_deque_appendable_i)

// iterable

CRA_API CRA_ITERABLE_DEF(cra_g_deque_iterable_i);
#define CRA_DEQUE_ITERABLE_I (&cra_g_deque_iterable_i)

#endif