# CraDeque

双端队列

请先看[数据类型的解释](./cra_collects.md#存放值类型和指针类型)

## 可访问字段

- `count` 当前元素个数，只读
- `itemsize` 元素大小，只读

## init

```c
bool
(cra_deque_init_with_size)(CraDeque *deque, size_t itemsize, size_t init_capacity);
bool
cra_deque_init_with_size(T, CraDeque *deque, size_t init_capacity);
bool
cra_deque_init(T, CraDeque *deque);
```

初始化

- `T` 元素类型
- `itemsize` 元素大小
- `init_capacity` 初始化容量。deque据此分配桶空间

成功返回**true**，失败返回**false**  
只有申请内存失败时才会返回**false**

## uninit

```c
void
cra_deque_uninit(CraDeque *deque);
```

反初始化

## clear

```c
void
cra_deque_clear(CraDeque *deque);
```

清空队列

## add

```c
bool
cra_deque_insert(CraDeque *deque, size_t index, T *val);
bool
cra_deque_prepend(CraDeque *deque, T *val);
bool
cra_deque_append(CraDeque *deque, T *val);
```

添加元素

`insert`:  在**index**处插入元素  
`prepend`: 在队列头部添加元素  
`append`:  在队列尾部添加元素  
成功返回**true**，失败返回**false**

## remove and pop

```c
bool
cra_deque_remove_at(CraDeque *deque, size_t index);
bool
cra_deque_remove_front(CraDeque *deque);
bool
cra_deque_remove_back(CraDeque *deque);

bool
cra_deque_pop_at(CraDeque *deque, size_t index, out T *retval);
bool
cra_deque_pop_front(CraDeque *deque, out T *retval);
bool
cra_deque_pop_back(CraDeque *deque, out T *retval);
```

删除/弹出元素

`remove_at`: 删除**index**处的元素  
`remove_front`: 删除头部元素  
`remove_back`: 删除尾部元素  
`pop_at`: 弹出**index**处的元素  
`pop_front`: 弹出头部元素  
`pop_back`: 弹出尾部元素  

## get and set

```c
bool
cra_deque_get(CraDeque *deque, size_t index, out T *retval);
T *
cra_deque_get_ref(CraDeque *deque, size_t index);

bool
cra_deque_peek_front(CraDeque *deque, out T *retval);
T *
cra_deque_peek_front_ref(CraDeque *deque);
bool
cra_deque_peek_back(CraDeque *deque, out T *retval);
T *
cra_deque_peek_back_ref(CraDeque *deque);

bool
cra_deque_set(CraDeque *deque, size_t index, T *newval);
bool
cra_deque_get_and_set(CraDeque *deque, size_t index, T *newval, out T *retoldval);
```

获取/更新元素  
**newval**不可为**NULL**。
**retoldval**不可为**NULL**。

## reverse

```c
void
cra_deque_reverse(CraDeque *deque);
```

翻转队列

## 已实现接口

### initializable

```c
CRA_DEQUE_INITIALIZABLE_I // deque可初始化接口

// 传递给初始化函数的必要参数
typedef struct CraDequeInitializableParam
{
    size_t itemsize;
    size_t init_capacity;
} CraDequeInitializableParam;
// 初始化参数
CRA_DEQUE_INITIALIZABLE_PARAM_INIT(T, init_capacity)

// ============

// 1.
CraDequeInitializableParam param = CRA_DEQUE_INITIALIZABLE_PARAM_INIT(T, 0);
// 2.
CRA_DEQUE_INITIALIZABLE_PARAM_DECL(T) param = CRA_DEQUE_INITIALIZABLE_PARAM_INIT(T, 0);
// 3.
CRA_DEQUE_INITIALIZABLE_PARAM_DEF(param, T, 0);

CraDeque *deque = cra_alloc(CraDeque);
if (!cra_initializable_init(CRA_DEQUE_INITIALIZABLE_I, deque, &param))
    printf("init failed");
cra_initializable_uninit(CRA_DEQUE_INITIALIZABLE_I, deque);
cra_dealloc(deque);
```

### appendable

```c
CRA_DEQUE_APPENDABLE_I // deque可追加接口

// ============

CraTwoVals vals = {.val1_ref = &val};
if (!cra_appendable_append(CRA_DEQUE_APPENDABLE_I, deque, &vals))
    printf("append failed");
```

### iterable

```c
CRA_DEQUE_ITERABLE_I // deque可迭代接口

// ============

T val;
CraAlist *list = ...;
// 正向迭代
CRA_FOREACH(CRA_DEQUE_ITERABLE_I, deque, vals)
{
    memcpy(&val, vals.val1_ref, sizeof(val));
    printf("val = %??\n", val);
}
// 反向迭代
CRA_FOREACH_REVERSE(CRA_DEQUE_ITERABLE_I, deque, vals)
{
    memcpy(&val, vals.val1_ref, sizeof(val));
    printf("val = %??\n", val);
}
```
