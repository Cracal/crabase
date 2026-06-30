# CraLog

日志

## 日志级别

- TRACE
- DEBUG
- INFO
- WARN
- ERROR
- FATAL

## 日志文件滚动策略

1. 按文件大小滚动策略
    - 当日志文件大小超过指定大小([max_file_size](#config-log-file))时，会创建一个新的日志文件。
2. 按日滚动策略
    - 当日期改变了并且有新的日志要输出时，会创建一个新的日志文件。

## 日志文件名格式

```shell
日志名_yyyyMMdd_hhmmss_SSS[Z].log
```

## 日志输出格式

```shell
时间 日志级别 线程id 日志内容 -- 文件名:行号
```

格式是固定不可配置的  
**文件名:行号**是可选的。通过**CRA_LOG_FILE_LINE**宏控制

## 日志输出

```c
void
cra_log_<level>(CraLogger *logger, const char *fmt, ...);
```

`<level>`:

- trace
- debug
- info
- warn
- error
- fatal

使用:

```c
// xxx.c
#include "cra_log.h"

CraLogger *logger = NULL;

...

void func(...)
{
    cra_log_debug(logger, "debug log");
    ...
    cra_log_fatal(logger, "error: %d.", error);
    ...
}

...

int main(void)
{
    logger = cra_log_open("log-name", CRA_LOG_LV_INFO, true, true);
    ...
    cra_log_close(logger);
    logger = NULL;
    ...
    return 0;
}

```

## level to string

```c
const char *
cra_log_level_to_str(CraLogLv_e level);
```

将日志级别转换为对应的字符串

## get level

```c
CraLogLv_e
cra_log_get_level(CraLogger *logger);
```

获取日志级别

## set level

```c
void
cra_log_set_level(CraLogger *logger, CraLogLv_e level);
```

设置日志级别

## get log name

```c
const char *
cra_log_get_name(CraLogger *logger);
```

获取日志名称

## config log file

```c
void
cra_log_config(CraLogger *logger, unsigned int max_file_size, const char *log_dir);
```

配置日志文件(仅日志输出到文件时生效)  
在打开日志记录器后调用该函数配置日志文件。  
如果未调用该函数，系统会使用默认配置。

- `max_file_size` 最大文件大小（字节）
- `log_dir` 日志输出目录

## open

```c
CraLogger *
cra_log_open(const char *name, CraLogLv_e level, bool use_zulu, bool output_to_file);
```

打开一个日志记录器  
无需检查函数返回值，直接使用即可

- `name` 日志名称
- `level` 日志级别
- `use_zulu`
  - true:  "2026-02-03T04:56:29.387Z"
  - false: "2026-02-03T12:56:38.301+08:00"
- `output_to_file`
  - true: 异步输出到文件
  - false: 同步输出到控制台

当`output_to_file`为true时，日志系统会创建且只创建一个后台线程，用于异步输出日志到文件。  
当`output_to_file`为false时，日志系统会直接在调用日志函数的线程中输出日志。

## close

```c
void
cra_log_close(CraLogger *logger);
```

关闭日志记录器  
在程序退出前应该调用该函数
