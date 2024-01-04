+++
title = "C++ 奇淫技巧之耦断丝连：临时 C 字符串"
[taxonomies]
categories = ["程序设计"]
tags = ["C++"]
+++

这里想分享一个许是微不足道 (trivial) 的问题及其解决思路。
<!-- more -->

## 问题及方案

工作背景是一个 C++ 服务项目，采用 printf 风格的日志系统。在一个函数中想打印正
要处理的消息，当然已知消息正文是采用文本协议（如 json）。传进函数的参数是字符
串指针及长度，因为底层库获取或消费消息时就给出这样的 C 风格的二元组，而不是
C++ 的字符串对象（避免不必要的拷贝）。显然，加一行日志是很简的事情，如：

```c++
void DealMessage(const char* pMsg, size_t nMsgLen)
{
    LOG_DEBUG("to deal message: %s", pMsg);
    // todo the bussiness ...
}
```

问题在于，底层库给出的 `pMsg` 字符串未必能保证在第 `nMsgLen` 字节是以 `\0` 结
尾，虽然文本协议能基本保证在 [0, nMsgLen) 区域是可打印字符。这样，打印 `%s` 就
会超出长度，直至遇到 `\0` ，运气好时不过是在日志中多打印出一些乱码，运气不好就
难说了。

直接的解决方案是从指针长度二元组临时构造出一个 `std::string` 再取其 `c_str()`
方法打印 `%s`，如：

```c++
LOG_DEBUG("to deal message: %s", std::string(pMsg, nMsgLen).c_str());
```

现在的 STL 版本，已能保证 `std::string` 以空字符结尾。据说早期蛮荒时代的 STL
版本标准，`std::string` 却是不保证能空字符结尾的，所以因此及其他诸多原因备受嫌
弃。

这样修改比较膈应的是，人家用指针与长度传参本来是为了性能的原因，结果为了打印日
志这种非核心业务需求构造出一个临时 `std::string` 对象，凭空多了一次拷贝，性价
比很低的呀。即使用其他第三方认为比 `std::string` 更好的字符串实现，甚至就额外
用一个 C 缓冲区，也是无法避免这次拷贝再附加 `\0` 封尾的操作。

## 优化及封装

不过再仔细想一下，其实也没必要额外开缓冲区拷贝字符串，完全可以利用原缓冲区来搞
事。原缓冲区的问题不就是缺个空字符结尾吗？那就临时改出一个空字符，顺利打印日志
后再改回去。如：

```c++
{
    char* buffer = const_cast<char*>(pMsg);
    char save = buffer[nMsgLen];
    buffer[nMsgLen] = '\0';
    LOG_DEBUG("to deal message: %s", pMsg);
    buffer[nMsgLen] = save;
}
// todo the bussiness ...
```

这里多了好几行代码，用 `{}` 包装了一下限制作用域。主要是还要先将 `const char*`
强行转换为 `char*` 才可写。但要注意如果是指向字面量字符串的指针，强行转为可写
指针，是会失败的。但在这个业务场景中，只要是指向堆区的字符串，都是可以强行修改
的。

代码有点多，可以抽出来封装成一个类，把篡改与回滚尾字符的操作分别放在对象的构造
函数与析构函数中：

```c++
class CutString
{
    const char* ptr_ = nullptr;
    size_t len_ = 0;
    char last_ = '\0';

public:
    CutString(const char* ptr, size_t len)
        : ptr_(ptr), len_(len)
    {
        char* buffer = const_cast<char*>(ptr_);
        last_ = buffer[len_];
        buffer[len_] = '\0';
    }

    ~CutString()
    {
        char* buffer = const_cast<char*>(ptr_);
        buffer[len_] = last_;
    }

    const char* c_str() const 
    {
        return ptr_;
    }
};
```

这就能简化使用了：

```c++
void DealMessage(const char* pMsg, size_t nMsgLen)
{
    LOG_DEBUG("to deal message: %s", CutString(pMsg, nMsgLen).c_str());
    // todo the bussiness ...
}
```

也就只是把原来的 `std::string` 改成 `CutString` ，后者的临时对象不涉及字符串拷贝，
经编译器优化内联函数后基本就是零成本抽象。主要是利用了 C++ 类的 RAII
(Resource Acquisition Is Initialization) 特性，让构造函数中切断字符串，析构时
再自动粘回去——所以我称之为耦断丝连的骚操作。

对此还有困惑的朋友，可在上面的构造函数与析构函数中添加打印观察测试一下，如：

```c++
class CutString
{
    CutString(const char* ptr, size_t len)
    {
        // ...
        printf("in CutString, save last: %c\n", last_);
    }
    ~CutString()
    {
        // ...
        printf("in ~CutString, restore last: %c\n", last_);
    }
};

int main()
{
    char* buffer = (char*)malloc(1024);
    for (int i = 0; i < 1024; ++i)
    {
        buffer[i] = i % (127-32) + 32;
    }

    printf("at buffer[16]: %c\n", buffer[16]);
    printf("first buffer[16]: %s\n", CutString(buffer, 16).c_str());
    printf("at buffer[16]: %c\n", buffer[16]);

    free(buffer);
}
```

输出如下：

```
at buffer[16]: 0
in CutString, save last: 0
first buffer[16]:  !"#$%&'()*+,-./
in ~CutString, restore last: 0
at buffer[16]: 0
```

注意 `buffer` 循环存放着可打印 [32, 127) 区间的 ascii 字符，第 16 个字符正好是
数字 `0` ，而不是空字符 `\0` 。

## 结语

关于本文提出的问题，现代 C++ （20 以后）也可以利用 `std::format` 与
`std:: string_view` 解决。不过呢，工程项目的代码是妥协的结果，`printf` 风格的
打印已经广泛使用，一时也难以改过来，何况编译器升级牵涉更广。所以本文提出用临时
切断字符串的方案，利用构造析构的 RAII 来使影响最小化。当然此修改非线程安全，不
过从线程池分发消息到处理函数后，一般也是只有本函数在处理了，故不应在此考虑线程
安全。

## 附录：源代码

[cutstring.cpp](cutstring.cpp) 
