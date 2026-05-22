# CraDict

字典

## 可访问字段

- `count` 当前key-val对个数,只读
- `capacity` 当前容量，只读
- `key_size` key大小，只读
- `val_size` val大小，只读
- `entry_size` entry大小，只读

## init

```c
bool
(cra_dict_init_with_size)(CraDict    *dict,
                          size_t      key_size,
                          size_t      val_size,
                          size_t      key_offset,
                          size_t      val_offset,
                          size_t      init_capacity,
                          cra_hash_t (*hash_key)(const TKey *key),
                          int (*compare_key)(const TKey *a, const TKey *b));

bool
cra_dict_init_with_size(TKey, TVal, CraDict *dict, size_t init_capacity,
                        cra_hash_t (*hash_key)(const TKey *key),
                        int (*compare_key)(const TKey *a, const TKey *b));
bool
cra_dict_init(TKey, TVal, CraDict *dict, cra_hash_t (*hash_key)(const TKey *key), int (*compare_key)(const TKey *a, const TKey *b));
```

初始化

- `TKey` key类型
- `TVal` val类型
- `init_capacity` 初始容量。默认是**CRA_DICT_DEFAULT_CAPACITY**
- `hash_key` key的hash函数
- `compare_key` key的比较函数

成功返回**true**，失败返回**false**

## uninit

```c
void
cra_dict_uninit(CraDict *dict);
```

反初始化

## clear

```c
void
cra_dict_clear(CraDict *dict);
```

清空字典

## reserve

```c
bool
cra_dict_reserve(CraDict *dict, size_t new_capacity);
```

扩大/缩小字典容量。  
成功返回**true**，失败返回**false**。内存分配失败时返回**false**

## add

```c
bool
cra_dict_put_and_return_kv(CraDict *dict, TKey *key, TVal *val, out TKey *retoldkey, out TVal *retoldval);
bool
cra_dict_put_and_return_v(CraDict *dict, TKey *key, TVal *val, out TVal *retoldval);
bool
cra_dict_put(CraDict *dict, TKey *key, TVal *val);
bool
cra_dict_add(CraDict *dict, TKey *key, TVal *val);
```

添加**key-val**对

成功返回**true**，失败返回**false**。扩容时内存分配失败会返回**false**  
**add**时如果**key**已存在，则该**entry**不会被添加，并返回**false**

## remove

```c
bool
cra_dict_pop_kv(CraDict *dict, const TKey *key, out TKey *retkey, out TVal *retval);
bool
cra_dict_pop(CraDict *dict, const TKey *key, out TVal *retval);
bool
cra_dict_remove(CraDict *dict, const TKey *key);
```

删除**key-val**对

- `retkey` 返回被删除的key
- `retval` 返回被删除的val

## get

```c
TVal *
cra_dict_get_ref(CraDict *dict, const TKey *key);
bool
cra_dict_get(CraDict *dict, const TKey *key, out TVal *retval);
```

获取value

## 已实现接口

### initializable

```c
CRA_DICT_INITIALIZABLE_I // dict可初始化接口

// 传递给初始化函数的必要参数
typedef struct CraDictInitializableParam
{
    size_t      key_size;
    size_t      val_size;
    size_t      key_align;
    size_t      val_align;
    cra_cmp_fn  compare_key;
    cra_hash_fn hash_key;
} CraDictInitializableParam;
// 初始化参数
CRA_DICT_INITIALIZABLE_PARAM_INIT(TKey, TVal, hash_key, compare_key)

// ============

// 1.
CraDictInitializableParam param = CRA_DICT_INITIALIZABLE_PARAM_INIT(TKey, TVal, hash<TKey>, compare<TKey>);
// 2.
CRA_DICT_INITIALIZABLE_PARAM_DECL(TKey) param = CRA_DICT_INITIALIZABLE_PARAM_INIT(TKey, TVal, hash<TKey>, compare<TKey>);
// 3.
CRA_DICT_INITIALIZABLE_PARAM_DEF(param, TKey, TVal, hash<TKey>, compare<TKey>);

CraDict *dict = cra_alloc(CraDict);
if (!cra_initializable_init(CRA_DICT_INITIALIZABLE_I, dict, &param))
    printf("init failed");
cra_initializable_uninit(CRA_DICT_INITIALIZABLE_I, dict);
cra_dealloc(dict);
```

### appendable

```c
CRA_DICT_APPENDABLE_I // dict可追加接口

// ============

CraTwoVals vals = {.val1_ref = &key, .val2_ref = &val};
if (!cra_appendable_append(CRA_DICT_APPENDABLE_I, dict, &vals))
    printf("append failed");
```

### iterable

```c
CRA_DICT_ITERABLE_I // dict可迭代接口

// ============

TKey key;
TVal val;
CraDict *dict = ...;
// 正向迭代
CRA_FOREACH(CRA_DICT_ITERABLE_I, dict, vals)
{
    memcpy(&key, vals.val1_ref, sizeof(key));
    memcpy(&val, vals.val2_ref, sizeof(val));
    printf("{key: %??, val: %??}\n", key, val);
}
// 反向迭代
CRA_FOREACH_REVERSE(CRA_DICT_ITERABLE_I, dict, vals)
{
    memcpy(&key, vals.val1_ref, sizeof(key));
    memcpy(&val, vals.val2_ref, sizeof(val));
    printf("{key: %??, val: %??}\n", key, val);
}
```
