# JSON

将对象序列化成JSON
配合[测试代码](../../tests/test_json.c)能更好理解

## 序列化

```c
bool
cra_json_stringify_err(char *buf, size_t *len, bool format, CraSeriObject *obj, CraSerErr *err);
static inline bool
cra_json_stringify(char *buf, size_t *len, bool format, CraSeriObject *obj);
```

- `buf` 缓冲区
- `len` 缓冲区长度。要传入`buf`的长度，函数会返回实际写入的数据长度
- `format` 是否格式化JSON字符串
- `obj` 要序列化的对象。见[CraSeriObject](./cra_serialize.md#craseriobject)
- `err` 错误信息。可传入**NULL**

## 反序列化

```c
bool
cra_json_parse_err(char *buf, size_t len, CraSeriObject *retobj, CraSerErr *err);
static inline bool
cra_json_parse(char *buf, size_t len, CraSeriObject *retobj);
```

- `buf` JSON字符串
- `len` 字符串长度
- `obj` 要序列化的对象。见[CraSeriObject](./cra_serialize.md#craseriobject)
- `err` 错误信息。可传入**NULL**

## key length

JSON的key长度限制为**CRA_MAX_JSON_KEY_LENGTH**

## safe int

在编译时如果定义了宏**CRA_JSON_SAFE_INT**，那么序列化的整数被限制在区间`[CRA_MIN_SAFE_INT, CRA_MAX_SAFE_INT]`内

## 不支持类型

不支持**CRA_TYPE_BYTES**类型

## 例子

直接看[测试代码](../../tests/test_json.c)吧
