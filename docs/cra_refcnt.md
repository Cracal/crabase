# CraRefcnt

引用计数

## init

```c
static inline void
cra_refcnt_init(CraRefcnt *ref, cra_refcnt_release_fn func);
```

初始化，计数为1

- `func` 计数归零时的回调函数

## ref

```c
static inline void
cra_refcnt_ref(CraRefcnt *ref);
```

引用一次（使计数+1）

## unref

```c
static inline bool
cra_refcnt_unref(CraRefcnt *ref);
static inline void
cra_refcnt_unref0(CraRefcnt *ref);
```

解引用一次（使计数-1）  
当计数变0时就会调用[init](#init)设置的回调函数

## 内嵌使用

将Refcnt当成对象的成员

```c
struct MyObject
{
    int field1;
    ...
    CraRefcnt ref;
    ...
    int fieldN;
};



static void
release_myobject(CraRefcnt *ref)
{
    struct MyObject *obj = container_of(ref, struct MyObject, ref);
    cra_dealloc(obj);
}

struct MyObject *obj = cra_alloc(struct MyObject);

// init
cra_refcnt_init(&obj->ref, release_myobject);

// ref
cra_refcnt_ref(&obj->ref);
...
// unref
cra_refcnt_unref(&obj->ref);
// unref
cra_refcnt_unref(&obj->ref);
```

## 临时使用

```c
#define CRA_REFCNT_NAME_DEF(Type, name)
    => struct name { Refcnt rc; Type o; }
#define CRA_REFCNT_DEF(Type)
    => struct { Refcnt rc; Type o; }

#define CRA_REFCNT_PTR_NAME_DEF(Type, name)
    => struct name { Refcnt rc; Type *p; }
#define CRA_REFCNT_PTR_DEF(Type)
    => struct { Refcnt rc; Type *p; }

#define CRA_REFCNT_RC(rc)  (&rc)->rc
#define CRA_REFCNT_OBJ(rc) (&rc)->o
#define CRA_REFCNT_PTR(rc) (rc)->p
```

当一个结构体在定义时没有Refcnt字段时，可以通过这几个宏来使用引用计数

```c
struct A
{
    int   i;
    float f;
};

void
release_as(CraRefcnt *ref)
{
    CRA_REFCNT_DEF(struct A) *a = (void *)container_of(ref, CRA_REFCNT_DEF(struct A), rc);
    cra_free(a);
}

void
release_ap(CraRefcnt *ref)
{
    CRA_REFCNT_PTR_DEF(struct A) *a = (void *)container_of(ref, CRA_REFCNT_PTR_DEF(struct A), rc);
    cra_free(CRA_REFCNT_PTR(a));
    cra_free(a);
}

int
main(void)
{
    // def
    CRA_REFCNT_DEF(struct A) *a1 = cra_malloc(sizeof(CRA_REFCNT_DEF(struct A)));
    CRA_REFCNT_PTR_DEF(struct A) *a2 = cra_malloc(sizeof(CRA_REFCNT_PTR_DEF(struct A)));
    CRA_REFCNT_PTR(a2) = cra_alloc(struct A);

    // init
    CRA_REFCNT_OBJ(a1)->i = 100;
    CRA_REFCNT_OBJ(a1)->f = 1.5f;
    cra_refcnt_init(CRA_REFCNT_RC(a1), release_as);
    CRA_REFCNT_PTR(a2)->i = 200;
    CRA_REFCNT_PTR(a2)->f = 4.5f;
    cra_refcnt_init(CRA_REFCNT_RC(a2), release_ap);

    // ref
    cra_refcnt_ref(CRA_REFCNT_RC(a1));
    cra_refcnt_ref(CRA_REFCNT_RC(a2));

    // unref
    cra_refcnt_unref(CRA_REFCNT_RC(a1));
    cra_refcnt_unref(CRA_REFCNT_RC(a1));
    cra_refcnt_unref(CRA_REFCNT_RC(a2));
    cra_refcnt_unref(CRA_REFCNT_RC(a2));
}
```
