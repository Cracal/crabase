# CraMemPool

这是一个简单的内存池（对象池）

## init

```c
void
cra_mempool_init(CraMemPool *pool, size_t itemsize, unsigned int count);
```

初始化

- `itemsize` 对象（内存块）大小
- `count` 对象（内存块）数量

## uninit

```c
void
cra_mempool_uninit(CraMemPool *pool);
```

反初始化

## alloc

```c
void *
cra_mempool_alloc(CraMemPool *pool);
```

获取一个空闲内存块。如果没有，则新建一个

## dealloc

```c
void
cra_mempool_dealloc(CraMemPool *pool, void *ptr);
```

归还一个内存块  
如果`ptr`不是[init](#init)分配的，是[alloc](#alloc)新建的，那么`dealloc`直接销毁它
