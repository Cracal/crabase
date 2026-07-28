# reference

## 引用计数

```c
typedef struct CraRefHead CraRefHead, CraRef;
```

### 侵入式

将`CraRefHead`嵌入到结构体中，即可实现侵入式引用计数。

#### 工具函数

##### cra_ref_head_init

```c
void
cra_ref_head_init(CraRefHead *head);
```

初始化引用计数为1

##### cra_ref_head_ref

```c
void
cra_ref_head_ref(CraRefHead *head);
```

引用一次对象（引用计数加1）

##### cra_ref_head_unref

```c
bool
cra_ref_head_unref(CraRefHead *head);
```

解引用一次对象（引用计数减1）  
引用计数归零时返回**true**，否则返回**false**  
因为`init`算是一次`ref`，所以`unref`应该且只应该比`ref`多调用一次。不然对象永远不会被释放。

#### 示例

```c
// ================== 定义 ==================

typedef struct MyStruct {
    // 其他字段...
    CraRefHead ref_head;
    cra_ref_fn free_ref;
    // 其他字段...
} MyStruct;

void
my_struct_init(MyStruct *my, cra_ref_fn free_fn, ...)
{
    cra_ref_head_init(&my->ref_head);
    my->free_ref = free_fn;
    // 初始化其他字段
    ...
}

// 一般会隐藏uninit函数， 以防外部绕过unref()直接调用
static void
my_struct_uninit(MyStruct *my)
{
    assert(my->ref_head.cnt == 0);
    // 释放其他字段
    ...
    if (my->free_ref)
        my->free_ref(my);
}

void
my_struct_ref(MyStruct *my)
{
    cra_ref_head_ref(&my->ref_head);
}

void
my_struct_unref(MyStruct *my)
{
    if (cra_ref_head_unref(&my->ref_head))
        my_struct_uninit(my);
}

// ================== 使用 ==================

MyStruct *my1, my2;

my1 = malloc(sizeof(MyStruct));
my_struct_init(my1, free);
my_struct_init(&my2, NULL);

my_struct_ref(my1);
my_struct_ref(&my2);

my_struct_unref(my1);
my_struct_unref(&my2);

my_struct_unref(my1);
my_struct_unref(&my2); 
```

见[test_ref_intrusive](../tests/test_refcnt.c)

### 非侵入式

通过将对象包装在**引用计数对象**中，即可实现非侵入式引用计数。

#### 接口函数

##### cra_ref_make

```c
CraRef *
cra_ref_make(size_t size, cra_ref_fn uninit);
```

创建一个非侵入式的引用计数对象。  
成功返回一个**引用计数对象**，否则返回**NULL**。  

- size: 对象大小
- uninit: 析构函数，可空

##### cra_ref_take

```c
CraRef *
cra_ref_take(void *ptr, cra_ref_fn uninit, cra_ref_fn dealloc);
```

创建一个非侵入式的引用计数对象。  
成功返回一个**引用计数对象**，否则返回**NULL**

- ptr: 对象
- uninit: 析构函数，可空
- dealloc: free函数, 可空

##### cra_ref_ref

```c
void
cra_ref_ref(CraRef *ref);
```

引用一次对象（引用计数加1）

##### cra_ref_unref

```c
bool
cra_ref_unref(CraRef *ref);
void
cra_ref_unref_clear(CraRef **ref);
```

[unref](#cra_ref_head_unref)  
解引用一次对象（引用计数减1）  
`unref`只在引用计数**归零**时返回**true**。  
引用计数**归零**时，`uninit`和`dealloc`会被调用以释放对象；  
引用计数对象也会被释放，传给`unref`的`ref`将变成**dangling pointer**。

##### cra_ref_get_ptr_uncheck

```c
void *
cra_ref_get_ptr_uncheck(CraRef *ref);
```

从**引用计数对象**中获取**实际对象**  
> 引用计数既不会增加，也不会减少  
> 函数内部不做任何检查，调用前请确保引用计数不为0

## 弱引用

```c
typedef struct CraWeakRefHead CraWeakRefHead, CraWeakRef;
```

### 侵 入 式

将`CraWeakRefHead`嵌入到结构体中，即可实现侵入式弱引用。

#### 工具 函数

##### cra_weak_ref_head_init

```c
void
cra_weak_ref_head_init(CraWeakRefHead *head);
```

初始化强引用计数为1，弱引用计数为1。

##### cra_weak_ref_head_ref

```c
void
cra_weak_ref_head_ref(CraWeakRefHead *head);
```

增加一次弱引用计数（弱引用计数加1）

##### cra_weak_ref_head_unref

```c
bool
cra_weak_ref_head_unref(CraWeakRefHead *head);
```

减少一次弱引用计数（弱引用计数减1）  
弱引用计数归零时返回**true**，否则返回**false**  
`ref`和`unref`必须成对出现，调用多少次`ref`就必须调用多少次`unref`。
> 弱引用计数归零后应该立即释放弱引用对象。

##### cra_weak_ref_head_acquire

```c
bool
cra_weak_ref_head_acquire(CraWeakRefHead *head);
```

尝试增加一次强引用计数。  
成功返回**true**，强引用计数+1；否则返回**false**

##### cra_weak_ref_head_release

```c
bool
cra_weak_ref_head_release(CraWeakRefHead *head);
```

减少一次强引用计数（强引用计数减1）。  
强引用计数归零，返回**true**，否则返回**false**。
> 强引用计数归零后应该立即释放对象（被保护的对象，而非弱引用对象），  
> 随后要调用一次`unref`以消除强引用隐含的一次弱引用。

#### 示 例

见[test_weak_ref_intrusive](../tests/test_refcnt.c)

### 非 侵入式

通过将对象包装在**弱引用对象**中，即可实现非侵入式弱引用。

#### 接口 函数

##### cra_weak_ref_make

```c
CraWeakRef *
cra_weak_ref_make(size_t size, cra_ref_fn uninit);
```

创建一个非侵入式的弱引用对象。  
成功返回一个**弱引用对象**，持有对对象的强引用；否则返回**NULL**

- size: 对象大小
- uninit: 析构函数，可空

##### cra_weak_ref_take

```c
CraWeakRef *
cra_weak_ref_take(void *ptr, cra_ref_fn uninit, cra_ref_fn dealloc);
```

创建一个非侵入式的弱引用对象。  
成功返回一个**弱引用对象**，持有对对象的强引用；否则返回**NULL**

- ptr: 对象
- uninit: 析构函数，可空
- dealloc: free函数, 可空

##### cra_weak_ref_ref

```c
void
cra_weak_ref_ref(CraWeakRef *ref);
```

增加对对象的弱引用（弱引用计数加1）

##### cra_weak_ref_unref

```c
bool
cra_weak_ref_unref(CraWeakRef *ref);
void
cra_weak_ref_unref_clear(CraWeakRef **ref);
```

[unref](#cra_weak_ref_head_unref)  
减少对对象的弱引用（弱引用计数减1）  
> 注意`ref`有可能变成**dangling pointer**，之后不要访问`ref`。

##### cra_weak_ref_acquire

```c
void *
cra_weak_ref_acquire(CraWeakRef *ref);
```

弱引用的持有者尝试获取对对象的强引用。  
成功返回实际对象，强引用计数+1；否则返回**NULL**。

##### cra_weak_ref_release

```c
void
cra_weak_ref_release(CraWeakRef *ref);
void
cra_weak_ref_release_clear(CraWeakRef **ref);
void
cra_weak_ref_release_clear_ptr(CraWeakRef *ref, void **pp);
void
cra_weak_ref_release_clear_both(CraWeakRef **ref, void **pp);
```

释放对对象的强引用。强引用计数-1。  
强引用计数归零时，对象将被释放，但弱引用对象不一定会被释放。  
`ref`有可能变成**dangling pointer**，之后不要访问`ref`。
> 调用`acquire`成功持有对对象的强引用者，之后必须调用`release`释放对对象的强引用。  
> 调用`make/take`创建弱引用对象者**持有**对对象的强引用，也必须调用`release`释放对对象的强引用。

##### cra_weak_ref_get_ptr_uncheck

```c
void *
cra_weak_ref_get_ptr_uncheck(CraWeakRef *ref);
```

获取**实际对象**  
> 强引用计数和弱引用计数两者都不会增加或减少  
> 函数内部不做任何检查，调用前请确保强引用计数不为0  
> 只有对对象持有强引用者才能调用此函数。
