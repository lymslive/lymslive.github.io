+++
title = "couttast: 轻量级单元测试框架介绍"
[taxonomies]
categories = ["程序设计"]
tags = ["C++","单元测试"]
+++
<!-- # couttast: 轻量级单元测试框架介绍 -->

<!-- ## 引言 -->

笔者在之前一家公司的项目中曾经规范使用过 google test 作为单元测试。只是在实践
使用过程中也偶有不便，于是开始着手再造个轮子，从自己的习惯用法写个轻量级单
元测试框架（库）。

项目地址 [https://github.com/lymslive/couttast](https://github.com/lymslive/couttast.git) 。
初版是仅有 500 余行的头文件（head-only lib），无依赖，也不要求 C++11 ，自以为麻雀虽小，
五脏俱全。

取名源于两个单词拼接：`cout` 就是 `C++` 众所周知的那个 `std::cout`，`taste`
却不是 `test` 。在软件工程中，“测试”可能是个严肃的专业术语，但“尝试”不妨轻
松点，不必望而生畏。又为了与 `test` 这母长度相同，误写成了 `tast` 。
<!-- more -->
## 基本用法与最简示例

```cpp
#include "tinytast.hpp"

int main(int argc, char** argv)
{
    return RUN_TAST(argc, argv);
}

DEF_TAST(test_sizoef, "测试类型大小")
{
    COUT(sizeof(int));
    COUT(sizeof(int), 4);
    COUT(sizeof(int)==4, true);
    DESC("string 大小可能依编译器版本实现而不同");
    COUT(sizeof(std::string), 8);
}
```

唔，库名叫 `couttast` ，但那个主要的独立头文件命名为 `tinytast.hpp` 了。这段代
码能编译为可执行命令行程序。它用宏 `DEF_TAST` 仅定义了一个测试用例，在实际情况
下一般需要定义许多用例，可以组织到不同源文件中，只要每个 `.cpp` 都包含头文件
`tinytast.hpp` 即可。然后可用形如以下的命令编译、运行：

```bash
gcc -o utName.exe *.cpp
./utName.exe
./utName.exe tast_case_name
./utName.exe --help
./utName.exe --list
```

运行所有测试用例直接运行编译的可执行程序 `./utName.exe` ，也可以在命令行参
数指定要执行的测试用例名。`--list` 选项可列出所有测试用例名，也就是 `DEF_TAST`
宏的第一个参数，也有变种选项 `--List` 就把用例名及其说明一道打印出来。

利用 `couttast` 库，仅凭以上示例展示的三个半核心宏（`DESC` 宏可有可无算半个）
就能够写出有价值的单元测试了。

本文剩余部分，将介绍 `couttast` 的设计思路与思想。

## COUT 缘起：从 taste 到 test

`couttast` 库有且仅有一个宏用于比较、测试表达式的预期值，就是大写的 `COUT` 。
这个宏的设计有来由。

不妨假设从一个小白程序猿的思维经验考虑，在不知单元测试为何物的情况下，开发了一
个功能函数，它该怎么验证其正确性？譬如说写了一个实现加法的 `Add()` 函数，最直
观的方法就是写段小程序来验证，可能如下：

```cpp
#include <stdio.h>

int Add(int, int);
int main(int argc, char** argv)
{
    int sum = Add(1, 1);
    printf("%d\n", sum);
    return 0;
}
```

这就是传说中经典的 printf 调试、测试大法。编译、运行，如果在屏幕上打印出了 `2`
，那就有点信心这个加法函数写对了。不过好奇猿宝宝转念一想，孤例也许不足采信，正
数与正数相加是对了，正数与负数相加有没 bug 呢，一正一负相加呢？于是很自然地想
到扩充这段测试小程序：

```cpp
int sum = Add(1, 1);
printf("%d\n", sum);
sum = Add(-1, -1);
printf("%d\n", sum);
sum = Add(-1, 2);
printf("%d\n", sum);
```

这次运行将在屏幕打印三个数字：

```
2
-2
1
```

对是好像对了，但是随着打印语句的增加，孤零零的几个数字很快就不清楚是啥意思了，
须得人肉与源代码逐行比对才行。为了增加输出结果的可读性，可以将 `printf` 语句略
加修改：

```cpp
int sum = Add(1, 1);
printf("Add(1, 1) = %d\n", sum);
sum = Add(-1, -1);
printf("Add(-1, -1) = %d\n", sum);
sum = Add(-1, 2);
printf("Add(-1, 2) = %d\n", sum);
```

也可以再精简一下，好像用不上临时变量 `sum` 了：

```cpp
printf("Add(1, 1) = %d\n", Add(1, 1));
printf("Add(-1, -1) = %d\n", Add(-1, -1));
printf("Add(-1, 2) = %d\n", Add(-1, 2));
```

如此将打印：

```
Add(1, 1) = 2
Add(-1, -1) = -2
Add(-1, 2) = 1
```

这就清晰多了，如果对哪种情况的加法出 bug 导致结果不对，也就容易发现。

然后再回顾一下这时的 `printf` 语句写法，像 `Add(1, 1)` 这样的表达式前后重复出
现了，写着有点烦。话说懒惰是程序猿之美德，想想定义一个宏简化一下，不妨就叫
`COUT` 吧：

```cpp
#define COUT(expr) printf("%s = %d\n", #expr, expr)
COUT(Add(1, 1));
COUT(Add(-1, -1));
COUT(Add(-1, 2));
```

打印结果与前例应该一样。

这就与 `couttast` 库中 **单参数** 的 `COUT` 宏功能很接近了。至于为什么叫
`COUT` 而不是 `PRINT` ，那是因为 C++ 的 `std::cout` 是泛型安全的，（几乎）对任
意类型可用相同的写法。不过 `COUT` 在内部实现中其实用的是 `printf` 。

单参数的 `COUT` 可以很自然地打印表达式的值，这在开发初期进行少量的自测是可以的
。但随着测试用例与测试语句的增多，仍靠人肉去查看每条打印语句还是太累了。尤其是
在调完一版，过段时间又想优化代码，重新跑测试用例（回归测试）时，还要复查一条条
基本是已经验证过的结果，那就太傻了。

因此 `couttast` 库很直接地扩展了 **双参数** 的 `COUT` 宏，把结果预期值传给
`COUT` 的第二参数，让程序框架帮你判断该表达式的值是否符合预期——这就隐隐摸到
test 的门槛了。

在 `couttast` 库中，如果 `COUT` 的两个参数是相等的（`==` 操作符），那么除了像
单参数 `COUT` 那样打印第一参数的值外，还在其后额外打个 `[OK]` 标签，否则打个
`[NO]` 标签。剩下的事情就是再想个办法统计在一个函数（测试用例）中，有多少个
`COUT` 语句是 `OK` 的，有多少个是不 `OK` 的，这种事情总不能扔给人肉去数吧。

这是另一个也很重要的话题了，将在下节详述。现在关于 `COUT` 还有一些未尽事项
需先提点一下。

### COUT 的补充说明

`COUT` 读作“see out”，就是查看输出之意。单参数 `COUT` 表达 taste 语义，双参数
`COUT` 表达 test 语义。从程序语义上讲，以下两条语句是等效的：

```cpp
COUT(expr, expect);
COUT(expr == expect, true);
```

即使是自定义类型（`struct` 或 `class`），只要重载了 `==` 操作符与 `<<` 操作符，
也可以用于 `COUT` 。如果不想仅为了利用 `COUT` 进行单元测试而去重载操作符，也可
 COUT 该类型的每个成员（或关键成员）进行比较。

还有别忘了有个特殊的基本类型，浮点数，它是不宜直接用 `==` 比较的。所以 `COUT`
宏又专门为 `double` 类型定义了三参数版本，对此以下两条语句是等效的：

```cpp
COUT(expr, expect, limit);
COUT(abs(expr-expect) <= limit, true);
```

此外，C/C++ 还有个特殊的存在叫指针。对一般类型的指针而言，单参数 `COUT` 打印其
地址，类似 `printf("%p", ptr)` ，双参数 `COUT` 比较两个指针的地址，只有两个指
针指向相同地址，才认为它们是相等的。

然而，C 风格的字符串是一种特殊指针 `const char\*` 。在这种情况下，单参数
`COUT` 打印字符串，双参数 `COUT` 仍比较字符串的指针地址。如果意图比较字符串内
容，请显式转为 C++ 真正的字符串 `std::string` ，或使用 `strcmp()` 函数。例如：

```cpp
const char* psz = "...";
COUT(psz); // 打印字符串
COUT(psz, "..."); // 比较地址
COUT(psz, std::string("...")); // 比较内容
```

## 测试用例管理

当你需要测试的功能点逐渐增多时，譬如说又开发了一个实现减法的函数 `Sub()` ，也
想来测试一番。`COUT` 测试是语句级的，它本身是不介意测试多少功能的，但是你若将
所有 `COUT` 测试语句写在 `main()` 函数中，显然是不妥的。测试代码也是正常程序，
按其功能、目的细分为不同函数是程序猿的基本觉悟。

所以最原始的想法是，为每个被测函数写一个测试函数（用例），然后在 `main()` 中调
用这些测试函数。大约如下：

```cpp
#include "tinytast.hpp"

void test_Add()
{
    COUT(Add(1, 1), 2);
    COUT(Add(-1, -1), -2);
    COUT(Add(-1, 2), 1);
}

void test_Sub()
{
    COUT(Sub(1, 1), 0);
    COUT(Sub(-1, -1), 0);
    COUT(Sub(-1, 2), -3);
}

int main(int argc, char** argv)
{
    test_Add();
    test_Sub();
    return 0;
}
```

这个原始的方案，能跑，也能满足当时的需求。但是有以下几个缺点：

1. 每次增加一个测试函数，得手动在 `main()` 添加调用。这或许算不得大问题，毕竟
   在写正常程序时，某处定义了一个函数，基本必会在另一处施以调用。但是若能实现
   在 `main()` 中自动调用已定义的测试函数，能省事当然更好。
2. 多个测试函数的 `COUT` 输出接续在一起，不容易看出哪个是哪个测试函数的。这与
   之前想用 `COUT` 宏改善打印结果可读性的设计初衷是相悖的。
3. 更关键的是，你如今只能按 `main()` 中写死的顺序调用每个测试函数。如果想快速
   调用中间某个测试函数就会有麻烦。比如说在开发初期，程序有 bug ，在运行第一个
   测试函数时就挂了，后面的测试函数所测的目标程序不管是否有 bug 都没机会得到执
   行验证。
4. 与第 3 点紧密相关的是，如果能指定运行某个测试函数（用例），那就最好有方法获
   知当前编译的测试程序中包含哪些测试用例。如果只能打开源程序一个个找终究是落
   于下乘了。

为了解决以上问题，`couttast` 提供了两个宏，`DEF_TAST` 与 `RUN_TAST` 。

### 定义测试用例

`DEF_TAST` 一般会在不同 `\*.cpp` 源文件中应用多次，就相当于定义一个无参无返回
类型的函数 `void test()` 。一般形式如下：

```
DEF_TAST(test_name, "test description 可以中文描叙")
{
    // test body with COUT
}
```

该宏第一个参数是作为测试用例名的符号，需满足 C++ 标识符规范，并且显然在多个
`DEF_TAST` 之间不能重名。第二个参数是用双引号括起的字符串作为该测试用例的描叙
文字。然后接一对大括号，可认为它是 `void` 函数体。

当然了，在 `couttast` 内部实现中，`DEF_TAST` 显然不能是简单地定义一个 `void`
函数。它实际上定义了一个派生自抽象的测试用例基类，那对大括号 `{}` 括起的代码则
是该测试用例具体类重写的一个虚函数。另外还有个测试用例管理类，会将测试用例名
添加到一个 `map` 中。这个动作是在 `main()` 函数之前自动执行的，利用了 C++ 静态
对象初始化调用构造函数的特性。

### 运行测试用例

`RUN_TAST` 宏可当作函数使用，就是接管 `main()` 函数的两个输入参数的，可以
根据命令行参数运行测试特定用例，或自动运行所有或部分测试用例。因此它一般只需在
`main()` 中被调用一次。

它可以运行的，就是源代码中用 `DEF_TAST` 定义的测试实例。由于内部用 `std::map`
保存，测试用例是按字典序的，其顺序可控可预测。在默认无参数时就按此顺序执行所有
用例，在提供命令行参数时，就按参数提供的顺序执行。参数不仅可以提供测试用例完整
的名字，也可以只提供部分子串，以减少记忆与输入负担。但为了实现（及依赖）简单起
见，不支持复杂的通配符或正则表达式。

此外，命令行参数也支持几个选项。最常用的就是 `--list` 列出所有测试用例名，也是
按字典序打印的。大写的 `--List` 在列出测试用例名的同时，还列出测试用例的描叙说
明，也就是 `DEF_TAST` 的两个参数一起列出。

列出选项 `--list` 不支持额外参数过滤，如果测试用例很多，可以结合管道再用
`grep` 过滤。甚至可以将测试用例列表进行其他处理后再传回测试程序作为其参数，例
如随机序执行：

```bash
./utName.exe $(./utName.exe --list | grep xxx | sort -R)
```

当然了，太复杂的命令行参数及管道组合，或许不合适手动输入，但可灵活动用于某种自
动集成（CICD）的脚本中。

## 运行测试的输出格式

介绍完 `couttast` 测试库的主要功能宏，再来看一来实际运行测试程序（用例）的输出
。例如本文开头的最简示例，如果在 64 位机器较低版本的 `gcc` 编译，它可能报告测
试通过下：

```
## run test_sizoef()
|| sizeof(int) =~? 4
|| sizeof(int) =~? 4 [OK]
|| sizeof(int)==4 =~? ture [OK]
-- string 大小可能依编译器版本实现而不同
|| sizeof(std::string) =~? 8 [OK]
<< [PASS] test_sizoef

## Summary
<< [PASS] 1
<< [FAIL] 0

```

你可以上翻找到原测试用例代码，对照着看各个 `COUT` 语句的输出结果。然而，即使不
看源代码，就凭读这个输出结果，也大概知道测试的是什么吧。

如果在较高版本的 `gcc` 编译，它可能报告测试失败如下：

```
## run test_sizoef()
|| sizeof(int) =~? 4
|| sizeof(int) =~? 4 [OK]
|| sizeof(int)==4 =~? ture [OK]
-- string 大小可能依编译器版本实现而不同
|| sizeof(std::string) =~? 32 [NO]
>> Expect: 8
>> Location: [main.cpp:14](run)
<< [FAIL] 1 @ test_sizoef

## Summary
<< [PASS] 0
<< [FAIL] 1
!! test_sizoef
```

其中最大的不同，当 `COUT` 测试失败时，会另起一行打印期望值，也即源程序中
`COUT` 的第二参数，还会打印源程序文件行号，以便定位分析。当 `COUT` 测试成功时
则不打印源程序位置，因为一般认为对成功的语句不必特别关注。

注意到 `RUN_TAST` 宏的返回值是测试失败的用例个数，该返回值也可以直接作为
`main()` 的返回值，返回操作系统。当作测试全部通过时，返回值为 0 ，测试进程的退
出码也是 0 。由此若写脚本，直接测试退出码就可知测试是否通过，甚至不必费劲诊断
`Summary` 汇总输出。不过当测试有失败用例时，退出码非 0 ，但其数值未必表示失败
用例个数，因为还有退出掩码的作用。

输出每行打印的开头都有两个特殊符号，用以表示不同的输出类型。这一是为了整齐，二
是可以区分被测程序本身可能的输出（一般认为可能是顶格输出）。

`couttast` 没有处理颜色打印。理论上通过转义序列是可以控制打印至终端的文本颜色，
但几个原因没做这项锦上添花的工作。一是懒，想保持库简约，只提供基本功能。二是我
当时使用的终端 MobaXterm ，它正好有套简单的着色功能，可以将 `OK` 显示为绿色，
将 `NO` 显示为红色——那就将就可用了。

还有一个原因，我已经为每种类型的输出用不同的符号引导了，有了基本的可读性。另外
我还是 vim 重度用户，如果真有必要浏览看颜色，还可以将测试的输出结果重定向至文
件再用 vim 打开，并为其设定一种文件类型进行语法着色，例如：

```
$ ./utName.exe | vim -
: set filetype=cout
```

## 其他扩展宏

`DESC` 很简单，可视为能打印输出的注释。当一个测试用例比较长时，可选使用 `DESC`
分段提示解释上下文。但与注释的最大不同是它本质上是 `printf` 语句，支持
`printf` 的格式化参数。例如可在一个循环中用 `DESC` 描叙下第几次循环，用再
`COUT` 输出一些关注的变量、表达式，以此来观察某个算法函数在循环中的表现：

```cpp
for (int i = 0; i < N; ++i)
{
    DESC("loop: %d", i);
    // COUT ...
}
```

与 `printf` 的不同是自动换行，并在行首加上 `-- ` 引号符。笔者在使用原生
`printf` 时经常忘记加上末尾的 `"\n"`。

`COUT` 有个变种宏 `COUTF` ，后缀 `F` 是 `Fail` 缩写，表示只有失败的 `COUT` 语
句（两参数形式）才有输出，忽略测试成功的输出。这种效果通过命令行参数
`--cout=fail` 也能达到，这是为了精简输出的冗余度；`--cout=silent` 的输出将更加
精简，只报告运行哪个测试用例及最后汇总成功失败各多少。

另有个宏变种，`COUT_ASSERT` 与两参数的 `COUT` 宏功能类似，只是当该语句测试失败
时立即终止当前测试用例（`return`）。因为在编写实际测试中，有种情况很常见，当某
个关键变量的值测试失败时，会导致后面连续很多语句测试失败，甚至让整个测试程序崩
溃（例如指针变量为空）——这时提前 `return` 可避免太多的失败输出。

一般情况下，可以先写 `COUT` 测试语句即可，然后在实际运行测试时发现问题后再考虑
改为 `COUT_ASSERT` 或 `COUTF` 。

## 与 GTest 的不同

关于 Google Test 的入门用法可自行搜寻，此不详叙，只简要说明本 `couttast` 库与
之的不同用法提示。

GTest 提供的比较宏非常多，主要有两套，`EXPECT_EQ` 与 `ASSERT_EQ` 系列，字符串
比较又还有另一套变种 ，而且对于相等、大于、小于等判断还各有不同的宏。而
`couttast` 只提供唯一的统一宏 `COUT` 进行比较（或输出），然后是可选的
`COUT_ASSERT` 。只记一个宏名总是更简单的，而且从修改代码的角度讲，在一个单词后
附加 `ASSERT` 比将 `EXPECT` 改为 `ASSERT` 快捷得多。

在定义测试用例上，GTest 使用宏 `TEST` 或 `TEST_F` ，接受两个参数，分别称为
`suit_name` 与 `case_name` ，而测试用例的全名是 `suit_name.case_name`。这就需
要用户额外理解 `test suit` 与 `test case` 这两个概念的区别与联系。从代码角度讲
，`test suit` 可以是用户自定义的一个测试基类，同一个 `suit` 下的 `case` 隐式派
生自这个类，用户可以将每个 `case` 都要写的公共的初始化代码及清理代码写在
`suit` 基类中。

我的观点仍然是从简，不必让用户写测试代码还考虑继承派生的问题。所以 couttast 的
`DEF_TAST` 就是定义独立的测试用例函数 。命名上让用户自主按需规划统一的前缀，第
二个参数就退化为可选用例描叙，不过在实际测试中强烈书写用例描例，尤其是对中国人
来，对标识符英文命名可能是件困难的事。当能认真为 `DEF_TAST` 提供第二参数时，即
使测试用例名使用简单的编号如 `a1, a2, ..., b1, b2, ...` 也未尝不可——而且是
“真编号”，可按编号顺序执行。

但是要注意运行时不只是按确定顺序执行所有测试用例这一种用法，也要考虑只执行其中
一个用例的情况。所以依 couttast 库，尽管用例顺序可控，仍应保证每个用例独立可用。
如果有多个相关测试用例需要相同的初始化代码，可以按正常程序设计抽象出一个函数，
再在每个用例开头调用一下，我觉得这比隐藏在基类中更明确意图。

GTest 运行测试用例的顺序随机，它是有意为之，为了贯彻测试用例应该相互独立的思
想。然而我曾经为此遇到困扰，当我需要指定两个测试用例按先后顺序执行时很麻烦。因
为实际工程中可能（经常）出现这种情况，一键运行所有测试用例时，其中会有一个或几
个失败，但单独运行那个用例时又是成功的。排查这类问题有时比较费劲，应该是某两个
测试用例互相影响了，但它未必是被测代码的 bug ，也可能是测试代码的 bug 。这时就
需要猜测是哪两个用例有相互影响，挑出来顺序执行试图重现再解决。这时，按
couttast 的有序可控做法就更容易排查了。而当真需要随机打乱时，也是有办法的（命
令行管道工具链）。

对于筛选部分测试用例的方案，GTest 采用的 `--gtest_filter` 选项参数也是比较复杂
的。而 couttast 的简单地认为每个命令行参数就是测试用例名（或名字子串），代价是
不支持其他复杂的选项，因为我认为作为测试程序，最重要最常用的命令行参数就是指定
要执行哪个（哪些）测试用例。

当然了，GTest 功能是非常丰富完备的。比如它还可以测试一个函数是否抛出异常，
couttast 没特别考虑这事，要达到等效功能会比较迂回。所以，已经熟练并习惯使用
GTest 作单元测试的，仅管坚持用。我造 couttast 轮子，只是提供了另一种入门简单的
选择，并且试图解决了自己曾经使用 GTest 的一些痛点。

## couttast 特殊用法

### diff 大法的回归测试

用 couttast 写的单元测试程序，可以并鼓励将输出结果（重定向）保存为文件。当被测
程序修改之后而测试程序没有修改时，前后两次的输出结果应该完全一样。于是可以用
`diff` 来比较两次的输出，如果没有变化即意味着被测程序的修改没有影响原功能。这
是比所有测试用例都成功通过更强的判据，尤其是当测试程序中用到了较多的单参数的
`COUT` 语句是，因为用这种方法可以变相地把单参数 `COUT` 也纳入通过与否的判据。
如：

```
./utName.exe > utName.bout

# 被测程序修改或优化过后 ...
./utName.exe > utName.cout
diff utName.cout utName.bout
# 或者可视化查看差异
vimdiff utName.cout utName.bout
```

当然，这种办法有个小小限制，不应在测试程序中 `COUT` 指针变量的地址，因为指针地
址在每次启动进程时可能被分配置不同的绝对地址。一般需求是比较指针是否相等。另外
，这也是 `COUT` 成功时不打印文件行号的一个考量，这可以保证已验证通过的测试程序
，在仅仅添加空行、注释与调整用例在源文件的组织次序等修改情况下不影响输出结果。

### 定制 main 移动命令行参数

如前所述，用 couttast 写测试程序，在一般情况下 `main()` 函数只要转调
`RUN_TAST` 即可。当有特殊需求时，完全可以自己灵活处理，在 `main()` 中处理一部
分命令行参数，将余下参数再传给 `RUN_TAST` 。例如，可以假定测试程序必须要有一个
`.ini` 配置文件来启动：

```cpp
const char* g_pIniFile = nullptr;
int main(int argc, char** argv)
{
    g_pIniFile = argv[1];
    // todo with config file.ini
    return RUN_TAST(argc-1, argv+1);
}
```

这表示将第一个命令行参数固定为配置文件参数，剩余的才是测试用例参数。编译出的程
序用法形如：

```bash
./utName.exe config.ini [--list | test_case_names]
```

此外，`RUN_TAST` 也接收 `std::vector<std::string>&` 作为参数。所以用户也可以在
`main()` 中自由处理所有命令行参数，再重组参数转调 `RUN_TAST` 。

### 伪装成带子命令的工具集

测试程序本质上也是个命令行程序，在启动方式上与常规命令行工具没大区别。就比如用
couttast 写就的测试程序，它的显著特点不外乎是可以从命令行指定执行不同的测试用
例。如果某个“测试用例”函数，它的主要工作不是用（双参数）`COUT` 进行测试判定，
而是利用其代码对外部产生的副作用，或仅仅用 `COUT` 查看下结果，那也就可当作一个
命令行工具来使用了，而且不同“测试用例”还可提供不同的功能。例如，或许可以这么用
测试程序：

```bash
./utName.exe tool1
./utName.exe tool2 > /dev/null
```

这在外观上就与那些有子命令模式的工具很相像了，就如 `git` 或 `docker` 这些。只
不过按 couttast 原本提供的功能，在单元测试用例名参数后不能再为它提供参数了。若
有这需要，就可按上节所述在 `main()` 中先预处理参数，再调用 `RUN_TAST` ，或简单
点约定第一个参数是配置文件，或者少量配置也可以通过环境变量传递。若为此目的启动
“测试程序”，也可以另外取一个程序名，或建立软链接，或简单封装一个 bash 启动脚
本，以更明确表明其意图。

如果将这种利用副作用的工具性测试用例与正常的测试用例混合在一个程序编译的话，可
考虑在正常测试流程中忽略（或提早结束）这类测试用例，至少确保它不至报告失败影响
整体测试的结论。此外，运行工具性测试用例时也会按原框架打印测试报告，但可将标准
输出重定向到 `/dev/null` 忽略之。

至于有没有合理性将工具性子程序与单元测试用例混在一起编译，则是另一番讨论了。考
虑单元测试的常规流程，是设计已知的输入，比较预设的输出进行判断。而常规命令行工
具，是接收某种未知的输入，以求有用的输出。但其内部如何调用被测程序库的接口从输
入计算输出的方式，大体上是相通的，可互为印证。故而写在一起也不见得多么离经叛道
，或许还可复用一些代码设计呢。

举个比较有野心（但不太切实际）的例子，假如也写了一个 libcurl.so 这样的 http 客
户端网络库，再用 couttast 或其他单元测试框架为其写个测试程序，包含一系列的单元
测试用例，其中有个特殊的“单元测试用例”，表现就如 `curl` 命令行工具那样工作。
如果在日常使用 `curl` 中都自感良好，没发现什么问题，岂不也从另一个角度验证了
libcurl.so 的正确性与可用性？

## 结语

本文介绍了自研 C++ head-only 的轻量级单元测试框架， `couttast` 。展示了其基本
用法及特殊用法。也从单元测试的基本需求出发，详细阐释了关键宏及关键功能的设计思路，
并在一些方面以著名的 `gTest` 作为参照进行比较与论述。

笔者在后续的开发维护中，可能会给 `couttast` 添加一些额外功能，但仍坚持用一个简
单的头文件覆盖单元测试领域尽可能多的功能。