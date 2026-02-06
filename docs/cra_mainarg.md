# CraMainArg

这是一个处理`main`函数参数（命令行参数）的工具

## init

```c
void
cra_mainarg_init(CraMainArg *ma, char *program, char *intro, char *usage, CraMainArgElement options[]);
```

初始化并设置程序用到的参数

- `program` 程序名。函数内部可能会修改该字符串，如果该字符串在其他地方也有使用，请拷贝一份再传入
- `intro` 程序的简单描述
- `usage` 使用方法简要
  - `usage` = "[options] [others]", 输出："Usage: 程序名 [options] [others]"
- `options` 程序支持的选项。详见[CraMainArgElement](#cramainargelement)

## uninit

```c
void
cra_mainarg_uninit(CraMainArg *ma);
```

反初始化

## parse args

```c
void
cra_mainarg_parse_args(CraMainArg *ma, int argc, char *argv[]);
```

解析`main`参数。如果用户输入的选项（参数）不合法，函数会报错并退出程序

## get val

```c
bool
cra_mainarg_get_b(CraMainArg *ma, char *option, bool default_val);
int64_t
cra_mainarg_get_i(CraMainArg *ma, char *option, int64_t default_val);
double
cra_mainarg_get_f(CraMainArg *ma, char *option, double default_val);
char *
cra_mainarg_get_s(CraMainArg *ma, char *option, char *default_val);
```

获取选项的值  

- `option` 选项。可以省略选项前的短杠（减号）
- `default_val` 缺省值

一个没有值的选项（[CRA_MAINARG_ELEMENT_BOL](#工具宏)）可以调用`cra_mainarg_get_b`传入缺省值**false**来判断：

```c
if (cra_mainarg_get_b(&ma, "o", false))
    prinf("用户输入了'-o'\n");
else
    prinf("用户没有输入'-o'\n");
```

## get positional arguments

```c
int
cra_mainarg_get_pos_args_count(CraMainArg *ma);

bool
cra_mainarg_get_pos_args_b(CraMainArg *ma, int index, bool default_val, cra_mainarg_fn func, void *arg);
int64_t
cra_mainarg_get_pos_args_i(CraMainArg *ma, int index, int64_t default_val, cra_mainarg_fn func, void *arg);
double
cra_mainarg_get_pos_args_f(CraMainArg *ma, int index, double default_val, cra_mainarg_fn func, void *arg);
char *
cra_mainarg_get_pos_args_s(CraMainArg *ma, int index, char *default_val, cra_mainarg_fn func, void *arg);
```

用户输入的命令行参数中所有没有被[options](#init)匹配的参数都会被收录到位置参数（Positional Arguments）中  
`func`和`arg`在[工具宏](#工具宏)有解释

## CraMainArgElement

```c
CraMainArgElement
{
    char          *op;
    char          *option;
    char          *valtip;
    char          *optip;
    cra_mainarg_fn func;
    void          *arg;
};
```

构造一个`CraMainArgElement`数组来配置程序支持的选项  

### 工具宏

```c
#define CRA_MAINARG_ELEMENT_BEGIN(name)
#define CRA_MAINARG_ELEMENT_END()
#define CRA_MAINARG_ELEMENT_BOL(op, option, optip)
#define CRA_MAINARG_ELEMENT_VAL(op, option, valtip, optip, func, arg)
```

使用`BEGIN`和`END`来定义一个选项数组  
使用`BOL`来配置一个没有值的选项  
使用`VAL`来配置一个需要值的选项

- `op` 短选项。"\-O"。"\-"不可省略
- `option` 长选项。"\-\-OPTION"。"\-\-"不可省略
- `optip` 选项对应的提示。不可为空
- `fucn` 处理选项的值的函数。不可为空。详见[处理选项值的函数](#处理选项值的函数)
- `arg` 传给`func`的参数

> 不需要配置'-h, --help'，内部会自己生成

## 处理选项值的函数

```c
typedef bool (*cra_mainarg_fn)(CraMainArgVal_u *retval, const char *opval, void *arg);
```

函数会在[cra_mainarg_parse_args](#parse-args)中被调用  

- `retval` 处理后的值
- `opval` 要处理的值
- `arg` 其他

成功返回**true**，失败返回**false**  
如果返回**false**，cra_mainarg_parse_args会调用exit()退出程序

### 转换值

```c
bool
cra_mainarg_stob(CraMainArgVal_u *retval, const char *opval, void *_);
bool
cra_mainarg_stoi(CraMainArgVal_u *retval, const char *opval, void *_);
bool
cra_mainarg_stof(CraMainArgVal_u *retval, const char *opval, void *_);
bool
cra_mainarg_stos(CraMainArgVal_u *retval, const char *opval, void *_);
```

将`opval`转换成其他类型  
`stob`: string => bool  
`stoi`: string => int64  
`stof`: string => double  
`stos`: string => string  
`_`表示这个参数只传入NULL。[VAL](#工具宏)的`arg`必须是**NULL**  
`stob`的`opval`只能是**on**和**off**，**on**表示**true**，**off**表示**false**

### 转换并检查值

```c
bool
cra_mainarg_stob_values(CraMainArgVal_u *retval, const char *opval, void *values);

values = ["true_string", "false_string"]
check (opval in values)
```

```c
bool
cra_mainarg_stoi_in_range(CraMainArgVal_u *retval, const char *opval, void *range);

range = [min: int64, max: int64]
check (int64(opval) >= min && int64(opval) < max)
```

```c
bool
cra_mainarg_stof_in_range(CraMainArgVal_u *retval, const char *opval, void *range);

range = [min: double, max: double]
check (double(opval) >= min && double(opval) < max)
```

```c
bool
cra_mainarg_stos_in_array(CraMainArgVal_u *retval, const char *opval, void *array);

array = ["string1", ..., "stringN"]
check (opval in array)

// c
char *array[] = {"string1", ..., "stringN", NULL};
```

## 一般用法

```c
// main.c
#include "cra_mainarg.h"

int main(int argc, char *argv[])
{
    CraMainArg ma;

    // 配置

    CRA_MAINARG_ELEMENT_BEGIN(options)
    CRA_MAINARG_ELEMENT_BOL("-n", NULL, "Call a function")
    CRA_MAINARG_ELEMENT_VAL(NULL, "--bool", "<yes|no>", "Set a BOOL value", cra_mainarg_stob_values, ((char *[]){"yes", "no"}))
    CRA_MAINARG_ELEMENT_VAL("-i", "--int", "<integer>", "Set an INT value. range: [1, 10]", cra_mainarg_stoi_in_range, ((int64_t[]){1, 11}))
    CRA_MAINARG_ELEMENT_VAL("-f", "--float", "<float>", "Set a FLOAT value", cra_mainarg_stof, NULL)
    CRA_MAINARG_ELEMENT_VAL("-s", "--string", "<string>", "Set a STRING value", cra_mainarg_stos_in_array, ((char *[]){"str1", "str2", "str3", NULL}))
    CRA_MAINARG_ELEMENT_END();

    cra_mainarg_init(&ma, argv[0], "This is a test program", "[options] [host] [port]", options);

    // 解析
    cra_mainarg_parse_args(&ma, argc, argv);

    // 使用

    if (cra_mainarg_get_b(&ma, "n", false)) printf("Called\n");
    bool b = cra_mainarg_get_b(&ma, "bool", false);
    int64_t i = cra_mainarg_get_i(&ma, "i", 5);
    double d = cra_mainarg_get_f(&ma, "-f", 0.0);
    char *s = cra_mainarg_get_s(&ma, "--string", "null");
    printf("b: %s, i: %zd, d: %lf, s: %s\n", b ? "true" : "false", i, d, s);

    if (cra_mainarg_get_pos_args_count(&ma) == 2)
    {
        char *host = cra_mainarg_get_pos_args_s(&ma, 0, "0.0.0.0", cra_mainarg_stos, NULL);
        int port = (int)cra_mainarg_get_pos_args_i(&ma, 1, 8080, cra_mainarg_stoi_in_range, ((int64_t[]){1025, 65536}));
        printf("host: %s, port: %d\n", host, port);
    }

    // 清理
    cra_mainarg_uninit(&ma);
}
```
