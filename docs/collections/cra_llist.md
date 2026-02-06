
# CraLList

双链表

请先看[数据类型的解释](./cra_collects.md#存放值类型和指针类型)

## init

```c
void
cra_llist_init_size(CraLList *list, size_t element_size, size_t init_spare_node, bool zero_memory);
void
cra_llist_init(CraLList *list, size_t element_size, bool zero_memory);
#define cra_llist_init_size0(TVal, list, init_spare_node, zero_memory)
#define cra_llist_init0(TVal, list, zero_memory)
```

初始化

- `element_size` 元素大小
- `TVal` 元素类型
- `init_spare_node` 预先分配的结节个数
- `zero_memory` 没有有效元素的地方是否清零

## uninit

```c
void
cra_llist_uninit(CraLList *list);
```

反初始化

## get count

```c
static inline size_t
cra_llist_get_count(CraLList *list);
```

获取当前元素个数

## clear

```c
void
cra_llist_clear(CraLList *list);
```

清空链表

## add

```c
bool
cra_llist_insert(CraLList *list, size_t index, void *val);
#define cra_llist_prepend(list, val)
#define cra_llist_append(list, val)
```

添加元素

`insert`:  在**index**处插入元素  
`prepend`: 在链表头部添加元素  
`append`:  在链表尾部添加元素  
成功返回**true**，失败返回**false**

## remove

```c
bool
cra_llist_remove_at(CraLList *list, size_t index);
#define cra_llist_remove_front(list)
#define cra_llist_remove_back(list)

bool
cra_llist_pop_at(CraLList *list, size_t index, void *retval);
#define cra_llist_pop_front(list, retval)
#define cra_llist_pop_back(list, retval)

size_t
cra_llist_remove_match(CraLList *list, cra_match_fn match, void *arg);
```

删除元素

`remove_at`: 删除**index**处的元素  
`remove_front`: 删除链表头部的元素  
`remove_back`: 删除链表尾部的元素  
`pop_at`: 弹出**index**处的元素  
`pop_front`: 弹出头部元素  
`pop_back`: 弹出尾部元素  
`remove_match`: 删除匹配的元素。返回被删除的元素个数

## set

```c
bool
cra_llist_set(CraLList *list, size_t index, void *newval);
bool
cra_llist_set_and_pop_old(CraLList *list, size_t index, void *newval, void *retoldval);
```

更新元素

`set_and_pop_old`: 先获取旧元素，再更新

## get

```c
bool
cra_llist_get(CraLList *list, size_t index, void *retval);
bool
cra_llist_get_ptr(CraLList *list, size_t index, void **retvalptr);
```

获取元素

## reverse

```c
void
cra_llist_reverse(CraLList *list);
```

翻转链表

## clone

```c
CraLList *
cra_llist_clone(CraLList *list, cra_deep_copy_val_fn deep_copy_val);
```

克隆链表

- `deep_copy_val` 深拷贝函数

## sort

```c
void
cra_llist_sort(CraLList *list, cra_compare_fn compare);
```

对链表进行排序

- `compare` 比较函数

## add sort

```c
bool
cra_llist_add_sort(CraLList *list, cra_compare_fn compare, void *val);
```

有序添加元素  
使用前要保证链表有序，并且之后的添加操作只能使用`add_sort`，这样才能保证链表有序

## iterator

```c
void
cra_llist_iter_init(CraLList *list, CraLListIter *it);
bool
cra_llist_iter_next(CraLListIter *it, void **retvalptr);
```

迭代链表

## 直接操作链表结点

```c
// 新建结点
CraLListNode *
cra_llist_create_node(size_t element_size);
// 销毁结点
void
cra_llist_destroy_node(CraLListNode **node);
// 获取一个空闲结点
CraLListNode *
cra_llist_get_free_node(CraLList *list);
// 归还一个空闲结点
void
cra_llist_put_free_node(CraLList *list, CraLListNode *node);
// 插入结点
bool
cra_llist_insert_node(CraLList *list, size_t index, CraLListNode *node);
// 断开结点
void
cra_llist_unlink_node(CraLList *list, CraLListNode *node);
// 获取结点
CraLListNode *
cra_llist_get_node(CraLList *list, size_t index);
```
