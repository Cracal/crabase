# CraMalloc

对`malloc`、`calloc`、`realloc`和`free`的简易封装

```c
T *cra_alloc(T);
void *cra_malloc(size_t size);
void *cra_calloc(size_t num, size_t size);
void *cra_realloc(void *oldptr, size_t newsize);
void cra_free(void *ptr);
void cra_dealloc(void *obj);
```

`size`、`newsize`和`num`必须大于**0**  
`ptr`、`oldptr`和`obj`不能为**NULL**  
记得检查返回值是否为**NULL**

## memory leak detector

```c
void cra_memory_leak_report(void);
```

报告是否有内存泄漏  
> 只有定义了**CRA_MEMORY_LEAK_DETECTOR**宏才会报告内存是否泄漏  
> 这个宏只应该定义在**Debug**时期  
> 只有通过[CraMalloc](#cramalloc)系列函数申请和释放的内存会被记录。也就是说无法报告通过其他方式分配的内存是否泄漏

## 自定义内存分配函数

```c
void
cra_set_allocator(void *(*malloc_fn)(size_t),
                  void *(*calloc_fn)(size_t, size_t),
                  void *(*realloc_fn)(void *, size_t),
                  void  (*free_fn)(void *));
```

替换默认的内存分配函数。四个函数都必须提供
