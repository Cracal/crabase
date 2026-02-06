# CraAssert

断言。该断言会取代**assert.h**中的断言

## assert

```c
assert(condition)
```

只在Debug下生效

## assert_always

```c
assert_always(condition)
```

一直生效，不管有没有定义**NDEBUG**宏

## cra_assert_set_func

```c
void
cra_assert_set_func(void (*func)(const char *condition, const char *fname, const char *file, int line));
```

设置断言失败时调用的函数，**func**不能为'NULL'
