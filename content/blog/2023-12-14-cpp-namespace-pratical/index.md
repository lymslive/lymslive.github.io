+++
title = "C++ 命名空间理解与践行"
draft = false
[taxonomies]
categories = ["程序设计"]
tags = ["C++"]
[extra]
toc = true
+++

众所周知，C++ 算是 C 的超集，提供了非常丰富的高级特性。对一般 C++ 开发者而言，
并不要求精通 C++ 的每个特性，而应该按需学习与强化。在众多特性中，也许命名空间
是很不起眼，平平无奇那个。但假如做个思想实验，摒弃 C++ 绝大部分特性，只保留一
个特性加到 C 语言中，哪个最有助于改善 C 语言项目的开发体验与维护体验？

我觉得不是类（虽然 C with class 也是 C++ 的一种实用范式），也不是模板，或其他
奇淫巧技，而是命名空间。就因为 C 语言中几乎只有一个全局的命名空间，所有函数、
结构体等类型，以及可能必要的全局变量，都塞在同一个空间，对维护大型项目而言
太拥挤太可怕了，很容易出现命名冲突。C++ 的命名空间（namespace）就是为解决这个
问题设计的，后现代大多数语言都有的模块（module）或包（package）的一个作用也能
解决命名冲突，但更主要的作用是从项目整体宏观上抽象出模块的分组与组织。
<!-- more -->

## 从 C 命名前缀到 C++ 命名空间

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
并且命名空间的前缀在具体使用中有可能被简化或省略。命名空间是在 modern C++(11)
之前就已支持的语法特性，下文的内容除特别指明外，绝大部分也适用 C++11 之前的版
本标准。

## C++ 命名空间基本概念详解

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
定义命名空间，它只是打开命名空间，继续往里面添加代码。所以可以在同一个源文件
（编译单元）中多次打开与关闭某个命名空间，只要保证在同一个命名空间中不会对同一
个实体（变量，函数，类等）进行重复定义，但可以重复声明。也无需在某处预先创建命
名空间，当然也可以只跟个空大括号假装在创建命名空间，统一列在一起或许对项目管理
有清晰规范之意味。如：

```c++
// 本项目将创建、划分为以下命名空间：
namespace math {}
namespace physic {}
```

虽然原则上我们只应修改自己规划创建的命名空间，但理论上也可以打开来自标准库
或三方库的命名空间，往里面扩充代码，只需警惕在更新库时可能会发生潜在的符号冲突。
一个比较安全有实用的例子是在头文件的前向声明。如果库本身没有提供简单的前向声明
头文件（或你未在意而不知道），那就可以在头文件中手动打开其命名空间，写上自己要
用到的类前向声明，然后在源文件中才真正包含库的完整头文件。

譬如说，你定义了一个类，用到 `std::string` 来作字节缓冲，并且只在需要时才用到，
所以成员是 `std::string*` 指针类型，如果你觉得 `<string>` 头文件太大有洁癖，那
就可以手动打开 `std` 命名空间，添加前向声明。如：


```c++
// CMyClass.h
namespace std
{
    class string;
}

// 但不应该将自己定义的类塞到 std 空间
class CMyClass
{
    std::string* m_pBuffer;
    // todo
};

// CMyClass.cpp
#include <string>
// 类实现代码
```

当然，这是个假想的例子。标准容器我们一般直接用值，而不是间接的指针，只有指针才
能前向声明。但道理仍是这个道理。另一个例子是在低版本 C++ 中临时模拟高版本 C++
标准库才提供的功能。比如你由于某些难以启齿的原因还必须使用 C++98 ，但又觉得
C++11 有个 `std::to_string()` 很好用，那么就不妨自己打开 `std` 命名空间，自己
写个简单的 `std::to_string()` 实现（数字转字符串）。当然，为了避免将来升级到
C++11 （以上）时出现编译错误，最好将自己加的实现用条件编译判断 C++ 版本号。如：

```c++
namespace std
{
#if __cplusplus < 201103L
    string to_string(int value) // 注：可用模板支持多种数值类型
    {
        stringstream oss;
        oss << value;
        return oss.str();
    }
#endif
}
```

总之，命名空间是开放的，C++ 是灵活的，需要程序员自己把握。

### 引入命名空间的使用： using

再次回顾 C 风格的命名前缀，若以全名使用命名空间内的变量如 `math::value_` 并不
比命名前缀 `math_value_` 精简。但在同一个命名空间引用其他变量，可以省略命名空
间限定符，达到精简书写的目的。比如在各自命名空间内对 `GetValue()` 函数的实现，
就可以直接使用 `value_` ，若只能用 C 风格前缀，则无论在哪都必须明确写成
`math_value_` 或 `physic_value_` 。

像这样 C++ 命名空间内省略前缀的另一个好处是，将部分代码拷到另一个命名空间后可
以不改变量名，保持更多的命名风格统一。

在命名空间外，或使用用其他命名空间的符号时，C++ 也有语法功能可以省略命名空间限定
符 `::`，精简书写。这就是用关键字 `using` 引入命名空间符号的作用，但
这个功能注意也不得滥用，得慎用。`using` 又主要分为两种用法：

* 导入全部符号，`using namespace xxx;` 其中 `xxx` 为某个命名空间的名字。
* 导入指定符号，`using xxx::symbol;` 其中 `symbol` 为某空间内具体的某个符号。
* 别名导入符号，`using sym = xxx::symbol;` 可为指定导入的符号改换名字。

全部导入，原术语叫 using directive ，指定导入叫 using declaration，别名导入类
似 `typedef` ，但比 `typedef` 强的地方是还能带模板参数，所以也叫 alias
template 。别名导入是 C++11 的功能，此前就用 `typedef` 吧。

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
不在任何大括号内，那就是作用于当前文件（编译单元）之后的范围。所以在上例中，在
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

所以，在较大的文件级作用域，更建议只使用指定导入的 `using` 语法，只导入当前真
的需要使用的部分符号。如：

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
语法点。但在实际工程项目中请三思而后用，当然个人玩具项目代码随意。

据个人的开发习惯，是几乎不对 `std` 使用 `using` 的，全都显式写上 `std::` 前缀
，这能很直观地表示所用符号（函数或类）来自标准库。尤其是对于一些不太常用的标准
库，别人不一定能一眼就意识到它是被收录于标准库的东西。所以始终多写几个字符的标
准库前缀是值得的，只有在嵌套模板容器中会显得特别长可能引起不适感，如 `std::
map<std::string, std::string>` ，对于这种，尤其用 `using` 省前缀写成
`map<string, string>` ，不如就用 `typedef` 重定义或 `using` 别名导入为更有针对
性的类型名，例如：

```c++
using SStrMap = std::map<std::string, std::string>;
// 等效 typedef
// typedef std::map<std::string, std::string> SStrMap;

template<typename T>
using StrMap = std::map<std::string, T>;

// later use:
// StrMap<int> 等效 std::map<std::string, int>;
```

### 类与命名空间的相关性

首先要意识到一个事情，C++ 的类也起着命名空间的作用，想想不同类可以有同名成员，
这就显而易见。另外，类的静态成员（与方法）在类外使用的形式，也与命名空间完全相
似。举个例子：

```c++
struct Circle
{
    double x;
    double y;
    double r;

    static double PI;
};

double Circle::PI = 3.14159;

int main()
{
    Circle c;
    c.r = 2.0;
    double s = Circle::PI * c.r * c.r;
    // 注：c.PI 也是允许的
}
```

这里，我们定义了一个表示圆形的类 `Circle` 。为简单起见，用 `struct` 关键字了，
在 C++ 中它与 `class` 几乎是同义词，除了默认的访问权限为 `public` 。另外直接用
单字母符号表示圆心坐标与半径了，正常项目命名规范应该是有意义的单词，不过若只为
解决数学问题的项目，用对应数学符号的单字母或许也是可接受的。

关注点在于 `Circle::PI` 的用法，与命名空间是相同的。事实上，从业务抽象角度考虑，
圆周率不仅用在圆形类，其他数学领域也是经常用到的，所以将其放在 `math` 命名空间
应该更合适，那么使用圆周率就该写成 `math::PI` 。两者写法并无本质区别，脱离上下
文单看这句，无法获知 `PI` 是来源于某个命名空间还是某个类的静态成员。

类与命名空间的最大区别是，类在运行时可以定义实例变量，如上例的 `c` ，那也就相
当于创建了一个运行时命名空间，每个 `c` 变量有其独立的空间隔离表示其成员的符号
`x` 、 `y` 与 `r` 。为了区分这种运行期与编译期的名字隔离，C++ 择用了不同的分隔
符，单点 `.` 与双冒号号 `::` ，而命名空间的符号解析，更像类的静态成员，可在编
译期确定，所以也用 `::` 。事实上，类在 C++ 一开始就有的，命名空间是后来版本添
加的，类的功能很多，命名空间可以认为是将类的一部分功能再抽象独立出来，从更宏观
的角度专门做名字隔离的工作。

再注意到一个事实，类的方法可以在类外定义，在语法上定义每个方法时都还要加上类名
与 `::` 限定符。从命名空间理解，那也就相当于重新打开了该类代表的命名空间，所以
在方法体大括号内就可以直接使用该类或命名空间内的符号了。例如：

```c++
struct Circle
{
    double x;
    double y;
    double r;

    static double PI;

    double Area();
};

Circle::PI = 3.14159;

double Circle::Area()
{ // 打开命名空间 Circle
  return PI * r * r;
} // 关闭命名空间 Circle
```

再严格点说，方法的返回值类型 `double` 写在 `Circle::` 前面，所以返回值类型不在
该类的命名空间，当然这是个内置浮点类型，所以毫无违和感觉。但如果方法的返回类型
也是在类内定义的子类型，那写法就该不一样了。假设我们要为 `Circle` 类添加一系列
的变换方法，每个方法返回自身引用，以方便链式调用，就如支持如下写法：

```c++
Circle c;
c.resize(3.0).scale(1.5).move(3.0, 4.0);
```

为实现这个目的，我们增加一个 `typedef` 类型，用 `SelfType` 表示自身类型，这样
看起来与 `return *this` 更登对，意图更明显。在类内定义这些方法很直观，如：

```c++
struct Circle
{
    double x;
    double y;
    double r;

    // ...
    typedef Circle SelfType;

    SelfType& resize(double a_r)
    {
        r = a_r;
        return *this;
    }
};
```

但当你移到类外定义时，返回值就得写成 `Circle::SelfType`：

```c++
Circle::SelfType& Circle::resize(double a_r)
{
  r = a_r;
  return *this;
}
```

若省略类名前缀就会报告 `SelfType` 未定义，因为编译器在扫描到返回值时，还未进入
到 `Circle` 类的命名空间。但是如果用 C++11 语法，返回值可以写在后面（前面须用
`auto` 占位），那就不用加类名前缀了，如：

```c++
auto Circle::resize(double a_r) -> SelfType&
{
    r = a_r;
    return *this;
}
```

可以认为，当编译器看到 `Cirle::` 就知道打开命名空间了，所以不但后置返回类型，
而且参数的类型也可以使用类内的子类型。例如实现赋值操作符，可以这么写：

```c++
struct Circle
{
    // ...
    typedef Circle SelfType;
    SelfType& operator=(const SelfType& that);
};

auto Circle::operator=(const SelfType& that) -> SelfType&
{
    this->x = that.x;
    this->y = that.y;
    this->r = that.r;
    return *this;
}
```

参考类方法可移出类外定义的写法，命名空间内的函数定义也可移到外面，如：

```c++
namespace math
{
    int value_ = 1;
    int GetValue();
    void SetValue(int value);
}

int math::GetValue()
{
    return value_;
}

void math::SetValue(int value)
{
    value_ = value;
}
``

这是允许的语法，能通过编译。但一般没必要这么写，因为 `namespace` 关键字只是
打开空间，所以可以分开在头文件内声明空间内的函数，而在源文件重新打开空间定义函
数实现。而 `class` 或 `struct` 关键字是定义类，所以无法像命名空间那样重新定义
类，只好分别针对每个方法用 `::` 打开与类关联的命名空间，完成方法实现代码。如果
说命名空间的函数定义移到外面的写法有什么优点的话，我唯一想到的是可以减少缩进层
次，对于复杂函数与嵌套命名空间，缩进太多也是难看。

基于命名空间与类的相似性，再考虑一种特殊的类，只有静态方法或成员的类。在某个宣
称一切皆类的语言中，因其无法定义自由函数，就很容易就定义出这样的类，大约长这样
子：

```c++
class CMath
{
public:
    static int Add(int a, int b) { return a + b; }
    static int Sub(int a, int b) { return a - b; }
    // ...
};
```

这是合法的 C++ 代码，但没必要这样写，用 `namespace` 是更好的选择，不必反复写
`static` 关键字。故可改写成：

```c++
namespace math
{
    int Add(int a, int b) { return a + b; }
    int Sub(int a, int b) { return a - b; }
    // ...
}
```

### 命名空间与类的嵌套

命名空间与类都可以嵌套，形成多层次的作用域空间，这是很直接的语法扩展。

最常见的是将类放在某个命名空间，而类也起着命名空间的作用。如上节定义的
`Circle` 类，就很适合放在 `math` 命名空间。而 `math` 与 `physic`命名空间也可以
再放在另一个更大的命名空间，不妨取名`sci` 。于是代码框架形如：

```c++
namespace sci
{
    namespace math
    {
        double PI = 3.14159;
        struct Circle
        {
            double x;
            double y;
            double r;
            double Area() { return PI * r * r; }
            static double Area(double r) { return PI * r * r; }
        };
    }

    namespace phsyic
    {
        double G = 9.80;
        // ...
    }
}
```

类 `Circle` 定义在嵌套的命名空间中，在外部使用该类时也只要用命名空间限定符
`::` 依次给出空间名即可，并如其他难度：

```c++
int main()
{
    sci::math::Circle c;
    c.r = 2.0;
    std::cout << "c.Area() = " << c.Area() << std::endl;
    std::cout << "Circle::Area(3.0) = " << sci::math::Circle::Area(3.0) << std::endl;
    std::cout << "c.Area(3.0) = " << c.Area(3.0) << std::endl;
}
```

如果真嫌全限定的名字太长，在合适的地方可以考虑使用 `using` 。另外在上例特意在
`Circle` 类中重载了成员函数与静态函数都取名 `Area` 。请注意在无实例情况下通过
`::` 级联取到静态函数是很长的路径，但也能通过定义一个实例来访问静态方法，这也
极大化简了写法，每个实例自成空间且与类空间有镜像关系。

类可以放在命名空间中，但反过来，命名空间却不能放在类中。事实上，命名空间也不能
放在函数中，它只能放在全局空间或其他命名空间中。如果需要在类中定义一个类似子空
间的东西，可以定义为子类。如：

```c++
namespace sci { namespace math {
double PI = 3.14159;

struct Circle
{
    double x;
    double y;
    double r;
    struct Impl
    {
        static double Area(double r) { return PI * r * r;}
    };
    double Area() { return Impl::Area(this->r); }
    static double Area(double r) { return Impl::Area(r); }
};
}}
```

这里，全静态方法的类 `Impl` 却是可以伪装成一个子命名空间。

子空间可以看到父级以上空间的符号，可以直接使用，除非各级之间存在同名符号产生符
号隐藏，则需要用全限定名明确使用哪个，这实际项目中应极力避免，不要自找麻烦。
父空间不能直接看到子空间内的符号，需要用限定符逐级进入子空间以引用其符号。

事实上，命名空间嵌套的语法并不难，难的是对实际项目的业务功能抽象，如何用命名空
间来组织代码能提高代码可读性与可维护性。这可能是与设计类继承体系不同的思维方式
，而类的继承思想已逐渐被新兴语言抛弃了。

### 匿名空间与名字提升

前文多次提到，全局空间就是没有命名空间。还有另一个命名空间，它虽然有用
`namespace` 开辟一个命名空间，却是没有名字，直接跟一对大括号，这就是匿名空间。
匿名空间不能被其他源文件（编译单元）所访问，因为它没有名字，无法用 `using` 导
入。所以只能在当前文件访问，并且无需 `::` 限定符，相当于 C 语言文件级的静态函
数（内部链接）。

其实匿名空间就是用来完美取代静态函数的，当文件内有多个静态函数时，用匿名空间封
装可精简重复的 `static` 关键字。更重要的是，在 C++ 中，`static` 关键字是被滥用
的，有多重涵义，以致可以当作 C++ 初级面试题了。所以在有其他方法可不用 `static`
时尽量不用 `static` 。

仍取上节的示例，假如我们觉得 `Circle::Impl` 内部类的求面积函数比较通用，想提到
（限当前文件）全局作用域来，或者说可以从其他地方抄个 C 函数的实现来，那就可以
放在文件开头的匿名空间中。如：

```c++
// anonymous.cpp
namespace // static
{
    double area(double r) { return 3.14159 * r * r;}
}

namespace sci { namespace math {
    struct Circle
    {
        double x;
        double y;
        double r;
        double Area() { return area(this->r); }
        static double Area(double r) { return area(r); }
    };
}}
```

另外说明一下，这里将匿名空间的求面积函数名改为小写的 `area` ，需要与
`Cirle::Area` 方法名区分，否则虽能编译通过，却会在调用 `Circle::Area` 函数时发
生无限递归，因为就近原则，里面的 `Area` 方法名会屏蔽外面匿名空间的 `Area` 函数
名，那就始终调用自己了。如果不改名，也可以用 `::Area` 访问全局函数避免递归自身，
因为匿名空间的符号在全局空间也可见。

其实，匿名空间不仅可以放在全局空间中，也可以放在其他命名空间中，其本质意义是在
匿名空间内定义的符号也自己提升到父空间中可见。自 C++11 后，还有另一个命名空间
叫内联空间（在 `namespace` 关键字前加 `inline`），也可以将自身内部的符号提升到
父空间中。但是内联空间与匿名空间不同的是它仍然有名字，仍然可以给出包括自身空间
名的全限定名称来访问，但匿名空间内的符号因其无名只能通过父空间来访问。另外内联
空间在其他文件也可访问，即可进行外部链接。

内联命名空间的主要作用是版本控制。默认版本或当前版本可以省略版本名只用到父空间
的限定名，但也允许指定版本的空间名使用全限定名。仍以之前求圆面积的实现举例，还
是将其放到 `math` 命名空间，然后将不同精度的圆周率计算划分为不同版本：

```c++
// inline.cpp
namespace sci { namespace math {
    namespace v1
    {
        double PI = 3.14;
        double area(double r) { return PI * r * r;}
    }
    inline namespace v2
    {
        double PI = 3.14159;
        double area(double r) { return PI * r * r;}
    }
    struct Circle
    {
        double x;
        double y;
        double r;
        double Area() { return area(this->r); }
        static double Area(double r) { return area(r); }
    };
}}

#include <iostream>
int main()
{
    sci::math::Circle c;
    c.r = 2.0;
    std::cout << "c.Area() = " << c.Area() << std::endl;
    std::cout << "v1 area = " << sci::math::v1::area(c.r) << std::endl;
    std::cout << "v2 area = " << sci::math::v2::area(c.r) << std::endl;
    std::cout << "v? area = " << sci::math::area(c.r) << std::endl;
}
```

该示例输出为：

```
c.Area() = 12.5664
v1 area = 12.56
v2 area = 12.5664
v? area = 12.5664
```

因为 `math` 空间下面的 `v2` 空间被标记为 `inline` ，所以 `v2` 空间算是当前版本
的默认实现，其内的符号在父空间 `math` 直接可见，也即有 `math::PI ==
math::v2::PI == 3.14159` 。

内联空间对库开发者比较有用，可用于保持兼容性。而匿名空间对普通开发者比较实用，
C 风格的静态函数可弃之了。

在 C++ 还有另外一个语法结构，可以将词法作用域（大括号）内部的符号名字提到上层
作用域，那就是沿袭 C 的枚举 `enum` 。事实上，旧式枚举类型还可以省略名字，但不
管有无名字，其中的各个枚举项都是父空间可见的常量。于是，省略名字的枚举类型就相
当于匿名命名空间，有名字的枚举类型就相当于内联命名空间。在没有充分意识到枚举项
提升作用域的情况下，就容易造成枚举常量的命名冲突。因此，旧式枚举项在命名上常见
统一加上它们所属的枚举类型的名字前缀并大写。于是，在 `c++11` 又加入一种枚举类
类型，也叫有范围的枚举（scoped enumeration），语法是在 `enum` 关键字与枚举类型
名之间再添加 `class` 或 `struct` 关键字。几种不同风格的枚举示例如下：

```c++
// enum.cpp
namespace parent
{
    enum { FALSE, TRUE };
    enum InLine { ZERO, ONE, TWO };
    enum class LoveColor { RED, GREEN, BLUE };
    enum class HateColor { RED, GREEN, BLUE };
}
```

其中，父空间的 `parent::FALSE` 与 `parent::TRUE` 常量来自匿名枚举，内联枚举提
供了 `parent::ZERO` 至 `parent::TOW` 的常量符号，而 `parent::LoveColor::RED`
与 `parent::HateColor::RED` 枚举项在各自不同的子空间，互不影响。如果后两个枚举
类型去掉 `class` 关键字，就会造成 `parent` 空间的名字冲突。此外提一句，旧式枚
举弱类型，可隐式转换当整数用，新式枚举是强类型，有 `class` 的类型特征，不能隐
式转为整数，如果真要当作整数用，要用 `static_cast<int>` 显式转换。

## C++ 项目利用命名空间的实践探讨

以上讲了 C++ 命名空间的语法特性。下面谈谈在实际项目中可以如何利用命名空间更好
地组织代码，主要是个人的经验与尝试，不敢说最佳实践。与设计模式不同，或可称之为
设计章法，比具体的设计模式更粗粒度更宏观一层的。又有点像开发规范，但是不纠结于
命名、格式之类的细枝末节。

### 基本原则

1. 所有 C++ 代码应该封装在某个命名空间，全局空间只保留给 C 库。
2. 头文件除了命名空间分隔，只应包含简单的 C 语法元素，C++ 的复杂功能实现隔离隐
   藏在源文件。
3. 尽量保持头文件的简单性，其他源文件用不到元素，决不声明在头文件中。

以上所列几点是指导原则，尽量达到的目标，但也不应是教条，可根据具体遇到的问题作
一定的规范调整。若按此原则，从外部接口看，那就是 C with namespace 的 C++ 开发
范式。

先看第一点，应无疑异，不难达成。应该没有纯粹的 C++ ，但凡正常有实用的 C++ 项目，
几乎不可避免要链接 `libc` 吧，还有用 C 写的操作系统的系统调用。既然 C 必定是伴
生的，那就将 C 代码留在全局空间，而将所有自己写的 C++ 用命名空间保护起来。

使用的第三方 C++ 库，也优先使用有命名空间的。有些第三方 C++ 库还支持配置让用户
选个命名空间名字。如果有简短精巧的工具库没有命名空间，可以拉源码修改重编译，给
其套个命名空间也非难事。第三方 C 库，那就让它待在全局空间，优秀的 C 库，导出的
函数一般有统一前缀。

如果某个功能既有第三方 C++ 库，又有 C 库，我推荐使用 C 库。除了 C 库一般更稳定
可靠外，我还认为 C++ 是个性化很强的语言，别人的封装不一定适合自己，不如就直接
拿 C 库来，只用其中自己需要的功能，如果必要，再浅封装那部分用得上的功能，封装
的过程也是学习吃透 C 库的过程。

此外，在调用标准 `libc` 库或系统函数，或其他名字很短的 C 函数时，建议加上 `::`
前缀显式表明将调用全局空间的函数，避免意外地被当前空间的同名函数屏蔽。

第三点也容易理解与达成。主要难点是后续修改后要维护一致性会麻烦点，如有外
部检测工具辅助会省事不少。头文件是 C/C++ 的特色与即成史实，向来被其他语言鄙视。
但我认为头文件也不是一无是处，浏览头文件的声明，有提纲挈领的作用，而看其他没有
头文件的先进语言的模块源代码，不可避免要与实现细节一起看，要么人工过滤找重点，
要么使用文档工具提取单独的文档。所以要规范 C/C++ 头文件，以简约为第一要务，扬
长避短。从某种意义看，头文件就是针对与其对应的源文件所写的 `export` 语句，故只
应导出真正想导出的元素（即要公开给别的源文件使用的接口），除此之外，不为该目的
服务的废话都不应该存在，当然，注释文档不算废话。

比较复杂的是第二点，重点讨论一下，也就是在头文件中，具体允许什么存在。

若按纯净 C 的要求，头文件的主角就是函数声明，以及函数参数或返回值需要的结构体
类型与枚举类型。因为当函数参数非常多时，用结构体显然是更好的组织方式，比如配置
选项可以有很多。在头文件定义的结构体，以传参（输入输出）为主要目的，与源文件实
现中对业务抽象而定义的结构体与类，可以不尽相同，各有侧重。

头文件也尽量不要包含其他头文件，否则会形成复杂的头文件依赖。在管理第三方库不善
时，头文件的包含顺序还可能引发 Bug 。所以如果函数需要用到来自其他头文件或第三
方库的类型时，尽量使用指针或引用，这样就能自己加上前向声明，然后只在实现源文件
中真正包含所需的头文件。头文件的理想状态是最多只包含必要的标准库头文件，甚至
标准库头文件也不需要，就是纯粹地给他所服务的源文件导出声明。

尤其要说明的是，不要在头文件中定义 C++ 类。因为类其实是个很复杂的东西，即使开
局一个简单的类，将来也可能变得很复杂。把类定义放在头文件中，就无法真正做到导出
声明与隐藏实现的分离。只在源文件中定义类，把类当作一种实现手段，而不是一种接口，
接口只用 C 函数交互理论上也就够了。

这里试举一例说明如何将 C++ 类定义隐藏在源文件中。假设我们要开发一个模块（业务
抽象的模块，不是 C++20 的模块技术），规划放在 `mymod` 命名空间，其中绝大部分功
能可通过一个类 `CMyClass` 来实现。那么头文件可以只有前向声明，及几个自由函数：

```c++
// hide-class.h
namespace mymod {
    struct CMyClass;
    CMyClass* GetInstance();
    CMyClass* NewObject(/* init-args */);
    void FreeObject(CMyClass* pObject);
    void Foo(CMyClass* pObject);
    void Bar(CMyClass* pObject);
} // end of namespace
```

然后在源文件完整实现类，并将导出的自由函数转发为类方法调用：

```c++
// hide-class.cpp
namespace mymod {
struct CMyClass {
    CMyClass(/* init-args */);
    void Foo() { /* todo */ }
    void Bar() { /* todo */ }

    // data member ...
};

CMyClass* GetInstance() {
    static CMyClass instance(/* defalut-args */);
    return &instance;
}

CMyClass* NewObject(/* init-args */) {
    CMyClass* pObject = new CMyclass(/* init-args */);
    return pObject;
}
void FreeObject(CMyClass* pObject) {
    delete pObject;
}

void Foo(CMyClass* pObject) {
    pObject->Foo();
}
void Bar(CMyClass* pObject) {
    pObject->Bar();
}
} // end of namespace
```

可以看到，头文件很精简，甚至不需包含标准库文件，而源文件也不必包含自己的头文件
（当然按习惯最好加上），那个头文件只是给别的源文件包含使用的。

创建对象专门由一个函数 `NewObject` 来负责，按设计模式高大上的词汇，这也相当于
Builder 模式。当获得一个对象后，就按 C 函数的方式将该对象传为第一个参数，用以
完成其他工作。试想如果将参数 `pObject` 改名为 `self` （当然不能用 `this` ），
那是不是更有感觉了呢。对象用完，再调用 `FreeObject` 函数释放对象。如果当初在
工厂函数 `NewObject` 中返回的不是原始指针，而是 `std::unique_ptr<CMyClass>` ，
那不必提供释放函数了。`unique_ptr` 是零成本抽象，与原始指针一样性能，不过本文
重点不在智能指针，不再细谈。

此外，也很容易实现所谓的单例模式，只要在头文件中只保留 `GetInstance` 函数，删
掉 `NewObject` 函数声明即可，别人就无法创建第二个实例了。其实我是不太推荐严格
地在类设计加以限制只能创建一个实例的，保不准啥时候突然就有多实例的需求了。笔者
在工作中就曾接到一个优化需求，要去单例化，很蛋疼。所以，与其使用单例模式，不如
使用有默认实例的模式。

需要将一些自由函数转调类方法，可能会让人觉得繁琐。但这也正要求我们仔细设计接口，
只导出必要的对外接口，不需要把类的每个方法都冗余地写个自由函数转发。不要把类设
计本身当作目标，不能为设计了一个复杂完备的类而沾沾自喜，写一堆实际用不上的方法
是没必要的。

再看头文件的几个函数声明，这其实就是 C 语言模拟面向对象的基本方式。不过我们既
然是在写 C++ ，在源文件的具体实现中，就没必要自缚手脚了。在 C 语言模拟多态会复
杂点，虽然也能办到，但这也不是本文的重点。要实现这点，不妨将第二条原则稍放开，
也允许充当接口的纯虚类定义，它里面只有虚函数方法。

将上例改成支持多态，头文件形如：

```c++
// hide-interface.h
namespace mymod {
    struct Interface {
        virtual ~Interface() {}
        virtual void Foo() = 0;
        virtual void Bar() = 0;
    };
    Interface* CreateObject(int arg);
    void FreeObject(Interface* pObject);
} // end of namespace
```

源文件形如：

```c++
// hide-interface.cpp
#include "hide-interface.h"
#include <stdio.h>
namespace mymod {
    struct CHisClass : public Interface {
        CHisClass() {}
        virtual void Foo() override { printf("CHisClass::Foo\n"); }
        virtual void Bar() override { printf("CHisClass::Bar\n"); }
    };

    struct CHerClass : public Interface {
        CHerClass() {}
        virtual void Foo() override { printf("CHerClass::Foo\n"); }
        virtual void Bar() override { printf("CHerClass::Bar\n"); }
    };

    Interface* CreateObject(int arg) {
        if (arg == 1) {
            return new CHisClass();
        } else {
            return new CHerClass();
        }
    }
    void FreeObject(Interface* pObject) {
        delete pObject;
    }
} // end of namespace
```

这里将 `NewObject()` 函数改成 `CreateObject()` ，并且实现上根据参数创建不同的
具体子类，这就更像工厂函数了。然后也不需要其他自由函数来转发对象方法了，因为在
创建出对象后，可以直接使用接口方法了，一个使用的示例如：

```c++
// use-interface.cpp
#include "hide-interface.h"

int main()
{
    mymod::Interface* p = mymod::CreateObject(1);
    p->Foo();
    mymod::FreeObject(p);
    p = mymod::CreateObject(2);
    p->Bar();
    mymod::FreeObject(p);
}
```

输出：

```
CHisClass::Foo
CHerClass::Bar
```

这用起来就更有面向对象的特征了，但这不应成为追求目标。在 C++ 中，如非必要，也
不建议滥用虚函数多态继承。对于不需多态的简单类，就按前面的 `hide-class.h` 示例
隐藏实现即可，用 C 风格的函数转发对象方法也没问题。如果定要支持用户用对象的方
法，也可以通过常见的 `Impl` 内部类实现。

例如头文件可这么写：

```c++
// hide-impl.h
namespace mymod {
struct CMyClass
{
    CMyClass();
    ~CMyClass();
    void Foo();
    void Bar();

private:
    struct Impl;
    Impl* pImpl = nullptr;
};
} // end of namespace
```

源文件这么写：

```c++
// hide-impl.cpp
#include "hide-impl.h"
#include <stdio.h>
namespace mymod {
struct CMyClass::Impl {
    void Foo() { printf("Impl::Foo\n"); }
    void Bar() { printf("Impl::Bar\n"); }
};

CMyClass::CMyClass() {
    pImpl = new Impl;
}
CMyClass::~CMyClass() {
    delete pImpl;
}

void CMyClass::Foo() {
    pImpl->Foo();
}
void CMyClass::Bar() {
    pImpl->Bar();
}
} // end of namespace
```

如果不想将内部实现定义成子类 `CMyClass::Impl` ，也可以直接在同级命名空间定义个
辅助类 `mymod::CMyClassImpl` ，并无太大差别，我推荐用子类，更显空间层次感。对
该类的一个使用示例如：

```c++
// use-imple.cpp
#include "hide-impl.h"
int main()
{
    mymod::CMyClass obj;
    obj.Foo();
    obj.Bar();
}
```

输出：

```
Impl::Foo
Impl::Bar
```

这看起来与常规使用类对象没啥区别了。但是再审视一下在头文件 `hide-impl.h` 定义
的 `CMyClass` 类，若非教程示例性代码，可能就没那么简单了。它定义了构造函数，那
就是非平凡 (non-trivial) 类了，而且既然定义构造与析构，那四件套（或六件套）的
其他套装还要不要定义呢？就是拷贝构造函数，拷贝赋值函数，移动构造函数与移动赋值
函数。一般来说，类中有指针成员，那这六件套都得考虑，否则这个类暴露给了用户，说
不准哪天就给你捅出漏子来。在这里，那个唯一的 `pImpl` 指针，可以改用智能指针
`std:: unique_ptr` 免写析构函数，但也没有降低太多复杂度。所以说，C++ 的类是个
很复杂的东西，不如全移到源文件去，隔离用户，不给用户压力。

不妨再分析一下业务需求。假设用户代码（其他源文件）的典型用法，就是创建该类的一
个对象，再依次调用其 `Foo` 与 `Bar` 两道工序方法，那其实在头文件甚至不用暴露这
个类的存在，只要在源文件中再添加一个函数 `Work`，组合这样的工序流，如：

```c++
// hide-class.cpp
namespace mymod {
// ...
void Work(/* arguments */)
{
    CMyClass obj(/* arguments */);
    obj.Foo();
    obj.Bar();
}
} // end of namespace
```

然后在头文件中只要导出 `Work` 这一个函数，其余的都用不上删了，然后在外面也就只
要调用 `mymod::Work` 函数能完成任务就行，完全不用管它内部是用类来实现还是其他
什么来实现。

所以，在头文件该写什么，要根据具体项目分析而定。总之以导出最小化为原则，以函数
为主，不要有类。如果项目复杂，可以允许有纯数据的结构体（传参），或纯虚函数的
接口类。其实若不限纯 C 语法的话，（在命名空间内）也不妨有重载函数声明，新式的
枚举类 `enum class` ，以及 `std::function` 类型代替函数指针用于设置回调函数
参数（这或可代替虚函数表实现多态需求）……这些 C++ 特性，也不会增加太大的复杂
性及影响声明与实现的分离布局。

### 目标为可执行文件的项目

C++ 项目按最终编译的成品分，主要分有两类，一是可执行文件，二是动态链接库，其实
还有一类是静态库，不过那几乎是将各个目标文件的简单打包，故此不多作讨论。

先看可执行文件，它需要一个入口 `main` 函数，这是唯一例外的自己写的却不能放入
命名空间的函数，但我们可以尽可能将它写得简单，只一行转调。比如我们可以设计一个
命名空间来代表整个应用程序，不妨叫 `app` （或 `exe` ？），将程序的主要入口逻辑
写在该空间的一个函数，也可以叫 `main` ，不过这个单词较为敏感，我更喜欢取名为
`run` 。然后在全局空间的 `main` 就只要写一行调用，并且之后基本不需改动了。大致
结构形如：

```c++
// app.cpp
namespace app {
int run(int argc, char* argv[]) // main
{
    // todo: 命令行参数与配置解析
    // todo：业务逻辑处理
}
} // end of namespace

int main(int argc, char* argv[])
{
    return app::run();
}
```

如果涉及比较复杂的命令行参数或配置处理，可以增设一个子空间 `app::cli` 或
`app::conf` 。至于业务逻辑该怎么划分命名空间，那是具体项目具体分析了，这其实是
需求分析与整体设计，与 C++ 语言的关系并不是很密切。

### 目标为动态链接库的项目

动态链接库的项目，其实也有些必须的代码必须放到全局空间来，那就是需要导出的 C
函数，可以被其他程序链接或运行时加载的函数最好用 C 接口。这部分函数一般还要用
`extern C` 声明，避免 C++ 对函数符号的修饰。

可以专门设计一个命名空间 `dll` 来承接欲导出的动态链接库函数，并根据 C 习惯为这
些导出 C 函数取个统一前缀。比如头文件大致如下：

```c++
// dll.h
namespace dll
{
    void Foo();
    void Bar();
} // end of namespace

#ifdef __cplusplus
extern "C"
{
#endif
    void mydll_foo();
    void mydll_bar();
#ifdef __cplusplus
}
#endif
```

源文件大致如下：

```c++
// dll.cpp
#include "dll.h"
#include "foo.h"
#include "bar.h"

namespace dll
{
    void Foo()
    {
        foo::Work();
    }
    void Bar()
    {
        bar::Work();
    }
} // end of namespace

void mydll_foo()
{
    dll::Foo();
}

void mydll_bar()
{
    dll::Bar();
}
```

以上，`extern C` 只要在头文件声明即可，在源文件实现时可不加，如不确定，也可加
上。此外更常见的是将 `extern C` 及其前后大括号放在条件编译中，这是考虑当作纯 C
编译时没有 `extern C` 语法。于是总体上看，由 `extern C` 导出的 C 函数转调 `dll` 
空间的相应函数，但实际业务肯定也不可能全写在 `dll` 命名空间，一般是分在其他命
名空间，因此 `app` 空间的函数也只做一层转发而已。

另外注意，用 gcc 编译 linux 的动态链接库 `.so` 时，符号是默认导出的，而在
windows 编译动态链接库 `.dll` 时，符号是默认不导出的。因此，要精确控制只导出那
些设计为要导出的 C 函数，需要添加额外属性标注及调整相关编译链接参数，具体技术
细节就非本文想涉及的了。

## 总结

本文在厘清 C++ 有关命名空间的相关语法特性的基础，结合实际开发经验，尝试提出
“C with namespace”的 C++ 开发范式，在面向对象编程思想出现反思潮的当下，C
with namespace 也许是比 C with class 更简单易用的一种范式。命名空间不仅是避免
符号冲突的手段，更是对业务抽象进行模块化划分的体现。因此，合理使用命名空间，再
结合严格规范的头文件与源文件进行声明与实现的分离，能有效地改善 C++ 项目代码的
组织结构，提升代码可读性与可维护性。

在计算机编程界，自从 C++ 在 C 语言的基础上发展壮大以来，期间常有新兴语言诞生，
宣称自己是“更好的 C”，其拥趸者更是热切期待它们能取代万恶复杂的 C++。事实上，
C++ 从来就是更好的 C 。作为多范式编程语言，C++ 可以自由应对灵活多变的需求。如
果觉得 C++ 太复杂了，也可以有意识地在实际的具体的项目为之做减法。本文从相当宏
观的项目代码组织层面上，提出用命名空间及隐藏实现来隔离 C++ 的复杂度，希望能为
广大 C++ 开发者提供一种思想启迪。

## 附录：示例源码参考

* [sample1.cpp](./sample1.cpp) [sample2.cpp](./sample2.cpp) [science.cpp](./science.cpp) [science.h](./science.h)
* [shape.cpp](./shape.cpp) [nested.cpp](./nested.cpp)
* [anonymous.cpp](./anonymous.cpp) [inline.cpp](./inline.cpp) [enum.cpp](./enum.cpp)
* [hide-class.cpp](./hide-class.cpp) [hide-class.h](./hide-class.h)
* [hide-interface.cpp](./hide-interface.cpp) [hide-interface.h](./hide-interface.h) [use-interface.cpp](./use-interface.cpp)
* [hide-impl.cpp](./hide-impl.cpp) [hide-impl.h](./hide-impl.h) [use-impl.cpp](./use-impl.cpp)
* [dll.cpp](./dll.cpp) [dll.h](./dll.h)
