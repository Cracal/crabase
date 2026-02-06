# 时间

## monotonic sec

```c
double
cra_monotonic_sec(void);
unsigned long
cra_tick_ms(void);
unsigned long long
cra_tick_us(void);
```

获取时间戳

## gmtime

```c
static inline void
cra_gmtime(time_t secs, struct tm *_tm);
```

将`secs`转成GMT时间

## localtime

```c
static inline void
cra_localtime(time_t secs, struct tm *_tm);
```

将`secs`转成本地时间

## print tm

```c
void
cra_print_tm(const struct tm *const _tm);
```

打印tm时间

## datetime now UTC

```c
void
cra_datetime_now_utc(CraDateTime *dt);
```

获取当前UTC时间

## datetime now locatime

```c
void
cra_datetime_now_localtime(CraDateTime *dt);
```

获取当前本地时间

## print datetime

```c
void
cra_print_datetime(const CraDateTime *const dt);
```

打印DateTime时间
