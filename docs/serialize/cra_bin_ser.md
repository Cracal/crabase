# CraBinSer

将对象序列化成二进制数据  
配合[测试代码](../../tests/test_bin_ser.c)能更好理解

## 二进制数据格式

```shell
null:         [T:4,P:4]             -- 1 byte
bool:         [T:4,V:4]             -- 1 byte
[u]int8:      [T:4,S:4][V:8]        -- 1 + 1 bytes
[u]int16:     [T:4,S:4][V:16]       -- 1 + 2 bytes
[u]int32:     [T:4,S:4][V:32]       -- 1 + 4 bytes
[u]int64:     [T:4,S:4][V:64]       -- 1 + 8 bytes
var[u]int:    [T:4,S:4][V:N]        -- 1 + (1 ~ 10) bytes
float:        [T:4,S:4][V:32]       -- 1 + 4 bytes
double:       [T:4,S:4][V:64]       -- 1 + 8 bytes
string:       [T:4,P:4][L:N][V]     -- 1 + (1 ~ 10) + N bytes
bytes:        [T:4,P:4][L:N][V]     -- 1 + (1 ~ 10) + N bytes
struct:       [T:4,P:4][C:1]{IM}    -- 1 + 1 + N bytes
list/array:   [T:4,P:4][C:N]<E>     -- 1 + (1 ~ 10) + N bytes
dict:         [T:4,P:4][C:N]<KV>    -- 1 + (1 ~ 10) + N bytes

T: 类型
V: 值
S: 数据大小
L: 值长度
C: 元素个数/成员个数
M: 结构体成员
K: 字典的key
P: 可忽略的填充
E: 元素
I: 结构体成员的ID
```

## 序列化

```c
bool
cra_bin_serialize_err(unsigned char *buf, size_t *len, CraSeriObject *obj, CraSerErr *err);
static inline bool
cra_bin_serialize(unsigned char *buf, size_t *len, CraSeriObject *obj);
```

- `buf` 缓冲区
- `len` 缓冲区长度。要传入`buf`的长度，函数会返回实际写入的数据长度
- `obj` 要序列化的对象。见[CraSeriObject](./cra_serialize.md#craseriobject)
- `err` 错误信息。可传入**NULL**

## 反序列化

```c
bool
cra_bin_deserialize_err(unsigned char *buf, size_t len, CraSeriObject *retobj, CraSerErr *err);
static inline bool
cra_bin_deserialize(unsigned char *buf, size_t len, CraSeriObject *retobj);
```

- `buf` 要反序列化的二进制数据
- `len` `buf`的长度
- `retobj` 反序列化出来的对象。见[CraSeriObject](./cra_serialize.md#craseriobject)
- `err` 错误信息。可传入**NULL**

## write/read length

```c
void
cra_bin_write_len(unsigned char *buf, uint64_t len, size_t len_size);
uint64_t
cra_bin_read_len(unsigned char *buf, size_t len_size);
```

`write`: 向`buf`写入`长度`——`len`  
`read`:  从`buf`中读取数字`长度`  
`len_size`描述`长度`是**uint8_t**、**uint16_t**、**uint32_t**还是**uint64_t**  
要保证`buf`足够容纳`len_size`字节

## 例子

```c
struct resp
{
    int32_t  code;
    uint32_t userid;
    char    *message;
};
CRA_TYPE_META_BEGIN(meta_resp)
CRA_TYPE_META_MEMBER_INT(struct resp, code, 1)
CRA_TYPE_META_MEMBER_UINT(struct resp, userid, 2)
CRA_TYPE_META_MEMBER_STRING(struct resp, message, 3, true)
CRA_TYPE_META_END();

// 序列化
size_t        length;
unsigned char buffer[1024];
struct resp   r = { 0, 1000, "OK" };
length = sizeof(buffer) - sizeof(uint16_t);
cra_bin_serialize(buffer + sizeof(uint16_t), &length, CRA_SERI_STRUCT(r, false, meta_resp, NULL, NULL));
assert(length < UINT16_MAX);
cra_bin_write_len(buffer, length, sizeof(uint16_t));

// 反序列化
size_t         length2;
unsigned char *buffer2;
struct resp   *r2;

buffer2 = buffer; // ...
length2 = cra_bin_read_len(buffer2, sizeof(uint16_t));
cra_bin_deserialize(buffer2 + sizeof(uint16_t), length2, CRA_SERI_STRUCT(r2, true, meta_resp, NULL, NULL));
cra_free(r2->message);
cra_free(r2);
```
