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

这是合法的。因为 `namespace` 关键字的意思它不是定义或创建命名空间，并不会重复
定义命名空间，它只是打开命名空间，所以可以在同一个源文件（编译单元）中多次打开
与关闭某个命名空间。只要保证在同一个命名空间中不会对同一个实体（变量，函数，类
等）进行重复定义，但可以重复声明。也无需在某处预先创建命名空间，当然也可以只跟
个空大括号假装在创建命名空间，统一列在一起或许对项目管理有清晰规范之意味。如：

```c++
// 本项目将创建、划分为以下命名空间：
namespace math {}
namespace physic {}
```

### 引入命名空间的使用： using

再次回顾 C 风格的命名前缀，若以全名引入命名空间内的变量如 `math::value_` 并不
比命名前缀 `math_value_` 精简。但在同一个命名空间引用其他变量，可以省略命名空
间限定符，达到精简书写的目的。比如在各自命名空间内对 `GetValue()` 函数的实现，
就可以直接使用 `value_` ，若只能用 C 风格前缀，则无论在哪都必须明确写成
`math_value_` 或 `physic_value_` 。

像这样 C++ 命名空间内不必加前缀的写法有个好处是，将部分代码拷到另一个命名空间
后可以不改变量名，保持更多的命名风格统一。

在命名空间外，或引用其他命名空间的符号时，C++ 也有语法功能可以省略命名空间限定
符，精简命名空间前缀的书写。这就是用关键字 `using` 引入命名空间符号的作用，但
这个功能注意不得滥用，得慎用。`using` 又主要分为两种用法：

* 导入全部符号，`using namespace xxx;` 其中 `xxx` 为某个命名空间的名字。
* 导入特定符号，`using xxx::symbol;` 其中 `symbol` 为某空间内具体的某个符号。

全部导入的示例如：

```c++
// sample3.cpp
#include <iostream>
int main (int argc, char* argv[])
{
    {
        using namespace math;
        SetValue(111);
        std::cout << "value in math: " << GetValue() << std::endl;
    }
    {
        using namespace physic;
        SetValue(222);
        std::cout << "value in pyhsic: " << GetValue() << std::endl;
    }

    using namespace std;
    cout << "value in math: " << math::GetValue() << endl;
    cout << "value in math: " << physic::GetValue() << endl;
    return 0;
}
```

被 `using` 导入的符号的有效范围，就在写 `using` 语句的那个作用域内，即它所在那
个大括号范围内，更准确点，是从写下 `using` 语句开始到闭大括号之间。若 `using`
不在任何大括号内，那就是作用于当前文件（编译单元）的范围。所以在上例中，在
`using namespace std` 之前，必须写全名 `std::cout` 与 `std::endl` ，在那之前才
能简写为 `cout` 与 `endl` 。而前面的 `using namespace math` 与 `using
namespace physic` 在离开大括号后又不可见了，故又必须加上 `::` 限定符写全名。

为什么说 `using` 要慎用？因为滥用 `using` 也可能导致符号冲突。不能因为贪图书写
的简便而使命名空间的划分形同虚设，那就本末倒置了。比如，在上例中，我们不能将
`using namespace math` 与 `using namespace physic` 这两个语句并列写在同一个作
用范围，因为它们之间有相同的符号。

特别地，我们也不能滥用标准命名空间 `using namespace std` ，因为 `std` 包含了太
多的符号，很可能一不小心就冲突了。底线是只能在一个 `.cpp` 源文件中使用该语句，
红线是不能在头文件中使用。因为我们无法保证在将来这个头文件会有多少个源文件
`include` 它。

所以，在较大的文件级作用域，更建议只使用特定导入的 `using` 语法，只导入当前真
的需要部分使用符号。如：

```c++
// sample4.cpp
#include <iostream>
// using namespace std; //< 不建议使用
using std::cout;
using std::endl;
int main (int argc, char* argv[])
{
    {
        using namespace math;
        SetValue(111);
        cout << "value in math: " << GetValue() << endl;
    }
    {
        using namespace physic;
        SetValue(222);
        cout << "value in pyhsic: " << GetValue() << endl;
    }

    return 0;
}
```

很多 C++ 教程的示例代码，都会不假思索地将 `using namespace std` 当作起手式，实
在是流毒无穷。那只不过因为教学性质的代码为了精简写法，以突出当前上下文要讲解的
语法特性。但在实际工程项目中请三思而后用，当然个人玩具项目代码随意。

据个人的开发习惯，是几乎不对 `std` 使用 `using` 的，全都显式写上 `std::` 前缀
，这能很直观地表示所用符号（函数或类）来自标准库。尤其是对于一些不太常用的标准
库，别人不一定能一眼就意识到它是被收录于标准库的东西。所以始终多写几个字符的标
准库前缀是值得的，只有在嵌套模板容器中会显得特别长可能引起不适感，如
`std::map<std::string, std::string>` ，对于这种，尤其用 `using` 省前缀写成
`map<string, string>` ，不如就用 `typedef` 重定义为更有针对性的类型名。
