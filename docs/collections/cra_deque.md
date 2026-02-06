
# CraDeque

双端队列

请先看[数据类型的解释](./cra_collects.md#存放值类型和指针类型)

## init

```c
void
cra_deque_init(CraDeque *deque, size_t element_size, size_t que_max, bool zero_memory);
#define cra_deque_init0(TVal, deque, que_max, zero_memory)
```

初始化

- `element_size` 元素大小
- `TVal` 元素类型
- `que_max` 队列最大容量。可传入**CRA_DEQUE_INFINITE**
- `zero_memory` 没有有效元素的地方是否清零

## uninit

```c
void
cra_deque_uninit(CraDeque *deque);
```

反初始化

## get count

```c
static inline size_t
cra_deque_get_count(CraDeque *deque);
```

获取当前元素个数

## clear

```c
void
cra_deque_clear(CraDeque *deque);
```

清空队列

## add

```c
bool
cra_deque_insert(CraDeque *deque, size_t index, void *val);
#define cra_deque_push_left(deque, val)
#define cra_deque_push(deque, val)
```

添加元素

`insert`:  在**index**处插入元素  
`push_left`: 在队列头部添加元素  
`push`:  在队列尾部添加元素  
成功返回**true**，失败返回**false**

## remove

```c
bool
cra_deque_remove_at(CraDeque *deque, size_t index);

bool
cra_deque_pop_at(CraDeque *deque, size_t index, void *retval);
#define cra_deque_pop_left(deque, retval)
#define cra_deque_pop(deque, retval)

size_t
cra_deque_remove_match(CraDeque *deque, cra_match_fn match, void *arg);
```

删除元素

`remove_at`: 删除**index**处的元素  
`pop_at`: 弹出**index**处的元素  
`pop_left`: 弹出头部元素  
`pop`: 弹出尾部元素  
`remove_match`: 删除匹配的元素。返回被删除的元素个数

## set

```c
bool
cra_deque_set(CraDeque *deque, size_t index, void *newval);
bool
cra_deque_set_and_pop_old(CraDeque *deque, size_t index, void *newval, void *retoldval);
```

更新元素

`set_and_pop_old`: 先获取旧元素，再更新

## get

```c
bool
cra_deque_get(CraDeque *deque, size_t index, void *retval);
bool
cra_deque_get_ptr(CraDeque *deque, size_t index, void **retvalptr);
bool
cra_deque_peek(CraDeque *deque, void *retval);
bool
cra_deque_peek_ptr(CraDeque *deque, void **retvalptr);
bool
cra_deque_peek_left(CraDeque *deque, void *retval);
bool
cra_deque_peek_left_ptr(CraDeque *deque, void **retvalptr);
```

获取元素

## reverse

```c
void
cra_deque_reverse(CraDeque *deque);
```

翻转队列

## clone

```c
CraDeque *
cra_deque_clone(CraDeque *deque, cra_deep_copy_val_fn deep_copy_val);
```

克隆队列

- `deep_copy_val` 深拷贝函数

## iterator

```c
void
cra_deque_iter_init(CraDeque *deque, CraDequeIter *it);
bool
cra_deque_iter_next(CraDequeIter *it, void **retvalptr);
```

迭代队列
