# CraAList

动态数组

请先看[数据类型的解释](./cra_collects.md#存放值类型和指针类型)

## 可访问字段

- `array` 存放元素的数组。直接访问时请用**CRA_ALIST_PVAL(list, index)**宏
- `count` 当前元素个数，只读
- `capacity` 当前容量，只读
- `itemsize` 元素大小，只读

## init

```c
bool
(cra_alist_init_with_size)(CraAList *list, size_t itemsize, size_t init_capacity);
bool
cra_alist_init_with_size(T, CraAList *list, size_t init_capacity);
bool
cra_alist_init(T, CraAList *list);
```

初始化

- `T` 元素类型
- `itemsize` 元素大小
- `init_capacity` 初始容量。默认是**CRA_ALIST_INIT_CAPACITY**

返回值：成功返回**true**，失败返回**false**  
只有为list->array申请内存失败时才会返回**false**

## uninit

```c
void
cra_alist_uninit(CraAList *list);
```

反初始化

## clear

```c
void
cra_alist_clear(CraAList *list);
```

清空数组

## ensure

```c
bool
cra_alist_ensure(CraAList *list, size_t nspare, bool shrink2fit)
```

调用此函数可确保数组容量足够容纳**nspare**个元素。

- `nspare` 需要的空闲空间个数
- `shrink2fit` 当空闲空间超过**nspare**时，是否缩小数组容量到只有**nspare**个空闲空间。

成功返回**true**，失败返回**false**  
只有为扩容失败时才会返回**false**

## add

```c
bool
cra_alist_insert(CraAList *list, size_t index, T *val);
bool
cra_alist_prepend(CraAList *list, T *val);
bool
cra_alist_append(CraAList *list, T *val);
```

添加元素

`insert`:  在**index**处插入元素  
`prepend`: 在数组头部添加元素  
`append`:  在数组尾部添加元素  
成功返回**true**，失败返回**false**

## remove

```c
bool
cra_alist_remove_at(CraAList *list, size_t index);
bool
cra_alist_remove_front(CraAList *list);
bool
cra_alist_remove_back(CraAList *list);

bool
cra_alist_pop_at(CraAList *list, size_t index, out T *retval);
bool
cra_alist_pop_front(CraAList *list, out T *retval);
bool
cra_alist_pop_back(CraAList *list, out T *retval);
```

删除元素  
**retval**为**NULL**时，`pop`等价于`remove`。

`remove_at`: 删除**index**处的元素  
`remove_front`: 删除数组头部的元素  
`remove_back`: 删除数组尾部的元素  
`pop_at`: 弹出**index**处的元素  
`pop_front`: 弹出头部元素  
`pop_back`: 弹出尾部元素  

## get and set

```c
T *
cra_alist_get_ref(CraAList *list, size_t index);
bool
cra_alist_get(CraAList *list, size_t index, out T *retval);
bool
cra_alist_get_and_set(CraAList *list, size_t index, T *newval, out T *retoldval);
bool
cra_alist_set(CraAList *list, size_t index, T *val);

// ============

T *pval = cra_alist_get_ref(list, index);

T val; cra_alist_get(list, index, &val);

T newval = XXX, retoldval; cra_alist_get_and_set(list, index, &newval, &retoldval);

T newval = XXX; cra_alist_set(list, index, &newval);
```

获取/更新元素  
**newval**不可为**NULL**。
**retoldval**不可为**NULL**。

## reverse

```c
bool
cra_alist_reverse(CraAList *list);
```

反转数组  
只有`MSVC`才会在临时内存分配失败时返回**false**

## sort

```c
bool
cra_alist_sort(CraAList *list, int (*compare)(const T *, const T *));
```

对数组进行（快速）排序

- `compare` 比较函数

成功返回**true**，失败返回**false**  
只有`MSVC`才会在临时内存分配失败时返回**false**

## add sort

```c
bool
cra_alist_add_sort(CraAList *list, int (*compare)(const T *, const T *), T *val);
```

有序添加元素  
使用前要保证数组有序，并且之后的添加操作只能使用`add_sort`，这样才能保证数组有序

## 已实现接口

### initializable

```c
CRA_ALIST_INITIALIZABLE_I // alist可初始化接口

// 传递给初始化函数的必要参数
typedef struct CraAListInitializableParam
{
    size_t itemsize;
} CraAListInitializableParam;
// 初始化参数
CRA_ALIST_INITIALIZABLE_PARAM_INIT(T)

// ============

// 1.
CraAListInitializableParam param = CRA_ALIST_INITIALIZABLE_PARAM_INIT(T);
// 2.
CRA_ALIST_INITIALIZABLE_PARAM_DECL(T) param = CRA_ALIST_INITIALIZABLE_PARAM_INIT(T);
// 3.
CRA_ALIST_INITIALIZABLE_PARAM_DEF(param, T);

CraAList *list = cra_alloc(CraAList);
if (!cra_initializable_init(CRA_ALIST_INITIALIZABLE_I, list, INIT_CAPACITY, &param))
    printf("init failed");

size_t count = cra_initializable_get_count(CRA_ALIST_INITIALIZABLE_I, list);

cra_initializable_uninit(CRA_ALIST_INITIALIZABLE_I, list);
cra_dealloc(list);
```

### appendable

```c
CRA_ALIST_APPENDABLE_I // alist可追加接口

// ============

CraTwoVals vals = {.val1_ref = &val};
if (!cra_appendable_append(CRA_ALIST_APPENDABLE_I, list, &vals))
    printf("append failed");
```

### iterable

```c
CRA_ALIST_ITERABLE_I // alist可迭代接口

// ============

T val;
CraAlist *list = ...;
// 正向迭代
CRA_FOREACH(CRA_ALIST_ITERABLE_I, list, vals)
{
    memcpy(&val, vals.val1_ref, sizeof(val));
    printf("val = %??\n", val);
}
// 反向迭代
CRA_FOREACH_REVERSE(CRA_ALIST_ITERABLE_I, list, vals)
{
    memcpy(&val, vals.val1_ref, sizeof(val));
    printf("val = %??\n", val);
}
```
