# 序列化通用

## 元数据

```c
#define CRA_TYPE_META_BEGIN(var)
#define CRA_TYPE_META_BEGIN_CONST(var)
#define CRA_TYPE_META_END()

#define CRA_TYPE_META_MEMBER_BOOL(Type, member, id)
#define CRA_TYPE_META_MEMBER_INT(Type, member, id)
#define CRA_TYPE_META_MEMBER_UINT(Type, member, id)
#define CRA_TYPE_META_MEMBER_VARINT(Type, member, id)
#define CRA_TYPE_META_MEMBER_VARUINT(Type, member, id)
#define CRA_TYPE_META_MEMBER_FLOAT(Type, member, id)
#define CRA_TYPE_META_MEMBER_STRING(Type, member, id)
#define CRA_TYPE_META_MEMBER_BYTES(Type, member, id, is_ptr)
#define CRA_TYPE_META_MEMBER_STRUCT(Type, member, id, is_ptr, member_meta, init_i, arg)
#define CRA_TYPE_META_MEMBER_ARRAY(Type, member, id, is_ptr, element_meta)
#define CRA_TYPE_META_MEMBER_LIST(Type, member, id, is_ptr, element_meta, szer_i, arg)
#define CRA_TYPE_META_MEMBER_LIST(Type, member, id, is_ptr, kv_meta, szer_i, arg)

#define CRA_TYPE_META_ELEMENT_BOOL()
#define CRA_TYPE_META_ELEMENT_INT(type)
#define CRA_TYPE_META_ELEMENT_UINT(type)
#define CRA_TYPE_META_ELEMENT_VARINT(type)
#define CRA_TYPE_META_ELEMENT_VARUINT(type)
#define CRA_TYPE_META_ELEMENT_FLOAT(type)
#define CRA_TYPE_META_ELEMENT_STRING(type, is_ptr)
#define CRA_TYPE_META_ELEMENT_BYTES(type, is_ptr, length_var)
#define CRA_TYPE_META_ELEMENT_STRUCT(type, is_ptr, member_meta, init_i, arg)
#define CRA_TYPE_META_ELEMENT_ARRAY(type, is_ptr, narray_var, element_meta)
#define CRA_TYPE_META_ELEMENT_LIST(type, is_ptr, element_meta, szer_i, arg)
#define CRA_TYPE_META_ELEMENT_DICT(type, is_ptr, kv_meta, szer_i, arg)
```

要对一个对象进行序列化，需要先生成它的元数据  
**MEMBER**序列宏用于定义结构体成员的元数据  
**ELEMENT**序列宏用于定义容器元素/entry的元数据

- `meta_name` 元数据变量名
- `Type` 结构体类型
- `type` 该元数据描述的对象的类型
- `member` 结构体成员名
- `id` 该元数据的ID
- `is_ptr` 是否是指针
- `member_meta` 该结构体的成员元数据
- `element_meta` 该list的元素元数据
- `kv_meta` 该dict的entry元数据
- `length_var` 记录bytes长度的变量
- `narray_var` 记录数组长度的变量
- `init_i` [构造对象接口](#crainitializable_i)
- `szer_i` [序列化对象接口](#craserializable_i)
- `arg` 传给**init**函数的参数

## CraInitializable_i

```c
CraInitializable_i
{
    void (*init)(void *obj, CraInitArgs *arg);
    void (*uninit)(void *obj, bool dont_free_ptr_member);
};
```

该接口只在反序列化中使用  
反序列化时会调用**init**对对象初始化。参数[arg](#crainitargs)是传给**init**的一些必要信息  
**uninit**只会在反序列化失败时才会被调用。**dont_free_ptr_member**永远为**true**，这个参数只是为了提醒用户此**uninit**和其他地方的**uninit**不同。不要在这个**uninit**内free结构体的指针类型成员和容器的指针类型元素

## CraInitArgs

```c
CraInitArgs
{
    size_t size;
    size_t length;
    size_t val1size;
    size_t val2size;
    void  *arg;
};
```

- `size` 对象大小
- `length` 容器元素个数，对于struct，该值为0
- `val1size` 容器第一个元素大小，对于struct, 该值为0
- `val2size` 容器第二个元素大小，对于struct、array和list，该值为0
- `arg` 用户传给meta的参数

## CraSerializable_i

```c
CraSerializable_i
{
    CraInitializable_i init_i;

    void (*iter_init)(void *obj, uint64_t *retlen, void *it, size_t itsize);
    bool (*iter_next)(void *it, void **retval1, void **retval2);
    bool (*add)(void *obj, void *val1, void *val2);
};
```

只有容器类型使用

- `init_i` 见[CraInitializable_i](#crainitializable_i)
- `iter_init`和`iter_next`在序列化时使用
  - `retlen` 返回容器中当前元素个数
  - `it` 迭代器首地址。在函数内部把你的迭代器拷贝给it
  - `itsize` it的大小，一般是64bytes
- `add` 在反序列化时使用。将反序列化出来的容器元素加入容器
  - `val1` list/array的元素; dict的key
  - `val2` dict的value

## CraSerErr

```c
CraSerErr
{
    CraSerErr_e err;
    char        msg[124];
};
```

失败时返回的错误信息

- `err` 错误码
- `msg` 错误信息

## CraSeriObject

```c
CraSeriObject
{
    void       *objptr;
    CraTypeMeta meta[3];
};
```

构造一个可序列化对象。一般直接使用下面的宏

```c
#define CRA_SERI_STRUCT(stru, is_ptr, members_meta, init_i, arg)
#define CRA_SERI_ARRAY(array, is_ptr, narray_var, elements_meta)
#define CRA_SERI_LIST(list, is_ptr, element_meta, szer_i, arg)
#define CRA_SERI_DICT(dict, is_ptr, kv_meta, szer_i, arg)
```

这些参数的解释同[元数据](#元数据)  
如：

```c
struct S s, *p;

CRA_SERI_STRUCT(s, false, ...);
CRA_SERI_STRUCT(p, true, ...);
```
