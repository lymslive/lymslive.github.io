+++
title = "漫谈二进制与十六进制在计算机中的运用"
[taxonomies]
categories = ["天马行空"]
tags = ["cs","linux"]
+++

# 漫谈二进制与十六进制在计算机中的运用

众所周知，计算机内部使用二进制，与人们日常使用的十进制大相径庭。可能有些人，包
括一些初学计算机的学生，会对此感到困惑。本文试图用浅显的语言来聊一聊这个话题。

## 进制的选择源于生产实践

其实，计算机是人类发明的，进制也是人类发明，所以计算机使用什么进制，只是人类的
选择。人类发明进制，主要是为了方便指导人类自身的生活及生产活动。在人类历史上，
使用最广泛的进制是十进制，大概人有十根手指，容易以此为凭计数吧。

但是，除了十进制外，人们在特定场合也经常使用其他进制。比如中国古代使用的算盘与
算筹，相当于五进制，那是在十进制为主的情况下的辅进制。现在仍然广泛使用的非十进
制是时间单位，以六十进制为主，十二进制为辅。中国古代的十天干与十二地支的循环也
是构成六十甲子循环。人类对时间的感知源于天体运动，对时间与日历的划分主要以太阳
与（或）月亮的运行规律为基础。人们不对此作十等分，而是十二等分，六十等分，是因
为发现这样更方便，更合理。譬如中国农历的二十四节气，在农业社会对农事生产的指导
是相当有意义的。所以，我们在某个领域选择某种进制，是实践导向的结果，这种进制更
适合这个领域而已。

另外，十六进也不新鲜，中国古代在称量时也用过十六进制，“半斤八两”这词就这么来的。
因为等分其实是最容易实现的，比十等分、十二等分都容易得多。不过只用一刀两半、一分
为二的粒度在很多情况下还是太粗了，所以一般要继续等分，分个三、四次，做成八等分、
十六等分，大家觉得这样的份量更方便使用，满足大部分场合的需求，就定下十六进制，
半斤八两。

## 计算机信息产业选择了二进制

所以，计算机使用二进制，也只是因为它更合适、更简单。首先，二进制是最小的进制，
并不存在一进制，如果一个事物只能承载一种状态，它是无法表示多种信息变化的，至少
需要两种状态，也就是二进制。其次，二进制实现也最简单。用电子元器件的高电平与低
电平就能表示 1 与 0 ，两种状态的区分与辨识是最容易的，容错也高。假设要使用所谓
的三进制，增加一种“不高不低”电平，那会使状态判断的难度剧增，完全得不偿失。这就
是大道至简，二进制足以表达任意变化，任意数据与信息，那就只用二进制就可以了。

当然，任何技术都该以人为本。在计算机之外讨论二进制的数值，经常也需要转为十进制
的“真实”数值。这种转换，只涉及一个基本的简单的数学原理。参考十进制是怎么用多项
式表示一个数的：

```
1985 = 1 * 1000 + 9 * 100 + 8 * 10 + 5 * 1
     = 1 * 10^3 + 9 * 10^2 + 8 * 10^1 + 5 * 10^0
```

然后随手写个 `1101` 的二进制，它就可按类似的公式求出对应的十进制表示：

```
1101 = 1 * 2^3 + 1 * 2^2 + 0 * 2^1 + 1 * 2^0
     = 1 * 8 + 1 * 4 + 0 * 2 + 1 * 1
     = 8 + 4 + 1  = 13
```

如果要反过来，将十进制的 `13` 转为二进制的 `1101` ，那也就是个逆分解过程，不断
除 2 取余的过程，这里不再赘述。

简言之，任意进制的数值，可以用基数与进制数的指数幂多项式来表示，我们写在一起的
各位数字，在数学表达式上就是这个多项式的系数。二进制的基数只有 0 与 1 ，十进制
的基数就是常用的 0-9 这十个数，而十六进制就在 0-9 之后，借用 a-f 这六个字母分
表达到 10 至 15 的基数。

## 从二进制到十六进制

除二进制外，在计算机学科还常用八进制与十六进制。但后两者不算独立的进制，它们本
质上也是二进制，或者说是基于二进制的辅进制，目的是为了更方便地表达或书写二进制
数值。

就如本文开头提及的算筹（与算盘）的五进制，它也不是独立的进制，只是十进制的辅进
制。一根算筹就像长条形的筷子，竖着放一根表示 1 ，放两根表示 2 …… 放四根表示 4
，但到了表示 5 ，它就不再是并排竖放五根了，而是横着放一根表示 5 。为啥这样规定
呢？因为一直并排着放下去，既费材料，也费空间，还对人肉识别不友好，比如并排放八
根或九根，你不能一眼很快看出具体是八根还是九根。所以就引入了“逢五转一”的辅进制，
但更根本的还是“逢十进一”的十进制。在算筹系统中，如果约定个位数用竖筹表示 1 ，
横筹表示 5 ，那么在十位数就反过来，用横筹表示 1 （也就是 10），竖筹表示 5 （也
就是 50），如此轮换，进一步增加数字的识别度。当然，这些约定，就是具体的技术与
工程问题了。现在我们不需要算筹了，所以也就不需要五进制。

但二进制仍需要八进制或十六进制的辅助，也是基于类似的表达原因。如果把写在纸上
（或打印在屏幕上）的二进制数值，那长串的 1 想象为并排竖放的算筹，就能发现问题
了，它很难被人眼识别。所以为了增加识别度，我们会将它分成三个一组，或四个一组，
并且用更丰富的数字符号（而不仅有 1 与 0）来表示每个分组部分。

其实在表达常规的十进制大数时，人们也经常会采用分组的办法。比如在西方国家，习惯
于每三位一组，有时会显式用逗号（下标）或单引号（上标）分隔，依次表示 thousand
（千）、million （百万）等。而在中国，更习惯按每四位分组，依次表示为万、亿等。

二进制的问题在于，它的单位太小，即使在日常十进制中并不大的数，用二进制表达也需
要很多位，太长了。所以对二进制位数分组，就显得更加迫切，如果每三位分组，就是八
进制，每四位分组，就是十六进制。理论上，也可以有按两位分组的四进制，与按五位分
组的三十二进制。但其他的分组二进制，并无实用，而八进制与十六进制，是有实用需求
才引入的，然后才逐渐流行起来，并成为事实标准。

八进制的基数有 `0-7` 八个符号，十六进制有基数有 `0-9a-f` 十六个符号（字母不分
大小写），它们与二进制 `0-1` 串组的对应关系如下：

基数 | 三分组 | 四分组 | 十进制 |
-----|--------|--------|--------|
0    | 000    | 0000   | 0      |
1    | 001    | 0001   | 1      |
2    | 010    | 0010   | 2      |
3    | 011    | 0011   | 3      |
4    | 100    | 0100   | 4      |
5    | 101    | 0101   | 5      |
6    | 110    | 0110   | 6      |
7    | 111    | 0111   | 7      |
8    |        | 1000   | 8      |
9    |        | 1001   | 9      |
a    |        | 1010   | 10     |
b    |        | 1011   | 11     |
c    |        | 1100   | 12     |
d    |        | 1101   | 13     |
e    |        | 1110   | 14     |
f    |        | 1111   | 15     |
     | 8进制  | 16进制 |        |

在计算机相关代码或文献中，为了将八进制或十六进制与常用十进制区分开来，在表述写
法上会加点前缀。其中八进制用 `0` 前缀，如 `0755` 表示八进制的 `755` ，也就是二
进制的 `111 101 101` 。十六进制用 `0x` 前缀，或大写的 `0X` ，如 `0xfe` 表示十
六进制的 `fe` ，也就是二进制的 `1111 1110` ，也就是十进制数值 `254` 。

由此可见，八进制、十六进制与二进制的转换是非常方便的，只要按基数表查表，分别转
换即可，基本可用人肉心算。但是二进制转十进制不是恰好倍数关系，没法分组分治，转
换起来就略麻烦些。用八进制或十六进制能大辐减少二进制数的位数长度，相比十进制的
等值数值，八进制表示略长，十六进制则短得多。

## 八进制的应用场景

据说，早期计算机的一个字节，不都是八位，而也有六位的情况。这可能是由于相关元器
件制造工艺与成本的原因，做六根导线比八根线会相对容易些吧。但现今这不是问题了，
一个字节八比特位已是工业与学界标准。八比特位正好分成两个四位组，也就是两个十六
进制数字，所以使用十六进制非常方便。但如果一个字节是六比特位的情况，则用八进制
更方便，这可能就是八进制出现的一个原因。

而现在，八进制与字节没关系了，所以它的应用场景远不如十六进制。现在仍然在广泛使
用的场合是 linux/unix 系统的文件权限表示位。

文件有三个重要权限，分别是读、写与执行。是否可读或写好理解，是否可执行是表示该
文件是否能像程序那样执行，否则就当作普通的数据文件。在 Linux 中，很多普通文本
文件都可能是可执行的脚本程序，所以可执行这个属性或权限很重要。这三种权限，通常
表示为 `rwx` ，分别只有两种状态，是否或有没有该种权限，那就可用 `0` 或 `1` 表
示，即三位二进制数，也就可用一位八进制数来表示。在 Linux 中，相对于文件的用户
又分为三种，即文件所有者（owner）、同组用户（group）与其他用户（other），每种
用户的权限用一位八进制的话，完整权限就是三位八进制数字表示了。

Linux 用 `chmod` 命令来修改权限，它接受八进制数值，也接收文本参数，用文本参数
可能更直观，但当熟悉八进制表示法后，用八进制更简捷与直接。假设现在有个文件
`file.txt` 的权限是 `400` ，表示只有该文件的所有者有读权限，其他用户没有任何权
限；根据系统用户合作需要，要使其他用户也有读权限，自己及其他同组用户有写权限。
若用文本参数描叙这些权限修改动作，可能要分几条命令来执行：

```bash
chmod +r file.txt
chmod u+w file.txt
chmod g+w file.txt
```

最终结果的权限是 `-rw-rw-r--` ，转为二进制是 `110 110 100` ，转成八进制就是
`664` ，所以直接按八进制数值修改权限的操作会更快：

```bash
chmod 664 file.txt
```

当然，另外有个常见需求，写完一个脚本后，需要给它加个可执行权限，如果想给所有用
户加个可执行权限，用文件参数 `+x` 更方便：

```bash
chmod +x script.sh
```

但如果觉得给其他用户开脚本运行权限是比较危险的事，只想给自己及或信任的同组用户
开执行权限，那就用八进制一次修改更方便了，如：

```bash
chmod 774 script.sh
```

除了 Linux 文件权限表示法，笔者并没有在其他方面看到八进制有良好的运用实践了。
可以想见，如果 Linux/Unix 系统完成历史使命，或者有更好的方式来表达权限特征，八
进制或许也会像五进制那样退出历史舞台。在八位字节统一标准后，只要十六进制辅助二
进制就足够了，没必要增加更多的复杂性。

## 十六进制为表的二进制

因此，现在在很多场合下，十六进制与二进制，几乎是同义词了。二进制为里，十六进制
为表，在内部用二进制运算，输出给人类用户看时用十六进制。

比如，很多宣称能编辑二进制的文本编辑器，它实际是展示十六进制的。Linux 下常用的
文本编辑器 vim 也有二进制编辑功能，执行 `:%!xxd` 就把当前文件转为二进制“打开”
了。实际上 `xxd` 是随 vim 安装的独立工具，其功能是将输入内容用十六进制方式打印
出来。而在 vim 中执行 `:%!xxd` 其实是利用了 `:!` 的过滤功能，调用外部 `xxd` 程
序将当前编辑内容转为十六进制展示，并替换当前编辑内容（注意不要用 `:w` 保存，
否则就将十六进制的展示方式当作实际内容写入文件了，这很可能不是想要的；看完十六
进制的展示，最好用 `u` 命令撤回操作，回到正常文本展示模式）。

所以我们也可直接在 shell 命令行中用 `xxd` 来查看文件的二进制内容，例如：

```bash
xxd ~/.bashrc
00000000: 2320 7e2f 2e62 6173 6872 633a 2065 7865  # ~/.bashrc: exe
00000010: 6375 7465 6420 6279 2062 6173 6828 3129  cuted by bash(1)
00000020: 2066 6f72 206e 6f6e 2d6c 6f67 696e 2073   for non-login s
00000030: 6865 6c6c 732e 0a23 2073 6565 202f 7573  hells..# see /us
00000040: 722f 7368 6172 652f 646f 632f 6261 7368  r/share/doc/bash
00000050: 2f65 7861 6d70 6c65 732f 7374 6172 7475  /examples/startu
00000060: 702d 6669 6c65 7320 2869 6e20 7468 6520  p-files (in the
00000070: 7061 636b 6167 6520 6261 7368 2d64 6f63  package bash-doc
00000080: 290a 2320 666f 7220 6578 616d 706c 6573  ).# for examples
00000090: 0a0a 2320 4966 206e 6f74 2072 756e 6e69  ..# If not runni
000000a0: 6e67 2069 6e74 6572 6163 7469 7665 6c79  ng interactively
000000b0: 2c20 646f 6e27 7420 646f 2061 6e79 7468  , don't do anyth
000000c0: 696e 670a 6361 7365 2024 2d20 696e 0a20  ing.case $- in.
000000d0: 2020 202a 692a 2920 3b3b 0a20 2020 2020     *i*) ;;.
......
```

而 `.bashrc` 其实是个普通文本文件，对应的前面几行如下：

```bash
cat ~/.bashrc
# ~/.bashrc: executed by bash(1) for non-login shells.
# see /usr/share/doc/bash/examples/startup-files (in the package bash-doc)
# for examples

# If not running interactively, don't do anything
case $- in
    *i*) ;;
......
```

大家可以对照着文件的实际内存，体会一下 `xxd` 打印二进制文件的格式，其他二进制
编辑器也基本是类似的风格。前面的“行号”，其实是地址，第二行 `10` 的十六进制数值
等于十进制的 `16` ，也就是每行打印 16 个字符（或字节）。中间部分的主体内容就是
每个字节的十六进制表示，每两个数值代表一字节，每两个字节间它额外加个空格也只为
整齐分隔，否则 32 个数字连在一起辨别困难。右侧部分是对应该行每个字节的文本展示，
如果是可打印字符（32-126 ascii 码），就可直接打印，其他字符统一用点占位表示不
可打印，比如该文件实际内容的第一行末尾的换行符 `0a` ，就大约在第 `30` 行中间位置。
另外注意，空格（十六进制 `20`）是也算可打印字符，右侧也对应一个空格。对于真正
的二进制文件，非文本文件，`xxd` 输出的右半侧预览基本都是一些不可识别的 `.` ，
即使偶尔碰巧是可打印字符，也未必是原文件的本意，只是某个字节正好落在 `[32, 126)` 
区间。

再举个常见的例子，MD5 摘要，它将任意长度的数据，通过某种算法得到 16 字节摘要。
Linux 下也有个命令 `md5sum` 用于求一个文件的 MD5 摘要，它打印的是 32 个数字，
用以表达内部算法求出的 16 个字节数据。如：

```bash
md5sum .bashrc
f45e5e883584d4a9f955562066cf75f3  .bashrc
```

## 二进制大数的十进制单位

如前所述，在计算机很多领域，二进制或十六进制是表示数据的，并不一定有数值意义。
在向人类传达数据信息时，用两个十六进制数字代替一个字节数据更方便。此外，将十六
进制当作数值时，一般只用在与内存、存储相关的地址或容量上。

在表示容量数值时，人们又更习惯于十进制的表达，于此又衍生出一系列容量单位，如：

- 1K = 1024 = 2^10
- 1M = 1024K = 2^20
- 1G = 1024M = 2^30

这种 1024 的“进制”单位，主要是用于表达字节数量，写作 `KB` 或 `MB` ，而不会单独
使用 `K` 来表示 1024，比如我们不会将某件商品卖价 1024 元写作 1K 元，不合习惯而
已。同时，这些单位是给十进制数值体系用的，比如我们会说 `15G` ，而不会说（十六
进制）`fG` 。

事实上，单独的 `K` 在十进制中也经常表示 1000 。所以一些硬盘生产商就会故意混淆
概念，标称 1GB 的容量，其实没有 1024MB ，只有 1000MB 。

## 结语

本文简单探讨了计算机领域使用二进制与十六进制的相关话题。这尤其说是一种技术，不
如说是一种文化习惯，毕竟进制这概念在数学原理上也不复杂。在当前信息时代，即使不
是计算机从业人员，了解基本的二进制与十六进制也是有益的，以增加对这种数值表达的
熟悉与敏感度。
