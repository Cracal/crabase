# CraBuffer

Buffer

## init

```c
bool
cra_buffer_init(CraBuffer *buffer, unsigned int init_size, unsigned int head_size);
```

初始化Buffer  
成功返回**true**，失败返回**false**

- init_size: 初始化Buffer大小。必须大于0
- head_size: 在数据前预留的长度。可以为0

## uninit

```c
void
cra_buffer_uninit(CraBuffer *buffer);
```

反初始化

## write head

```c
bool
cra_buffer_write_head(CraBuffer *buffer, const void *head);
```

向预留的头部写入数据。写入的数据长度是调用`init`时指定的**head_size**。  
只有**head_size**大于0时才会写入并返回**true**；否则返回**false**

## append

```c
bool
cra_buffer_append(CraBuffer *buffer, const void *data, unsigned int len);
```

向Buffer追加数据。  
成功返回**true**，失败返回**false**

## retrieve

```c
unsigned int
cra_buffer_retrieve(CraBuffer *buffer, void *data, unsigned int len);
```

从Buffer中取出**len**长度的数据到**data**中。  
返回实现取出的数据长度。

## append size

```c
void
cra_buffer_append_size(CraBuffer *buffer, unsigned int len);
```

通知Buffer追加了**len**长度的数据。

## retrieve size

```c
unsigned int
cra_buffer_retrieve_size(CraBuffer *buffer, unsigned int len);
```

丢弃Buffer中前**len**字节长度的数据。  
返回实际丢弃的数据长度。

## get size

```c
unsigned int
cra_buffer_get_size(CraBuffer *buffer);
```

获取Buffer的大小。

## get head size

```c
unsigned int
cra_buffer_get_head_size(CraBuffer *buffer);
```

获取在`init`中指定的**head_size**

## get readable size

```c
unsigned int
cra_buffer_get_readable_size(CraBuffer *buffer);
```

获取Buffer当前可读数据大小。

## get readable size with head

```c
unsigned int
cra_buffer_get_readable_size_with_head(CraBuffer *buffer);
```

返回[cra_buffer_get_readable_size\(\)](#get-readable-size) + [cra_buffer_get_head_size\(\)](#get-head-size)

## get read start with head

```c
const void *
cra_buffer_get_read_start_with_head(CraBuffer *buffer);
```

返回[cra_buffer_get_read_start\(\)](#get-read-start) - [cra_buffer_get_head_size\(\)](#get-head-size)

## get read start

```c
void *
cra_buffer_get_read_start(CraBuffer *buffer);
```

返回Buffer当前可读数据的起始地址。

## get write start

```c
void *
cra_buffer_get_write_start(CraBuffer *buffer);
```

返回Buffer当前可写数据的起始地址。

## reset

```c
void
cra_buffer_reset(CraBuffer *buffer);
```

重置Buffer，将可读可写指针都重置为0。
