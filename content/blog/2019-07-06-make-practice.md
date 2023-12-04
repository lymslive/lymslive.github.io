+++
title = "浅谈 GNU Make 构建项目实践"
[taxonomies]
categories = ["工具使用"]
tags = ["make"]
+++
<!-- # 浅谈 GNU Make 构建项目实践 -->

## 摘要

本文简明地介绍 `make` 的基础原理，并组合实际项目经验，由浅入深讨论了一种实用
的 `makefile` 通用规则与模板的编写方案。对其中涉及的语法功能技巧择要阐述，希望
有助于初学者理解。
<!-- more -->

## make 原理简介

`make` 是 linux/unix 系统下的一款工具，就如同 `ls/cp/find/grep` 这类程序一样属
于基础、通用且经典的设施。其基本原理是根据目标文件与依赖文件的（时间戳）新旧关
系，如果依赖文件更新了（目标文件更旧了），就执行指定的一系列操作。因此它需要这
两部分关键信息，或按 make 的术语叫“规则”：

* 哪个目标文件，会依赖哪些目标文件
* 满足条件后要执行什么样的操作

用户需要将这些规则写入配置文件，以指示 `make` 的运行，习惯上取名为 `makefile`
或 `Makefie` ，也可取为其他名字，那就需要额外加上 `-f` 选项，如：

```bash
make
make -f filename_other_than_makefile
```

一个简单的 `makefile` 规则内容如下：

```make
oldfile: newfile
	echo Hello World
    echo 'cp newfile oldfile' >> make.log
    cp newfile oldfile
```

这里假设 `makefile` 文件所在目录中另有两个文件名为 `oldfile` 与 `newfile` ，只
要在该目录下执行 `make` 命令，`make` 程序就会比较这两个文件的时间戳，如果
`newfile` 更新了，就会将其复制到 `oldfile` （作为备份之用？），如果没有更新就
不会有任何操作。

可以配置多条操作，凡能在 shell 命令行执行的操作都行。每操作配置需要缩进一个
`TAB` 键，目标与依赖文件不能缩进，用一个 `:` 分隔。这就是 `makefile` 文件的基
本格式要求，当然还有其他许多语法细节，诸如变量引用、变量替换函数、目标依赖链、
隐式规则等，那就不是本文所能细说的了，请参考其他入门教程以及官方手册。

## 简单 C 程序的构建

虽然可以花式操作用 `make` 做些奇怪的事，但 `make` 的主要用途是构建 C/C++ 程序
。一般地，从 C/C++ 源文件到最终可执行程序要经过预处理、编译、链接等多步流程，
`make` 就是为简化这种构建流程而诞生的。

### 单文件程序

先看个最简单的情况，假设只有一个 `main.c` 文件，那么 `makefile` 可如下写法：

```make
main.exe: main.o
	gcc -o main.exe main.o

main.o: main.c
	gcc -c -o main.o main.c
```

这规则文件表明，`main.exe` 文件依赖 `main.o` ，而 `main.o` 又依赖 `main.c` 。
于是若执行 `make` 命令，就相当于依次执行如下两条命令：

```bash
gcc -c -o main.o main.c
gcc -o main.exe main.o
```

结果会生成 `main.exe` 可执行文件，当然在 linux 系统下可执行文件不须也不推荐加
上 `.exe` 后缀名，此只为说明方便起见。

另外，`gcc` 编译器有很多默认行为，可直接执行 `gcc -c main.c` 生成默认 `main.o`
的目标文件，不必加 `-o` 选项。甚至直接 `gcc main.c` 一步到位，自动预处理、编译
、链接成可执行文件，不过若无 `-o` 指定输出文件名，默认生成的是 `a.out` 。

正因为构建 C/C++ 程序是 `make` 的拿手好戏，它专门为此默认了一些隐式规则，以简化
`makefile` 的编写，比如 `*.o` 目标文件的依赖与生成规则就可以省略。简化为一条规
则：

```make
main.exe: main.o
	gcc -o main.exe main.o
```

### 多文件程序

当然对于单文件程序的编绎，没必要写 `makefie` ，直接 `gcc -o main.exe main.c`
一条命令解决问题。但假如有多个文件，除 `main.c` 外，还有两个额外的辅助源文件
`util1.c` `util2.c` （及相应的 `.h` 头文件）。这时直接一条命令虽然也能完成编译
链接工作：

```bash
gcc -o main.exe *.c
```

但有个问题，只要改了一个源文件，就要重新编译所有源文件。所以 `make` 就有用了，
可写个 `makefile` 如下：

```make
TARGET = main.exe
ALL_OBJ = main.o util1.o util2.o

CFLAGS += -Wall 

$(TARGET) : $(ALL_OBJ)
	gcc $(CFLAGS) -o $(TARGET) $(ALL_OBJ)
```

这里用到了变量，语法类似 shell ，不过引用变量要加括号 `$(VAR_NAME)` 。也省略了
从 `.c` 到 `.o` 的编译规则，但按 `make` 的隐式规则，也会自动编译生成三个 `.o`
中间文件。如果后来修改了一个 `.c` 文件，则 `make` 只会重新编译一个 `.o` 文件，
另外两个目标不用重新编译，然后将新编译 `.o` 文件与原来无改动的 `.o` 文件一起链
接生成新的 `main.exe` 最终可执行文件。

`make` 命令可以加参数的，参数就是所用 `makefile` 内定义的目标（文件）名，默认
是文件内定义的第一个目标。假如在开发中改过 `util1.c` 文件，只是暂时想
检查一下语法有没错误，可以明确提供目标参数：

```bash
make util1.o
```

这里的 `util1.o` 能由隐式规则生成，故而也是可用的。这比直接在命令行写
`gcc -c util1.c` 的优势是可在 `makefile` 定义一系列编译选项，避免记不住或每次
输入麻烦的问题。

### 头文件包含依赖处理

前述的三文件例程，还漏了重要一点，没加入头文件依赖。最终可执行 `main.exe` 只依
赖几个 `.o` 文件，而每个 `.o` 文件只依赖对应的 `.c` 文件，在上个 `makefile` 规
则中完全没有 `.h` 文件什么事儿。于是如果改动了某个头文件，再执行 `make` 是没有
作何故事发生的，因为没哪个 `.c` 文件比它的 `.o` 更新，`make` 认为不需要执行任
何操作。这不能甩锅给 `make` ，是我们没把规则需求给 `make` 讲明白。

加入头文件 `makefile` 可修改如下：

```make
TARGET = main.exe
ALL_OBJ = main.o util1.o util2.o

CFLAGS += -Wall 

.PHONY: all
all: $(TARGET)

$(TARGET) : $(ALL_OBJ)
	gcc $(CFLAGS) -o $(TARGET) $(ALL_OBJ)

util1.o: util1.c util1.h
	gcc $(CFLAGS) -o $@ $^

util2.o: util2.c util2.h util1.h
	gcc $(CFLAGS) -o $@ $^

main.o: main.c util1.h util2.h
	gcc $(CFLAGS) -o $@ $^
```

将每个 `.o` 目标文件所依赖的源文件及其头文件明确列出，注意一个源文件经常是会交
叉包含其他头文件的。这些规则添加在 `makefile` 末尾没有问题，因为 `make` 会先读
入整个 `makefile` 文件，扫描解析后定出依赖链，定出变量的最终值。再顺便解释几个
新元素：

* `$@` 在规则下的操作命令中代表目标文件
* `$^` 在规则下的操作命令中代表第一个依赖文件，这有利于简化书写及规则通用化
* `.PHONY: all` 明确指出 `all` 为伪规则，否则当目录中正好有个名为 `all` 的文件
  时，会触发 `make` 去读它的时间戳。习惯上常将 `all` 当作第一个默认目标。

这虽然正确地解决了问题，但很容易想到一旦程序源文件多起来，如此手动地指出每个目
标文件所依赖的头文件太繁琐易错了。`make` 不会这么愚蠢，肯定有更机智的办法。这
就需要预处理器的功能了，之前一直忽略了这个流程，也因为平时少用到。

预处理器 `cpp` （这是英文缩写，与 C++ 的后缀名正好犯冲，但完全是两个东西）一个
重要功能是展开宏，生成真正的源文件供给编译器。如果看不懂复杂宏技巧，可将 `cpp`
处理的中间文件保存下来分析，不过这是另一个话题了。但是 `cpp` 预处理时肯定要读
取每个被包含的头文件，它肯定能掌握被处理源文件所需依赖哪些头文件信息，只要将这
部分信息保存下来，就能为 `make` 所用了。

通过手册 `man cpp` 或搜索教程，就能找到使用 `-M` 选项正是输出目标文件依赖规则
的功能。例如 `cpp -M main.c` 可能是如下输出：

```
main.o: main.c util1.h uti2.h \
 /usr/include/stdio.h \
# ... 以下省略更多
```

显然，这与我们手写的目标文件依赖规则很像，只是可能更长得多，因为它必然还依赖某
些标准库头文件。如果一行显示不全，可能分行显示，但需要在上行开尾加 `\` 转义掉
换行符，使其逻辑上等效于写在一长行中。

如果用 `-MM` 选项代替 `-M` 选项，则不会输出那些在 `/usr/include/` 等标准目录下
的依赖头文件，这更适合于 `make` 使用。毕竟正常开发者不会也无权限去修改标准库的
头文件吧。此外，`cpp` 是在运行在标准输入与标准输出的，如果想保存为文件得重定向
。

现在，我们可以祭出 `cpp` 来优化 `makefile` 的编写方式了：

```make
TARGET = main.exe
SRC_DIR = .
ALL_SRC = $(wildcard $(SRC_DIR:%=%/*.c)) # 自动获取所有 .c 源文件
ALL_OBJ = $(ALL_SRC:%.c=%.o) # 替换 .c 后缀为 .o 得到所有目标文件列表
ALL_DEP = $(ALL_SRC:%.c=%.d) # 得到一系列 .d 依赖文件列表

CFLAGS += -Wall 

.PHONY: all clean
all: $(TARGET)
clean:
	rm -rf $(TARGET) $(ALL_OBJ) $(ALL_DEP)

$(TARGET) : $(ALL_OBJ)
	gcc $(CFLAGS) -o $(TARGET) $(ALL_OBJ)

$(ALL_DEP): %.d : %.c # 用 cpp 处理每个 .c ，保存 .d 依赖文件
	cpp -MM $< > $@

ifneq ($(MAKECMDGOALS), clean)
-include $(ALL_DEP) # 将所有 .d 文件包含进来，实现目标文件的依赖规则
endif
```

关键新行已添加注释（`makefile` 注释语法与 `bash` 一样使用 `#`）。我们先用
`wildcard` 函数提取 `SRC_DIR` 源码目录（这里是当前目录 `.`）下所有 `.c` 文件，
这就不限于之前的三个源文件的小程序了。得到 `ALL_SRC` 是一长串以空格分隔的
源文件名列表，然后用变量替换功能转换一长串 `.o` 目标文件名存为 `ALL_OBJ` ，以
及 `ALL_DEP` 一长串 `.d` 文件名，不妨称之为依赖文件。

每个依赖文件用 `cpp -MM` 命令生成，并且可只用一条通配规则 `%.d : %.c` 生成，避
免手动书写重复的相似规则。最后关键是，生成的 `.d` 文件可用 `include` 命令包含
进来，因为每个 `.d` 文件也是符合 `makefile` 语法规则的。`-include` 加个 `-` 前
缀是忽略错误之义，写在 `ifneq` 与 `endif` 之间只是锦上添花并非必须，这可避免在
执行 `make clean` 时把 `.d` 文件包含进来，因为那没意义了。

注意这里还是利用了生成目标文件的隐式规则。包含进来的 `.d` 文件，声明了类似
`%.o : %.c {with list of *.h}` 的依赖规则，但没有写明操作命令，`make` 有隐式规
则会从相同目录下的 `.c` 文件生成所需的 `.o` ，也能自动添加约定的 `CFLAGS` 选项
参数。其实也可显式添加一条通配规则，并未增加太多复杂度：

```make
$(ALL_OBJ): %.o : %.c
	gcc -c $(CFLAGS) -o $@ $<
```

## 复杂项目工程的构建

在上节讨论中，我们给出了一个几乎通用的 `makefile` 文件，内中完全没有写死哪个源
文件或目标文件。但它还是有些问题。首先它仍假设所有源文件在一个目录，需要进入那
个目录使之成为当前目录执行 `make` ，而且所有目标文件与依赖文件这些临时的中间文
件也都与源文件混在一起，污染视听。另一个小问题是它只用于构建 C 程序，而现在
C++ 程序似乎更主流，当然这改起来也简便。

### 典型实际工程项目的目录结构

在实践中，一个工程项目的目录结构其实是个设计问题，不一定有严格标准。但多少也有
些相似的共识习惯，至少肯定是有多个目录的层次结构，不会是单目录一把梭。

例如一个大项目可能是如下组织：

```
/path/to/project/root/macro_service
    doc/
    config/
    src/
        model1/
        model2/
        ......
    include/
    lib/
    obj/
```

主要就是将源代码放在 `src/` 目录下，并且由于源文件数量太多，还应该分模块子目录
存放。与 `src/` 平级可能有些辅助目录，很可能希望将编译中间文件从 `src/` 分离，
统一放在平级的 `obj/` 目录下。

现在互联网据说较为流行微服务。那么它一个项目要编译的就不只一个大程序了，可能是
一组数量较多的可执行程序。也许其目录结构会是这样：

```
/path/to/project/root
    common1/
    util2/
    micro_service/
        serviceA/
        serviceB/
            1.cpp 1.hpp 2.cpp 2.hpp ...
            may_also_subdir/
        serviceC/
    third1/
    third2/
    tools/
        toolA/
        toolB/
    unit-tets/
```

服务数量众多，但每个服务的代码量不甚巨大，也许几个或几十文件就可实现功能。也因
为服务众多，必然会有些公用代码，正常想法会提到外面共用。如此很可能每个服务用自
己的单个目录就能存放所有源文件了，不必要再加一层 `src/` ，否则太多 `src/` 也奇
怪。除了生产服务，可能还有命令行工具程序及单元测试，理论上这或许可用脚本完成任
务，但既然是熟悉 C/C++ 的团队，又在有较完善的公共库的情况下，直接用 C/C++ 也更
和谐。此外可能直接下载使用一些第三方开源库，不想安装在系统中，直接扔在项目中了
。如果三方库或自研库数量多起来，或许还要继续分别加层目录管理。

本文拟基于后一种微服务集的工程目录结构假设，说明以 `make` 构建项目的流程模式。
因为如果一个微服务内的源码也有再分子目录的话，那就与一个大服务的编译过程没有质
的区别了，只是量（编译时间）的差异了。

### makefile 模板与 include

现在需求是在许多子目录中编译出可执行程序。显然在每个这样的源码目录中写一份完整
的 `makefile` 略显笨拙，尽管可以将上节所述的通用 `makefile` 复制到每个需要编译
的目录中。更好的办法是将那个通用的 `makefile` 放在项目根目录中，比如取名为
`root.mk` ，然后在每个编译目录中的 `makefile` 中 `include` 它。

然后再梳理一下，在每个具体的 `makefile` 中，还需要提供哪些额外信息呢。将这些信
息以变量配置的方式写下来。大体框架可能如下：

```make
TYPE		= exe
TARGET		= a.out
INSTALL_DIR	= /path/to/install/
SRC_DIR		= . sub1 sub2
OBJ_DIR		=
INC_DIR		= 
LIB_DIR		= 
SYS_LIB		=
LIB_DEPENDS	= 
OBJ_DEPENDS	= 
EXTRA_CFLAGS	=
EXTRA_CXXFLAGS	=
include ../../root.mk
```

主要是用变量定义一些必要信息，且大部分若接受默认值的话可留为空串：

* 最终目录目标 `TARGET` 及其类型 `TYPE` ，比如是可执行程序还是打包的静态链接库
  或动态链接库等。
* 列出源码目录 `SRC_DIR` ，相对这个 `makefile` 文件的路径，
  如果有太多子目及深层子目录，可利用相关功能函数生成目录列表串。
* 编译中间文件的存放目录 `OBJ_DIR` ，希望与 `SRC_DIR` 分开。
* 头文件所在目录（非标准目录），将来要用于 `-I` 选项的。
* 所需用到的链接库与链接目录，将来用于 `-L` 与 `-l` 选项的。
* 所需本项目内预编译的其他公共依赖库 `OBJ_DEPENDS` 。
* 额外的 `gcc` 或 `g++` 编译选项。
* 最后是以相对路径引用项目根目录的通用规则文件。

### 通用 makefile 规则再优化

通用 `root.mk` 大约可分为几部分。首先是处理变量：

```make
TARGET  := $(strip $(TARGET))
TYPE    := $(strip $(TYPE))
INSTALL_DIR := $(strip $(INSTALL_DIR))
CC       = gcc
CXX      = g++
CPP      = cpp
CFLAGS   += $(C_INC)
CXXFLAGS += $(C_INC)
CFLAGS	 += $(EXTRA_CFLAGS)
CXXFLAGS += $(EXTRA_CXXFLAGS)

CFLAGS	 += -Wall
CXXFLAGS += -Wall
EXTRA_CXXFLAGS += -std=c++11

SRC_DIR  += .
OBJ_DIR  ?= obj/
INC_DIR  += $(COMM_INC_DIR)
INC_DIR  += /usr/include/mysql/ /usr/include/ ../ ./ ../../
SYS_LIB  += libpthread libcurl libmysqlclient # and more needed
LIB_DIR  += $(COMM_LIB_DIR)
LIB_DIR  += /usr/lib/ /usr/lib64/ /usr/lib64/mysql/
C_INC    = $(INC_DIR:%=-I%)
C_LIB    = $(LIB_DIR:%=-L%) $(LIB_DEPENDS:lib%=-l%) $(SYS_LIB:lib%=-l%)
```

这里将编译器名 `gcc` 或 `g++` 也赋给变量，万一以后要改用 `clang` 编译，也只要
改一处。`makefile` 的变量赋值涉及几种等号，简单解释一下。

* `=` 常规的等号，如果右边引用了其他变量，则会递归解析，扫描完全文才能最终确定
  变量值。例如 `CFLAGS` 变量先定义包含了 `C_INC` ，后边 `C_INC` 再由 `INC_DIR`
  定义变换而来。
* `:=` 一次解析当前变量值，等号右的引用的变量后面变化了也不影响左边的变量值。
* `?=` 只有当左边的变量未定义时赋值（空串值也算定义过，不会发生赋值）。例如
  `OBJ_DIR` 默认设为 `obj/` ，但如果（某个具体子目录内的 `makefile` 配置）定义
  了 `OBJ_DIR` 就使用原配置，空值就表示目标文件将与源文件混在同一目录。

这部分主要是变量值转换处理，准备好编译器命令的选项参数等。接下来是编译源文件为
目标文件的规则：

```make
C_SRC   = $(wildcard $(SRC_DIR:%=%/*.c))
CXX_SRC = $(wildcard $(SRC_DIR:%=%/*.cpp))
C_OBJ   = $(C_SRC:%.c=$(OBJ_DIR)%.o)
CXX_OBJ = $(CXX_SRC:%.cpp=$(OBJ_DIR)%.o)
C_DEP   = $(C_SRC:%.c=$(OBJ_DIR)%.d)
CXX_DEP = $(CXX_SRC:%.cpp=$(OBJ_DIR)%.d)

ALL_OBJ = $(C_OBJ) $(CXX_OBJ)
ALL_DEP = $(C_DEP) $(CXX_DEP)

$(C_DEP): $(OBJ_DIR)%.d : %.c
	@mkdir -p $(dir $@)
	$(CPP) $(EXTRA_CFLAGS) $(C_INC) -M $< > $@
    @sed -r -i 's|^(\w+)\.o[ :]*|$(@:.d=.o) : |' $@
$(CXX_DEP): $(OBJ_DIR)%.d : %.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(EXTRA_CXXFLAGS) $(C_INC) -MM $< > $@
	@sed -r -i 's|^(\w+)\.o[ :]*|$(@:.d=.o) : |' $@

$(C_OBJ): $(OBJ_DIR)%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<
$(CXX_OBJ): $(OBJ_DIR)%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o $@ $<

ifneq ($(MAKECMDGOALS), clean)
-include $(ALL_DEP)
endif
```

这里为了同时处理 C 与 C++ 源文件，分别定义了 `C_SRC` 与 `CXX_SRC` 保存所有匹配
的源文件列表，并分别转换获取 `C_OBJ` 与 `CXX_OBJ` ，最后拼接成所有目标文件
`ALL_OBJ` ，同理 `ALL_DEP` 保存所有 `.d` 文件列表。这只是平行地写了两套变量，
增加的一倍代码量属于线性复杂度而已。如果项目中有的 C++ 源文件还会用到 `.cc` 或
`.C` （大写C）后缀名，可考虑再平行写一套类似 `CCC_SRC` `CCC_OBJ` `CCC_DEP` 的
变量名；如果觉得这太冗余，有失美观，可以的话将 C++ 源文件后缀名批量统一改为
`.cpp` 吧。

与上节末的 `makefile` 文件相比，这段代码还将所有 `*.o` 与 `*.d` 中间文件放在
`OBJ_DIR` 目录下（个人觉得不必要再将中间文件分门别类放入不同的目录，所以只定义
了一个 `OBJ_DRI`）。但这会遭遇另一个问题，`cpp -MM` 的输出，不会包含目标文件
`.o` 的目录信息，只取基础文件名。这里的解决办法是用 `sed` 再处理生成的 `.d`
文件。

`sed` 的 `-r` 选项表示使用扩展的正则表达式，影响后面的正则表达式写法。常规正则
替换写法是 `s/exp/rep/` ，不过处理文件名时很可能用到 `/` ，于是改用 `s|reg|rep|` 
竖线分隔符更方便些。`-i` 表示原位修改文件，开始不确定时可以多写一步中间文件尝
试下：

```make
$(CXX_DEP): $(OBJ_DIR)%.d : %.cpp
	mkdir -p $(dir $@)
	$(CPP) $(EXTRA_CXXFLAGS) $(C_INC) -MM $< > $@.tmp
	sed -r 's|^(\w+)\.o[ :]*|$(@:.d=.o) : |' < $@.tmp > $@
	# rm $@.tmp
```

在 `makefile` 的规则操作行，前导 `@` 表示不回显执行命令本身。在调教成功后，可
以在 `mkdir -p` 与 `sed -ri` 命令前加 `@` 减少冗余输出。

然后是编译最终目标的部分，根据目标类型编写不同规则：

```make
TARGET_MAKE := $(OBJ_DIR)$(TARGET)
TARGET_INSTALL := $(INSTALL_DIR)$(TARGET)

ifeq ($(TYPE), exe)
$(TARGET_MAKE): $(ALL_DEP) $(ALL_OBJ) $(OBJ_DEPENDS)
	$(CXX) $(C_LIB) $(ALL_OBJ) -Wl,--start-group $(OBJ_DEPENDS) -Wl,--end-group $(LDFLAGS) -o $@
install: all
	cp -f $(TARGET_MAKE) $(TARGET_INSTALL)
endif

ifeq ($(TYPE), liba)
$(TARGET_MAKE):$(ALL_DEP) $(ALL_OBJ) $(OBJ_DEPENDS)
	$(AR) r $(TARGET_MAKE) $(ALL_OBJ)
install:all
	cp -f $(TARGET_MAKE) $(TARGET_INSTALL)
endif

ifeq ($(TYPE), libso)
SO_DEPENDS = $(LIB_DEPENDS:%=$(COMM_LIB_DIR)/%.so)
$(TARGET_MAKE): $(ALL_DEP) $(ALL_OBJ) $(OBJ_DEPENDS) $(SO_DEPENDS) 
	$(CXX) -shared -fPIC $(C_LIB) $(ALL_OBJ) -Wl,--start-group $(OBJ_DEPENDS) -Wl,--end-group $(LDFLAGS) -o $@
install: all
	cp -f $(TARGET_MAKE) $(TARGET_INSTALL)
endif
```

这里将 `make` 出的最终目标文件也暂丢进 `OBJ_DIR` 目录中，`make install` 再拷到
`INSTALL_DIR` 目录中。`makefile` 的 `if/endif` 也有 `else` 分支，但没有类似
`else if` 的语法，且嵌套的 `if/endif` 并不能缩进，所以可读性不佳，故而这里直接
将几个 `if/edif` 平行列出。

在定义目录前缀如 `OBJ_DIR` `INSTALL_DIR` 时，最好规则带上 `/` 后缀，然后在使用
处获取全路径时只有直拼接起来，不必额外多写个 `/` 。这样的好处是兼容空串目录前
缀，否则使用 `$(DIR)/$(FILE_NAME)` 时，若目录前缀为空就变成了系统的根目录 `/`
了，这经常是不期望的。

最后一些非关键部分。我们在这个 `makefile` 中进行了大量的变量处理，开发中有时不
确定这些变量定义得对不对，那可以加个伪目标，把其中关键变量名都打印出来，可利于
调试：

```make
.PHONY: all clean install echo

all:$(TARGET_MAKE)

clean:
	rm -rf $(ALL_OBJ) $(ALL_DEP) $(TARGET_MAKE)

echo:
    @echo $(ALL_SRC)
    @echo mkdir -p $(OBJ_DIR) $(INSTALL_DIR)
	@echo $(CPP) $(EXTRA_CXXFLAGS) $(C_INC)
	@echo $(ALL_DEP)
	@echo $(CXX) -c $(CXXFLAGS) $(EXTRA_CXXFLAGS)
	@echo $(ALL_OBJ)
	@echo $(CXX) $(C_LIB) $(ALL_OBJ) -Wl,--start-group $(OBJ_DEPENDS) -Wl,--end-group $(LDFLAGS) -o $(TARGET_MAKE)
	@echo $(TARGET) $(TARGET_MAKE) $(TARGET_INSTALL)
```

伪目标 `echo` 就是把上面搜索到的源文件，转换的目标文件及依赖文件，用到的编译命令
选项等复制下来，一键打印出来瞧一瞧，大概瞄一眼。`make -n` 选项也只打印将要执行
的操作而不实际执行，但如果目标已经是新的，不需要操作也不就不会有命令打印出来。

最后提醒一下，`all:` 目标定义最好移到 `makefile` 前面，使之成为第一个目标，这
样就可以按默认参数只敲入 `make` 而无需敲入 `make all` 了。

### 工程多目录自动编译

如果是开发中修改了项目某个或某几个子目录，只要进入相应的目录执行下 `make` 就好
了。但如果是重新拉一份项目代码，第一次从头编译时，手动进入每个目录进行 `make`
那也是繁琐的。这就引出一个需求，从根目录自动进入各个含 `makefile` 的子目录进行
`make` ，可以写个脚本做这件事，不过一条 `find` 命令也能实现基本需求：

```
find . -name 'makefile' -execdir make clean \;
find . -name 'makefile' -execdir make \;
```

如果不想记这串 `find` 命令，可以在项目根目录也写个 `makefile` ，将这个命令放在
第一个伪目录下面：

```make
.PHONY: all
all:
	find . -name 'makefile' -execdir make $(MAKECMDGOALS) \;
```

不过 `find` 对于大项目递归搜索可能比较耗时，且会搜索许多不必要的目录。如果对此
介意或想体验一下 `make` 的其他技巧，也可以使用纯 `make` 的方式来定制进入哪些子
目录。思路与前面所述的 `makefile` 配置模板与通用规则 `root.mk` 类似，不过要写
另一套用于中间目录的 `makefile` 模板与规则。

先在项目根目录与 `root.mk` 同级处另写一个 `dir.mk` 如下：

```make
.PHONY: all clean install $(TARGETS)
all clean install : $(TARGETS)
$(TARGETS):
	@echo "### make $(MAKECMDGOALS) in $@"
	@$(MAKE) $(MAKECMDGOALS) -C $@ --no-print-directory
	@echo
```

然后在每一个中间目录与根目录，也即不是需要编译源代码的“叶子”目录，写个简单的
`makefile` 如下：

```make
TARGETS = sub1 sub2 sub3
include ../dir.mk
```

只要将该目录下管理的需要编译的子目录明确列出来，赋给变量 `TARGET` ，然后将根目
录的 `dir.mk` 包含进来，就能实现自动编译往下一层的子目录了。虽然需要在每个中间
目录添加这样一个 `makefile` ，但胜在可精细控制，而且也不算太麻烦吧，毕竟在一个
项目中做出增加一层目录的决策本身，比写这个简单 `makefile` 的工作量复杂得多。

## 在 Vim 中利用 makefile 快捷编译

Vimer 党也许会直接在 linux 下编辑代码，所以也希望能利用 `make` 从 vim 直接调起
编译。这里简单说些思路。

如果只是编译单文件小程序，可以直接定义如下快捷键映射：

```vim
nnoremap <F9> :!gcc %<CR>
```

在编辑一个项目时，如果 vim 当前目录有定义 `makefile` ，也可以直接使用 `:make`
编译整个工程项目，且编译错误会呈现在 quickfix 窗口中。如果只是要编译当前编辑中
的源文件，看下有没编译错误之用，则可以输入 `:make currnet-filename.o` 。不过如
果 `makefile` 规则中将 `.o` 文件放到其他目录中，也得输入路径信息，利用 vim 命令
行参数补全或可节省些输入。如果想定义快捷键自动输入当前源文件关联的目标文件，那
可能是有点复杂。例如：

```vim
nnoremap <F9> :make <C-R>='obj/' . expand('%:r') . '.o'<CR><CR>
nnoermap <S-F9> :make<CR>
```

这里假设按上节的默认规则，目标文件存在当前目录的 `obj/` 子目录下。因为这种路径
关系依赖于项目的 `makefile` 书写，所以这个快捷键映射最好也是作为项目相关的配置
，而不要写在全局的 `.vimrc` 。比如若用 vim 原生的 Session.vim 保存会话，则可将
项目配置写在 `Sessionx.vim` 脚本中。

还有个问题，如果 vim 未设置 `autochir` ，则 vim 当前目录很可能与所编辑的源文件
不在一起，或者说只要当前目录没有 `makefile` 文件，那 `:make` 命令还得加上 `-C`
选项先转到含 `makfie` 的目录中去。至于如何找到相关联的 `makefile` 文件所在目录
，可能需要了解 VimL 脚本才能较好地解决这问题，这就不展开了。

## 总结

`make` 原理极简，但 `makefile` 细节不少。当然我们不一定要掌握 `make` 的所有细
节，需要时查询相关手册。本文讨论打造的 `makefile` 文件，大致可用于大中型 C/C++
项目的构建了，或许还需根据实际情况调整一些编译选项。`make` 的实际使用基础，还
是需要对 C/C++ 程序的编译流程与所用编译器提供的选项功能熟悉。不过在掌握更多 
`makefile` 语法后，可以更有效地优化与控制编译过程。

尽管现在也有许多自动构建工具了，比如 `CMake` ，能自动生成 `makefile` 。但由通
用工具生成的 `makefile` 可读性太复杂。若能理解 `make` 原理并手写 `makefile` ，
那会更有成就感与控制感。而且使用 `CMake` 不也得学习一下 `CMakeList.txt` 的写法
么。如果没有跨平台编译的需求，只在 `linux` 下使用的，根据自己的项目定制一份
`makefile` 模板，那后续使用也是相当简单方便的。

## 参考链接

* http://ruanyifeng.com/blog/2015/02/make.html 阮一峰写的 Make 教程

* https://blog.codingnow.com/2009/03/gnu_make_vpath.html 云风的博客，其中谈到
`vpath` 与 `cpp -MT` 选项，只是我具体没试出怎么用好。

* https://blog.csdn.net/yufei_email/article/details/78575637 另一篇 CSDN 的博客
，也用的 `sed` 大法处理 `cpp -M` 的头文件依赖关系。

* https://www.gnu.org/software/make/manual/html_node/index.html 官方手册，忘记了
变量与函数用法可以查查。
