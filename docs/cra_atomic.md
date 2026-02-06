# CraAtomic

原子操作。为了兼容**MSVC**只支持三种类型：

- `cra_atomic_int32_t`
- `cra_atomic_int64_t`
- `cra_atomic_flag_t`

## load

```c
static inline int32_t
cra_atomic_load32(cra_atomic_int32_t *p);

static inline int64_t
cra_atomic_load64(cra_atomic_int64_t *p);
```

获取值，有可能不是原子操作

## store

```c
static inline void
cra_atomic_store32(cra_atomic_int32_t *p, int32_t v);

static inline void
cra_atomic_store64(cra_atomic_int64_t *p, int64_t v);
```

设置值，有可能不是原子操作

## add

```c
static inline int32_t
cra_atomic_add32(cra_atomic_int32_t *p, int32_t v);

static inline int64_t
cra_atomic_add64(cra_atomic_int64_t *p, int64_t v);
```

将`*p`加上`v`的结果更新到`*p`上，返回`*p`的旧值

## sub

```c
static inline int32_t
cra_atomic_sub32(cra_atomic_int32_t *p, int32_t v);

static inline int64_t
cra_atomic_sub64(cra_atomic_int64_t *p, int64_t v);
```

将`*p`减去`v`的结果更新到`*p`上，返回`*p`的旧值

## inc

```c
static inline int32_t
cra_atomic_inc32(cra_atomic_int32_t *p);

static inline int64_t
cra_atomic_inc64(cra_atomic_int64_t *p);
```

使`*p`自增1，返回`*p`的旧值

## dec

```c
static inline int32_t
cra_atomic_dec32(cra_atomic_int32_t *p);

static inline int64_t
cra_atomic_dec64(cra_atomic_int64_t *p);
```

使`*p`自减1，返回`*p`的旧值

## compare and set

```c
static inline bool
cra_atomic_compare_and_set32(cra_atomic_int32_t *p, int32_t cmp_val, int32_t set_val);

static inline bool
cra_atomic_compare_and_set64(cra_atomic_int64_t *p, int64_t cmp_val, int64_t set_val);
```

比较`*p`和`cmp_val`的值，  
如果两者相等，将`*p`更新为`set_val`并返回**true**  
如果两者不相等，直接返回**false**

## flag test and set

```c
static inline bool
cra_atomic_flag_test_and_set(cra_atomic_flag_t *p);
```

测试`*p`是否为**true**，  
如果是**true**，直接返回**true**  
如果是**false**，将`*p`更新为**true**，然后返回**false**

## flag clear

```c
static inline void
cra_atomic_flag_clear(cra_atomic_flag_t *p);
```

无条件将`*p`更新为**false**
