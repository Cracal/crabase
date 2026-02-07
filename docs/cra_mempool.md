# CraMemPool

这是一个简单的内存池（对象池）

## init

```c
void
cra_mempool_init(CraMemPool *pool, size_t item_size, size_t items_per_block, size_t init_block);
```

初始化

- `item_size` 对象大小
- `items_per_block` 池中每块内存可以存多少个对象
- `init_block` 初始内存块数量

## uninit

```c
void
cra_mempool_uninit(CraMemPool *pool);
void
cra_mempool_uninit_no_check(CraMemPool *pool);
```

反初始化  
如果没有`dealloc`所有`alloc`出去的对象，那么：

1. `uninit`会报错并退出程序
2. `uninit_no_check`依旧执行销毁内存池操作，使得所有指向未`dealloc`的对象的指针全部变成**悬挂指针**

## alloc

```c
void *
cra_mempool_alloc(CraMemPool *pool);
```

获取一个空闲对象

## dealloc

```c
void
cra_mempool_dealloc(CraMemPool *pool, void *ptr);
```

归还一个对象
