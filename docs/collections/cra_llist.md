# CraLList

双链表

请先看[数据类型的解释](./cra_collects.md#存放值类型和指针类型)

## 可访问字段

- `head` 链表头结点, 只读
- `count` 当前元素个数，只读
- `itemsize` 元素大小，只读

## init

```c
bool
(cra_llist_init_with_size)(CraLList *list, size_t itemsize, size_t init_spare_node);
bool
cra_llist_init_with_size(T, CraLList *list, size_t init_spare_node);
bool
cra_llist_init(T, CraLList *list);
```

初始化

- `T` 元素类型
- `itemsize` 元素大小
- `init_spare_node` 预先分配的结节个数

成功返回**true**，失败返回**false**

## uninit

```c
void
cra_llist_uninit(CraLList *list);
```

反初始化

## clear

```c
void
cra_llist_clear(CraLList *list);
```

清空链表

## ensure

```c
bool
cra_llist_reserve(CraLList *list, size_t nspare);
```

添加/删除空闲结节。  
如果**nspare**多于当前空闲结节个数，会删除空闲结节。  
如果**nspare**小于当前空闲结节个数，会添加空闲结节。  
仅创建新结点失败时才会返回**false**。

## add

```c
bool
cra_llist_insert(CraLList *list, size_t index, T *val);
bool
cra_llist_prepend(CraLList *list, T *val);
bool
cra_llist_append(CraLList *list, T *val);
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
bool
cra_llist_remove_front(CraLList *list);
bool
cra_llist_remove_back(CraLList *list);

bool
cra_llist_pop_at(CraLList *list, size_t index, out T *retval);
bool
cra_llist_pop_front(CraLList *list, out T *retval);
bool
cra_llist_pop_back(CraLList *list, out T *retval);
```

删除元素  
**retval**为**NULL**时，`pop`等价于`remove`。

`remove_at`: 删除**index**处的元素  
`remove_front`: 删除链表头部的元素  
`remove_back`: 删除链表尾部的元素  
`pop_at`: 弹出**index**处的元素  
`pop_front`: 弹出头部元素  
`pop_back`: 弹出尾部元素  

## get and set

```c
T *
cra_llist_get_ref(CraLList *list, size_t index);
bool
cra_llist_get(CraLList *list, size_t index, out T *retval);
bool
cra_llist_get_and_set(CraLList *list, size_t index, T *newval, out T *retoldval);
bool
cra_llist_set(CraLList *list, size_t index, T *val);

// ============

T *pval = cra_llist_get_ref(list, index);

T val; cra_llist_get(list, index, &val);

T newval = XXX, retoldval; cra_llist_get_and_set(list, index, &newval, &retoldval);

T newval = XXX; cra_llist_set(list, index, &newval);
```

获取/更新元素  
**newval**不可为**NULL**。
**retoldval**不可为**NULL**。

## reverse

```c
void
cra_llist_reverse(CraLList *list);
```

翻转链表

## sort

```c
bool
cra_llist_sort(CraLList *list, int (*compare)(const T *, const T *));
```

对链表进行（快速）排序

- `compare` 比较函数

成功返回**true**，失败返回**false**  
只有`MSVC`才会在临时内存分配失败时返回**false**

## add sort

```c
bool
cra_llist_add_sort(CraLList *list, int (*compare)(const T *, const T *), T *val);
```

有序添加元素  
使用前要保证链表有序，并且之后的添加操作只能使用`add_sort`，这样才能保证链表有序

## 已实现接口

### initializable

```c
CRA_LLIST_INITIALIZABLE_I // llist可初始化接口

// 传递给初始化函数的必要参数
typedef struct CraLListInitializableParam
{
    size_t itemsize;
} CraLListInitializableParam;
// 初始化参数
CRA_LLIST_INITIALIZABLE_PARAM_INIT(T)

// ============

// 1.
CraLListInitializableParam param = CRA_LLIST_INITIALIZABLE_PARAM_INIT(T);
// 2.
CRA_LLIST_INITIALIZABLE_PARAM_DECL(T) param = CRA_LLIST_INITIALIZABLE_PARAM_INIT(T);
// 3.
CRA_LLIST_INITIALIZABLE_PARAM_DEF(param, T);

CraLList *list = cra_alloc(CraLList);
if (!cra_initializable_init(CRA_LLIST_INITIALIZABLE_I, list, INIT_SPARSE_NODE_COUNT, &param))
    printf("init failed");
cra_initializable_uninit(CRA_LLIST_INITIALIZABLE_I, list);
cra_dealloc(list);
```

### appendable

```c
CRA_LLIST_APPENDABLE_I // llist可追加接口

// ============

CraTwoVals vals = {.val1_ref = &val};
if (!cra_appendable_append(CRA_LLIST_APPENDABLE_I, list, &vals))
    printf("append failed");
```

### iterable

```c
CRA_LLIST_ITERABLE_I // llist可迭代接口

// ============

T val;
CraLList *list = ...;
// 正向迭代
CRA_FOREACH(CRA_LLIST_ITERABLE_I, list, vals)
{
    memcpy(&val, vals.val1_ref, sizeof(val));
    printf("val = %??\n", val);
}
// 反向迭代
CRA_FOREACH_REVERSE(CRA_LLIST_ITERABLE_I, list, vals)
{
    memcpy(&val, vals.val1_ref, sizeof(val));
    printf("val = %??\n", val);
}
```

## 直接操作链表结点

```c
// 新建结点
CraLListNode *
cra_llist_create_node(size_t itemsize);

// 销毁结点
void
cra_llist_destroy_node(CraLListNode *node);

// 连接结点
void
cra_llist_link_node(CraLListNode *node, CraLListNode *prev);

// 断开结点
void
cra_llist_unlink_node(CraLList *list, CraLListNode *node);

// 从llist中获取一个空闲结点
CraLListNode *
cra_llist_get_free_node(CraLList *list);

// 归还一个空闲结点到llist中
void
cra_llist_put_free_node(CraLList *list, CraLListNode *node);

// 获取llist中**index**处的结点
CraLListNode *
cra_llist_get_node(CraLList *list, size_t index);

// 向llist的**index**处插入结点
bool
cra_llist_insert_node(CraLList *list, size_t index, CraLListNode *node);

// 将**node**从llist中移除
// 如果**put_to_free_list**为**true**，则将**node**归还到llist的空闲结点列表中
bool
cra_llist_remove_node(CraLList *list, CraLListNode *node, bool put_to_free_list);
```
