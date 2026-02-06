# CraLog

日志

## 日志级别

- TRACE
- DEBUG
- INFO
- WARN
- ERROR
- FATAL

## 日志输出格式

```shell
时间 线程id 日志级别 日志名称 日志内容 - 文件名:行号
```

格式是固定不可配置的  
**日志名称**可以设置为模块名称  
**文件名:行号**是可选的。通过定义**CRA_LOG_FILE_LINE**宏控制

## 日志输出

```c
void cra_log_<level>_with_logname(logname, fmt, ...);
void cra_log_<level>(fmt, ...);
```

`<level>`是日志级别:
    - trace
    - debug
    - info
    - warn
    - error
    - fatal

没有`with_logname`后缀的在使用前需要先定义**CRA_LOG_NAME**宏  
`logname`是日志名称  

一般用法:

```c
// module.c
#include "cra_log.h"

#undef CRA_LOG_NAME
#define CRA_LOG_NAME "module-name"

...

void func(...)
{
    cra_log_debug("debug log");
    ...
    cra_log_fatal("error: %d.", error);
    ...
}

...

```

## get level

```c
CraLogLevel_e
cra_log_get_level(void);
```

获取日志级别

## set level

```c
void
cra_log_set_level(CraLogLevel_e level);
```

设置日志级别

## cleanup

```c
void
cra_log_cleanup(void);
```

关闭日志  
在程序退出前应该调用该函数

## startup

```c
void
cra_log_startup(CraLogLevel_e level, bool with_localtime, CraLogTo_i **logto);
```

开启并初始化日志  
在程序开始时应该调用该函数，否则无法使用日志  

- `level` 日志级别
- `with_localtime` 使用UTC时间还是本地时间
  - UTC - "2026-02-03T04:56:29.387Z"
  - local - "2026-02-03T12:56:38.301+08:00"
- `logto` 日志输出地接口（[CraLogTo_i](#cralogto_i)）

## CraLogTo_i

```c
CraLogTo_i
{
    void (*destroy)(CraLogTo_i **self);
    void (*append)(CraLogTo_i **self, const char *msg, int len, CraLogLevel_e level);
};
```

实现这个接口的一般方法如下：

```c
// MyLogTo

struct MyLogTo
{
    const CraLogTo_i *i;
    // my fields
};

static void mylogto_delete(CraLogTo_i **self)
{
    struct MyLogTo *obj = (struct MyLogTo*)self;
    // clean
    cra_dealloc(obj);
}

static void mylogto_append(CraLogTo_i **self, const char *msg, int len, CraLogLevel_e level)
{
    printf("%*.s", len, msg);
}

struct MyLogTo *mylogto_new(void)
{
    static const CraLogTo_i si = {
        .destroy = mylogto_delete,
        .append = mylogto_append,
    };

    struct MyLogTo *obj = cra_alloc(struct MyLogTo);
    obj->i = &si;
    // others
    return obj;
}

// ===============

// 使用时
cra_log_startup(CRA_LOG_LEVEL_INFO, true, (CraLogTo_i **)mylogto_new());

```

### 已实现的两个输出地

#### 输出到stdout

```c
CraLogToStdout *
cra_logto_stdout_create(bool async);
```

输出到标准输出。不同的日志级别会有不同的颜色  
`async`为**true**时会启动一个专门写日志的线程异步输出日志

#### 输出到文件

```c
CraLogToFile *
cra_logto_file_create(bool async, bool with_localtime, const char *path, const char *name, size_t max_file_size);
```

输出到文件中

- `async` 为**true**时会启动一个专门写日志的线程异步输出日志
- `with_localtime` 文件名时间部分使用UTC时间还是本地时间
  - UTC - "20260203T045629_384Z"
  - local - "20260203T045629_384"
- `path` 日志文件目录
- `name` 文件名（完整文件名的一部分）
- `max_file_size` 单个文件最大大小

完整的文件路径: "路径/文件名_时间.log"  
当文件大小超出`max_file_size`时会新建文件

## CraLogAsync

```c
CraLogAsync *
cra_log_async_create(cra_log_async_write2logto_fn on_write, CraLogTo_i **logto);
void
cra_log_async_destory(CraLogAsync **pobj);
void
cra_log_async_write(CraLogAsync *obj, const char *msg, int len);
void
cra_log_async_write_array(CraLogAsync *obj, const CraLogAsyncBuffer buffers[]);
```

当实现**CraLogTo_i**接口需要异步输出日志时可以考虑使用**CraLogAsync**
