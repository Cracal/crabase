# CraAList

动态数组

请先看[数据类型的解释](./cra_collects.md#存放值类型和指针类型)

## init

```c
void
cra_alist_init_size(CraAList *list, size_t element_size, size_t init_capacity, bool zero_memory);
void
cra_alist_init(CraAList *list, size_t element_size, bool zero_memory);
#define cra_alist_init_size0(TVal, list, init_capacity, zero_memory)
#define cra_alist_init0(TVal, list, zero_memory)
```

初始化

- `element_size` 元素大小
- `TVal` 元素类型
- `init_capacity` 初始容量。默认是**CRA_ALIST_INIT_CAPACITY**
- `zero_memory` 没有有效元素的地方是否清零

## uninit

```c
void
cra_alist_uninit(CraAList *list);
```

反初始化

## get count

```c
static inline size_t
cra_alist_get_count(CraAList *list);
```

获取当前元素个数

## clear

```c
void
cra_alist_clear(CraAList *list);
```

清空数组

## resize

```c
bool
cra_alist_resize(CraAList *list, size_t new_capacity);
```

重新设置数组大小  
`new_capacity`小于当前元素个数时返回**false**，  
否则将数组容量设为`new_capacity`并反回**true**

## add

```c
bool
cra_alist_insert(CraAList *list, size_t index, void *val);
#define cra_alist_prepend(list, val)
#define cra_alist_append(list, val)
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
#define cra_alist_remove_front(list)
#define cra_alist_remove_back(list)

bool
cra_alist_pop_at(CraAList *list, size_t index, void *retval);
#define cra_alist_pop_front(list, retval)
#define cra_alist_pop_back(list, retval)

size_t
cra_alist_remove_match(CraAList *list, cra_match_fn match, void *arg);
```

删除元素

`remove_at`: 删除**index**处的元素  
`remove_front`: 删除数组头部的元素  
`remove_back`: 删除数组尾部的元素  
`pop_at`: 弹出**index**处的元素  
`pop_front`: 弹出头部元素  
`pop_back`: 弹出尾部元素  
`remove_match`: 删除匹配的元素。返回被删除的元素个数

## set

```c
bool
cra_alist_set(CraAList *list, size_t index, void *newval);
bool
cra_alist_set_and_pop_old(CraAList *list, size_t index, void *newval, void *retoldval);
```

更新元素

`set_and_pop_old`: 先获取旧元素，再更新

## get

```c
bool
cra_alist_get(CraAList *list, size_t index, void *retval);
bool
cra_alist_get_ptr(CraAList *list, size_t index, void **retvalptr);
```

获取元素

## reverse

```c
void
cra_alist_reverse(CraAList *list);
```

翻转数组

## clone

```c
CraAList *
cra_alist_clone(CraAList *list, cra_deep_copy_val_fn deep_copy_val);
```

克隆数组

- `deep_copy_val` 深拷贝函数

## sort

```c
void
cra_alist_sort(CraAList *list, cra_compare_fn compare);
```

对数组进行排序

- `compare` 比较函数

## add sort

```c
bool
cra_alist_add_sort(CraAList *list, cra_compare_fn compare, void *val);
```

有序添加元素  
使用前要保证数组有序，并且之后的添加操作只能使用`add_sort`，这样才能保证数组有序

## iterator

```c
void
cra_alist_iter_init(CraAList *list, CraAListIter *it);
bool
cra_alist_iter_next(CraAListIter *it, void **retvalptr);
```

迭代数组
