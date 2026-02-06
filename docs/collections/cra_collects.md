# 容器通用

## 存放值类型和指针类型

### 值类型

不是指针类型的都是值类型，包括struct, enum和union

```c
init(object, sizeof(T), ...);
init0(T, object, ...);

T val;

get(object, index, &val);
add(object, &val);

T *pval;

get_ptr(object, index, &pval);
val == *pval;

iter_next(&it, &pval);
```

### 指针类型

所有带`*`的都是指针类型

```c
init(object, sizeof(T *), ...);
init0(T *, object);

T *val;

get(object, index, &val);
add(object, &val);

T **pval;

get_ptr(object, index, &pval);
val == *pval;

iter_next(&it, &pval);
```

永远不要忘记取地址符

## 比较函数

```c
static inline int
cra_compare_T(T a, T b);
int
cra_compare_T_p(T *a, T *b);

// 如:
static inline int
cra_compare_int32_t(int32_t a, int32_t b);

int
cra_comapre_string_p(const char **a, const char **b);
```

## hash函数

```c
static inline cra_hash_t
cra_hash_T(T val);
cra_hash_t
cra_hash_T_p(T *val);

// 如:
cra_hash_t
cra_hash_double_p(double *val);

cra_hash_t
cra_hash_string1(const char *val);
cra_hash_t
cra_hash_string2_p(const char **val);
```
