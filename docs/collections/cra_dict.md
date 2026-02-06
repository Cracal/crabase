# CraDict

字典

## init

```c
void
cra_dict_init_size(CraDict       *dict,
                   size_t         key_size,
                   size_t         val_size,
                   size_t         init_capacity,
                   bool           zero_memory,
                   cra_hash_fn    hash_key,
                   cra_compare_fn compare_key);
void
cra_dict_init(CraDict       *dict,
              size_t         key_size,
              size_t         val_size,
              bool           zero_memory,
              cra_hash_fn    hash_key,
              cra_compare_fn compare_key);
#define cra_dict_init_size0(TKey, TVal, dict, init_capacity, zero_memory, hash_key_fn, compare_key_fn)
#define cra_dict_init0(TKey, TVal, dict, zero_memory, hash_key_fn, compare_key_fn)
```

初始化

- `key_size` key大小
- `val_size` val大小
- `TKey` key类型
- `TVal` val类型
- `init_capacity` 初始容量。默认是**CRA_DICT_INIT_CAPACITY**
- `zero_memory` 没有有效元素的地方是否清零
- `hash_key` key的hash函数
- `compare_key` key的比较函数

## uninit

```c
void
cra_dict_uninit(CraDict *dict);
```

反初始化

## get count

```c
static inline size_t
cra_dict_get_count(CraDict *dict);
```

获取字典entry个数

## clear

```c
void
cra_dict_clear(CraDict *dict);
```

清空字典

## add

```c
bool
cra_dict_put(CraDict *dict, void *key, void *val, void *retoldkey, void *retoldval);
#define cra_dict_put0(_dict, _key, _val)
bool
cra_dict_add(CraDict *dict, void *key, void *val);
```

添加entry

- `retoldkey` 如果**put**的**key**已存在，则`retoldkey`返回旧key。可为**NULL**
- `retoldval` 如果**put**的**key**已存在，则`retoldval`返回旧val。可为**NULL**

**add**时如果**key**已存在，则该**entry**不会被添加，并返回**false**

## remove

```c
bool
cra_dict_pop(CraDict *dict, void *key, void *retkey, void *retval);
bool
cra_dict_remove(CraDict *dict, void *key);
```

删除entry

- `retkey` 返回被删除的key
- `retval` 返回被删除的val

## get value

```c
bool
cra_dict_get(CraDict *dict, void *key, void *retval);
bool
cra_dict_get_ptr(CraDict *dict, void *key, void **retvalptr);
```

获取value

## clone

```c
CraDict *
cra_dict_clone(CraDict *dict, cra_deep_copy_val_fn deep_copy_key, cra_deep_copy_val_fn deep_copy_val);
```

克隆字典

- `deep_copy_key` 深拷贝**key**
- `deep_copy_val` 深拷贝**val**

## iterator

```c
void
cra_dict_iter_init(CraDict *dict, CraDictIter *it);
bool
cra_dict_iter_next(CraDictIter *it, void **retkeyptr, void **retvalptr);
```

迭代字典
