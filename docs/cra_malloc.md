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
不用检查返回值是否为**NULL**，如果申请内存失败，函数内部会调用[malloc_failed_cb](#cra_set_malloc_failed_cb)退出程序

## memory leak detector

```c
void cra_memory_leak_report(void);
```

报告是否有内存泄漏  
> 只有定义了**CRA_MEMORY_LEAK_DETECTOR**宏才会报告内存是否泄漏  
> 这个宏只应该在**Debug**时期定义  
> 只有通过[CraMalloc](#cramalloc)系列函数申请和释放的内存会被记录。也就是说无法报告通过其他方法分配的内存是否泄漏

## cra_set_malloc_failed_cb

```c
void
cra_set_malloc_failed_cb(void (*cb)(const char *fname, size_t size));
```

设置申请内存失败时的回调函数。默认行为是输出一条报错信息，然后终止程序
