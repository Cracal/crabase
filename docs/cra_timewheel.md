# CraTimewheel

多层时间轮

## init

```c
void
cra_timewheel_init(CraTimewheel *wheel, uint32_t tick_ms, uint32_t wheel_size);
```

初始化时间轮

- `tick_ms` 最小时间间隔
- `wheel_size` 每层时间轮的槽数

## uninit

```c
void
cra_timewheel_uninit(CraTimewheel *wheel);
```

反初始化

## add

```c
bool
cra_timewheel_add(CraTimewheel *wheel, CraTimer_base *timer);
```

添加定时器[timer](#cratimer_base)

## tick

```c
void
cra_timewheel_tick(CraTimewheel *wheel);
```

时间轮向前“滴答”一次

## get tick ms

```c
static inline uint32_t
cra_timewheel_get_tick_time(CraTimewheel *wheel);
```

获取最小时间间隔

## get wheel size

```c
static inline uint32_t
cra_timewheel_get_wheel_size(CraTimewheel *wheel);
```

获取时间轮槽数

## CraTimer_base

```c
CraTimer_base
{
    uint32_t          active : 1;
    uint32_t          repeat : 31;
    uint32_t          timeout_ms;
    cra_timer_base_fn on_timeout;
    cra_timer_base_fn on_remove_timer;
};
```

### timer init

```c
void
cra_timer_base_init(CraTimer_base    *base,
                    uint32_t          repeat,
                    uint32_t          timeout_ms,
                    cra_timer_base_fn on_timeout,
                    cra_timer_base_fn on_remove_timer);
```

初始化定时器

- `repeat` 定时器重复次数。传入**CRA_TIMER_INFINITE**让定时器一直重复
- `timeout_ms` 定时时间
- `on_timeout` 定时器到期时被调用的函数
- `on_remove_timer` 定时器被移出时间轮时被调用的函数

### timer is active

```c
static inline bool
cra_timer_base_is_active(CraTimer_base *base);
```

测试定时器是否活动

### timer set active

```c
static inline void
cra_timer_base_set_active(CraTimer_base *base);
```

激活定时器

### timer cancel timer

```c
static inline void
cra_timer_base_set_deactive(CraTimer_base *base);
#define cra_timer_base_cancel     cra_timer_base_set_deactive
#define cra_timer_base_cls_active cra_timer_base_set_deactive
```

取消定时器

### timer get repeat

```c
static inline uint32_t
cra_timer_base_get_repeat(CraTimer_base *base);
```

获取定时器重复次数

### timer set repeat

```c
static inline void
cra_timer_base_set_repeat(CraTimer_base *base, uint32_t repeat);
```

设置定时器重复次数

### timer get timeout

```c
static inline uint32_t
cra_timer_base_get_timeout(CraTimer_base *base);
```

获取定时器定时时间

### timer set timeout

```c
static inline void
cra_timer_base_set_timeout(CraTimer_base *base, uint32_t timeout_ms);
```

设置定时器定时时间
