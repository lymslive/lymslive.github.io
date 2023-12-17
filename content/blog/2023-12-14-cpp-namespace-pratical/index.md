+++
title = "C++ 命名空间理解与践行"
draft = true
[taxonomies]
categories = ["程序设计"]
tags = ["C++"]
+++

众所周知，C++ 算是 C 的超集，提供了非常丰富的高级特性。对一般 C++ 开发者而言，
并不要求精通 C++ 的每个特性，而应该按需学习与强化。在众多特性中，也许命名空间
是很不起眼，平平无奇那类。但假如做个思想实验，摒弃 C++ 绝大部分特性，只保留一
个特性加到 C 语言中，哪个最有助于改善 C 语言项目的开发体验与维护体验？

我觉得不是类（虽然 C with class 也是 C++ 的一种实用范式），也不是模板，或其他
奇淫巧技，而是命名空间。就因为 C 语言中几乎只有一个全局的命名空间，所有函数、
结构体等类型，以及可能必要的全局变量，都塞在同一个空间，对维护大型项目而言
太拥挤太可怕了，很容易出现命名冲突。C++ 的命名空间（namespace）就是为解决这个
问题设计的，后现代大多数语言都有的模块（module）或包（package）的一个作用也是
解决命名冲突，但更主要的作用是从项目整体宏观上抽象出模块的分组与组织。
<!-- more -->

## 命名前缀与命名空间

在纯 C 中，为了保持语言特性的简单，对该问题的妥协策略或推荐实践是在抽象出模块
后为每个函数（及结构体）的命名添加统一的代表模块的前缀，例如：

```c
mymod_method();
mymod_another_method();
```

而在 C++ 中利用命名空间的对应用法大约像这样：

```c++
mymod::method();
mymod::another_method();
```

初看起来，从客户代码使用方角度，使用命名空间似乎比统一前缀没有显著的不同，不外
是将一个下划线 `_` 替换为两个冒号 `::`，总长度写起来还多了一个字符。其他一些流
行语言也有只用一个点 `.` 来分隔模块名的，但这可能易与类成员引用（视觉）混淆，
各有利弊吧，不过是具体语言设计的偏好选择。

然而，命名空间与命名前缀是有本质不同的，设计思想上的不同。相同的前缀只是个表象，
并且命名空间的前缀在具体使用中有可能被简化或省略。

## C++ 命名空间基本概念

在 C++ 可以用 `namespace` 打开一个命名空间，其后接一个空间名，然后是一对大括号，
大括号内可以是其他任何合法 C++ 代码，包括嵌套的命名空间。不同命名空间里面的标
识符不会产生名字冲突，也不会与全局空间（不在任何命令空间里）的标识符产生冲突。
例如：

```c++
// sample1.cpp
#include <stdio.h>

namespace math
{
    int value_ = 1;
    int GetValue() { return value_; }
    void SetValue(int value) { value_ = value; }
}

namespace physic
{
    int value_ = 2;
    int GetValue() { return value_; }
    void SetValue(int value) { value_ = value; }
}

int value_ = 0;

int main (int argc, char* argv[])
{
    math::SetValue(111);
    physic::SetValue(222);
    int value_ = 333;
    printf("value in math: %d\n", math::GetValue());
    printf("value in physic: %d\n", physic::GetValue());
    printf("value in local: %d\n", value_);
    printf("value in global: %d\n", ::value_);
    return 0;
}
```

编译该示例，运行，得如下输出：

```
value in math: 111
value in physic: 222
value in local: 333
value in global: 0
```

几点说明：

* 命名空间的名字本身也需要一个合法的标识符，不能与它所在的命名空间内的其他标识
  符重名。
* 命名空间的大括号后面不需要有分号，当然有分号也不会错，只当是空语句。这与类定
  义后面必须有分号不同。`namespace` 后面的 `{}` 可认为与 `if` 或 `for` 类似，
  代表一个词法范围，并不是像 `class` 定义什么实体。
* 命名空间限定符 `::` 前面一般是某个命名空间的名字，而全局空间没有名字，所以直
  接以 `::` 开头。但在不会出现名字隐藏的情况下，全局空间的 `::` 前缀可省略。
* `main` 函数不能放在命名空间中。或者说在默认情况下，C 程序的入口是全局空间的
  那个 `main` 函数，当然也可以从中调用其他自定义命名空间内的 `main` 函数。

### 命名空间隐藏实现与多次打开

命名空间没有权限控制，只要在词法范围可见，都能访问。词法范围通俗理解就是当编译
器顺序扫描源文件（编译单元）时，一对对大括号分隔的范围。在上例中我们特意只使用
`math` 与 `physics` 命名空间内的函数，而没使用它定义的 `value_` 变量，只是君子
约定，自立的规范，实际上编译器不能阻止我们直接访问其变量。

但是我们可以按常用的 C/C++ 技法，将声名与实现分离在头文件与源文件中，从而达到
隐藏实现的目的。例如先将 `math` 与 `physic` 两个命名空间的代码单独放在一个源文
件中，不妨名之为 `science.cpp`：

```c++
// science.cpp
namespace math
{
    int value_ = 1;
    int GetValue() { return value_; }
    void SetValue(int value) { value_ = value; }
}

namespace physic
{
    int value_ = 2;
    int GetValue() { return value_; }
    void SetValue(int value) { value_ = value; }
}
```

然后在包含 `main` 的主源文件中，删除对变量 `value_` 的定义，并将函数的实现去掉，
只留下声明：

```c++
// sample2.cpp
#include <stdio.h>

namespace math
{
    int GetValue();
    void SetValue(int value);
}

namespace physic
{
    int GetValue();
    void SetValue(int value);
}

int value_ = 0;

int main (int argc, char* argv[])
{
    math::SetValue(111);
    physic::SetValue(222);
    int value_ = 333;
    printf("value in math: %d\n", math::GetValue());
    printf("value in physic: %d\n", physic::GetValue());
    printf("value in local: %d\n", value_);
    printf("value in global: %d\n", ::value_);
    return 0;
}
```

这样，就能保证在主文件 `sample2.cpp` 中不能访问 `math::value_` 了。编译时要将
两个文件一起编译链接，如 `g++ sample2.cpp science.cpp` 。

在实践中，`sample2.cpp` 文件前半部分有包含声明语句的两个命名空间部分，应该单独
写在一个头文件中，比如 `science.h` ，然后可以多处 `include` 它，包括在
`science.cpp` 的最开始，按习惯也该 `include` 它，虽然在本例中并无必要。

分离实现后，如果实在想在其他文件访问 `science.cpp` 的内部变量 `math:value_` ，
则可以在 `science.h` 中相应的命名空间中添加 `extern` 声明，如：

```c++
namespace math
{
    extern int value_;
}
```

虽然 `extern` 变量并不是良好的项目实践，但放在各个命名空间内的 `extern` 变量总
比在 C 语言中全部是全局的 `extern` 变量要好管理得多，更有种被封装的安全感。

如此，在编译 `science.cpp` 时，展开 `science.h` 后，编译器看到的内容会出现多次
用 `namespace` 关键字引导相同的命名空间，如：

```c++
// 展开 science.h
namespace math
{
    extern int value_;
}
namespace math
{
    int GetValue();
    void SetValue(int value);
}
// ...
// 展开 science.h 结束
namespace math
{
    int value_ = 1;
    int GetValue() { return value_; }
    void SetValue(int value) { value_ = value; }
}
// ...
```

这是合法的。因为 `namespace` 关键字的意思它不是定义或创建命名空间，它只是打开
命名空间，所以可以在同一个源文件（编译单元）中多次打开与关闭某个命名空间。
只要保证在同一个命名空间中不会对同一个实体（变量，函数，类等）进行重复定义，但
可以重复声明。也无需在某处预先创建命名空间，当然也可以只跟个空大括号假装在创建
命名空间，统一列在一起或许对项目管理有清晰规范之意味。如：

```c++
// 本项目将创建、划分为以下命名空间：
namespace math {}
namespace physic {}
```
