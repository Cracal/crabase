# CraBuffer

buffer

## init

```c
void
cra_buffer_init(CraBuffer *buffer, size_t init_size);
```

将`buffer`初始化成大小为`init_size`的值  
`init_size`必须大于0

## uninit

```c
void
cra_buffer_uninit(CraBuffer *buffer);
```

反初始化

## get size

```c
#define cra_buffer_size(buffer)
```

获取`buffer`大小

## get readable

```c
#define cra_buffer_readable(buffer)
```

获取`buffer`可读区域大小

## get writable

```c
#define cra_buffer_writable(buffer)
```

获取`buffer`可写区域大小

## get read buffer

```c
#define cra_buffer_read_start(buffer)
```

获取`buffer`可读区域的起始地址

## get write buffer

```c
#define cra_buffer_write_start(buffer)
```

获取`buffer`可写区域的起始地址

## resize

```c
size_t
cra_buffer_resize(CraBuffer *buffer, size_t new_size);
```

重新设置`buffer`的大小，返回`buffer`最新的大小，这个大小可能不等于`new_size`  
`new_size`小于**readable**时，`buffer`的大小是**readable**而不是`new_size`  
`new_size`等于0时，`buffer`大小不变

## append

```c
void
cra_buffer_append(CraBuffer *buffer, const void *data, size_t len);
```

向`buffer`追加长度为`len`的数据（from `data`）

## append size

```c
size_t
cra_buffer_append_size(CraBuffer *buffer, size_t len);
```

通知`buffer`已追加长度为`len`的数据，`buffer`内部更新一下可写指针  
当用户使用**cra_buffer_write_start()**得到可写buffer并向其写入了数据，需要调用该函数更新可写指针  
当`len`大小于**writalbe**时，可写指针也只会更新到buffer最大长度（**writalbe**变为0）  
返回`min(len, WRITABLE)`

## retrieve

```c
size_t
cra_buffer_retrieve(CraBuffer *buffer, void *data, size_t len);
```

从`buffer`中读出长度为`len`的数据（to `data`）  
如果**readable**大于等于`len`，则返回`len`；否则返回**readable**

## retrieve size

```c
size_t
cra_buffer_retrieve_size(CraBuffer *buffer, size_t len);
```

通知`buffer`已读出长度为`len`的数据，`buffer`内部更新一下可读指针  
返回`min(len, READABLE)`

## retrieve all size

```c
static inline void
cra_buffer_retrieve_all_size(CraBuffer *buffer);
```

读出所有数据（清空`buffer`）
