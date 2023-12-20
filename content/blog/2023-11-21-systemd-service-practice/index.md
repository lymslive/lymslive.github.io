+++
title = "探索 systemd 管理服务之旅"
[taxonomies]
categories = ["工具使用"]
tags = ["linux","systemd"]
[extra]
toc = true
+++
<!-- # 探索 systemd 管理服务之旅 -->

最近学习了用 systemd 管理服务，感觉挺有趣。故有意写下自己的心路历程与人分享。
systemd 是 linux 下的系统管理工具，功能强大，其实远不限于管理服务，不过本文暂
且先只讨论服务。
<!-- more -->
## 服务与程序：有限与无限的 Hello World

首先明确与界定概念。什么叫服务？我认为服务也不过是一种或一类程序，其特征是一般
会长期在后台运行。若用更专业的术语，正在跑的程序叫进程，那才称得上服务。不过在
不会太混淆的情况下，我们仍可用更通俗的程序一词。

从这个角度看，程序可分为两种。第一种是短命的，可预期能快速终止的程序；第二种是
意在长期运行的，甚至可能理论上是无限期运行的。服务就是这第二种程序。

短命的程序，最著名的就是每个语言学习的第一个 `Hello World` 程序，例如：

```c
// hello-world.c
int main(int argc, char* argv[])
{
    printf("Hello, World!\n");
    printf("Hello, World!!\n");
    return 0;
}
```

当然，有很多命令行工具是实用的短命程序，如 `ls` ，它执行完一次特定任务就自然退
出，成功地退出。

长寿的程序，最简单的实现方式就是使用一个无限循环，让它永远不会从 `main` 返回，
例如：

```c
// hello-forever.c
int main(int argc, char* argv[])
{
    while (1)
    {
        printf("Hello, World!\n");
        sleep(1);
    }
    return 0;
}
```

不过，有意义的长寿程序是会在无限循环中做具体工作的，比如网络服务，可能就会监听
一个端口，在无限循环中用 `accept()` 函数等待一个客户端连接。当然，业界实际的网
络服务可能是用 `epoll` 循环。因此这不仅是长寿，本质上更是恒寿。

## 程序的启动：夭寿、长寿与恒寿

现在，假设我们已经将程序源代码编译为可执行程序文件，下一步就该考虑如何启动、运
行它了。

### 不同寿命的程序启动方式

对于短命或夭寿的程序，很简单，直接敲在命令行上就是，如：

```bash
$ ./hello-world.exe
Hello, World!
# output go here
```

注：linux 可执行文件一般不用 `.exe` 后缀，但在文章表述中，我觉得加上后缀更达意。
`$` 表示 shell 的提示符，其后的文本是用户输入的，需要按回车。以 `#` 开头的行表
示注释，解释之用。行首没有特殊字符的表示程序的输出。

计算机的运行速度相对人的感官是很快的，所以对于很大一部分命令行工具程序，它很快
就能执行完毕而退出，即时反馈结果给终端用户，而这往往是用户需要的。

当然有时也会遭遇意外，本该夭寿的程序没如期快速结束。原因可能是涉及复杂计算，或
者要扫描大量文件，或者要访问网络的程序遇到卡网速了，等等。比较实际的例子是，编
译大型 C++ 或 rust 项目，以及执行其中大量的单元测试用例，那很可能就不是秒级的
时间尺度能完成，而需要按分钟或小时计。

这种情况不妨称之为夭寿程序的异化。有时夭寿程序的异化只因输入了错误的参数或配置
，如果是这样，当你在命令行发出启动命令后，对它没有及时结束感到焦虑或困惑时，可
以按 `ctrl-c` 中断程序运行，检查参数配置之类的再次执行试试看。

当确认启动姿势没问题，完成该任务确实需要花费不短时间后，你只能耐心等它结束。好
在现代操作系统早已脱离原始蛮荒时代，都支持多任务并行了，你只需重要打开一个（虚
拟）终端就可以同时做其他事情，并不一定要干等着。不过，若限于当前连接的终端讨论
，你仍然可以按 `ctrl-z` 将正在运行的（夭寿异化为长寿）程序挂起在后台，回到终端
允你执行其他命令。不过注意被这样挂起的程序是暂停的，可以用 `fg` 命令将它重新拉
到前台才能继续执行，才有可能跑到终点。该过程大约如下所示：

```bash
$ ./hello-world.exe 
# wait for short or long time
^Z
[1]+  Stopped                 ./hello-world.exe
# suspend to do other quick task
$ fg
# continue run the suspend process
```

此外，也可以用 `bg` 命令将它置于后台运行且不会暂停，示意如下：

```bash
$ ./hello-world.exe 
^Z
[1]+  Stopped                 ./hello-world.exe
$ bg
[1]+ ./hello-world.exe &
# return to shell to do other task
```

注：这里请不要在意仍用 `hello-world.exe` 程序示意，如按本文开头的代码编译，它
将是毫无意外的夭寿程序，故这里指代其他任意有潜质异化为长寿的程序。

然后，如果你已经摸清了该程序的脾性，能预测在某种情况下将长寿地运行，就可以直接
在启动时加上 `&` 表示后台运行。如此它会在打印启动的进程 pid 后快速返回 shell ：

```bash
$ ./hello-world.exe &
[1] 167
# return to shell quickly
```

置于后台运行的程序，断开了对终端控制，将不能从标准输入读取数据了，但仍能向标准
输出与标准错误（默认都是终端），有时这会干扰终端打印。因此，后台运行的程序，也
常常伴随着输入输出重定向，形如：

```bash
$ cat input-file | ./hello-world.exe >output-file 2>/dev/null
```

可以想象，已经异化为长寿的程序，在某方面的表现形式上已经接近恒寿程序了。但它们
仍然有本质的区别，异化的程序再怎么长寿，也是有限寿命的，与从根本上设计为无限循
环的恒寿程序仍有天壤之别。在现实项目中，异化的长寿程序，跑个几小时，已经很恐怖
了，但作为恒寿程序的服务，至少要按天跑吧，而严肃的高可用服务，那是要求长年累月
不间断运行的。

所以，对于恒寿程序，也许在开发、测试中，可以参考长寿程序的启动，简单地在未尾加
个 `&` 置于后台运行（甚至不加），但在服务正常运行时，这还是不够的。问题在于，
当 shell 断开当前连接会话时，从该 shell 会话启动的后台程序也会被终止。解决之道
是在前面再加个 `nohup` ：

```bash
$ nohup ./hello-forever.exe &
[1] 170
nohup: ignoring input and appending output to 'nohup.out'
```

理解 `nohup` 的关键，在于理解 `HUP` 信号。当 shell 断开时，会向其下派生的所有
进程发送 `HUP` 信号，该信号的默认动作就是终止进程。所以当 shell 断开时，由该会
话创建的进程也会随之终止。`nohup` 的意图，就是指定其后启动的进程，忽略 `HUP`
信号，于是不会因为 shell 的断开而终止。这正是恒寿程序的需求。

另外注意到，在用 `nohup` 结合 `&` 启动程序时，如果没有显式将标准输出重定向，它
会默认重定向到 `nohup.out` 文件。毕竟当前终端是可能被关闭的，它无法再写到当前
终端，必须重定向。

### 管理恒寿服务的问题

终于，我们能将某个服务（恒寿程序）以较为靠谱的姿势拉起了，但这只是万里长征的第
一步而已。因为作为需要长期运行的程序而言，后续的管理与监控也是至关重要的。

首先容易想起的问题是，我如何知道它一直在运行？或者转化为一个更实际的问题，将来
的任一时刻，我得有办法观察到它还在运行。这个其实容易的，用 `ps` 命令加 `grep`
就能看到它是否还在运行，如：

```bash
$ ps aux | grep hello
lymslive   170  0.0  0.0  10540   528 pts/1    S    23:30   0:00 ./hello-forever.exe
lymslive   193  0.0  0.0  14804  1196 pts/1    S    23:43   0:00 grep --color=auto hello
```

每个人尝试时具体的输出应该不一样，其中 `170` 是进程的 PID ，与它刚启动时打印在
终端的 PID 是一致的。

另一个不算难但比较烦的问题是，每次启动时，容易忘记 `nohup` 或 `&` ，尤其是还要
加命令行参数时，写起来更麻烦。如此，可考虑封装一个启动脚本，顺便还可以加环境变
量设置或其他必要扩展。例如：

```bash
$ cat run-hello-forever.sh
#!/bin/bash
# 设若还需依赖不在系统默认路径中的动态链接库
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./libs
# 设若程序启动接受命令行参数配置
nohup ./hello-forever.exe hello-forever.ini &
```

然后一个更重要的监控问题是，如果服务进程意外崩溃了，该如何及时重启？一个直观的
想法是，可以尝试利用上面的 `ps | grep` 命令，额外写个监控脚本，在无限循环中检
测相应的服务进程是否还在，如果不在了，就重启它。大致流程例如：

```bash
$ cat monitor-hello-forever.sh
#!/bin/bash
# 示意脚本，请勿食用

psCount=$(ps aux | grep hello-forever.exe | grep -v grep | wc -l)
while [[ "$psCount" == "0" ]]; do
    ./run-hello-forever.sh
    sleep 10
    psCount=$(ps aux | grep hello-forever.exe | grep -v grep | wc -l)
done
```

另注：在实际的项目中，也许只检测进程仍在还不够，有时虽然进程还在，但意外卡死了
，不能处理业务……这也叫健康度或活性检测。这可能已超出本文的范围，这里只讨论最
简单的进程存在性问题，毕竟这是作为恒寿程序的基本素养。但即便如此，这个手撸的监
控脚本也存在诸多问题：

* 这个 `monitor-hello-forever.sh` 脚本使用了循环，所以它也是个恒寿程序，也得用
  `nohup ... &` 来启动它，如果嫌弃这写法麻烦且丑陋，还得另外再封装个脚本，这就
  套娃了啊，为脚本再想个名字都是个事。
* 只在监控脚本 `monitor-hello-forever.sh` 判断了服务进程不在时启动它，但在启动
  脚本 `run-hello-forever.sh` 中却没有判断若服务进程存在时避免重复启动，有没可
  能在监控脚本的 `while` 循环中出现 bug 为服务反复启动很多进程呢？
* 用 `ps | grep | wc` 判断进程数是很粗糙的做法，手动查看时问题可能不大，但在脚
  本中判断就太脆弱了。例如，也幸好这里违反 linux 习惯加了 `.exe` 后缀，歪打正
  着了，否则在编辑配置文件 `vim hello-forever.ini` 时就有可能使 `grep
  hello-forever` 误判。

这里指出问题，并不是想怎么把上面的启动脚本与监控脚本优化，而是想要说明，当你试
图手搓脚本去管理服务（恒寿程序）时，那是吃力不讨好的事情，有很多坑等着你跳。正
确的姿势是跳到更高的维度，用专门的服务管理工具来做这事。在 linux 系统中，
`systemd` 就是这样的利器，你值得拥有。

## 系统的服务管理利器：systemd 与 systemctl

如果用 `ps` 查看，会发现 `systemd` 的进程 pid 为 1 ，表示它是 linux 启动后的第
一个进程，其他进程都是从它 fork 出来的。正可谓开天避地，万物之主。

按本文开始对程序的划分标准，显然 `systemd` 也是个恒寿程序，它必须在后台始终运
行着，才能管理其他服务，就像我们上节手撸的监控脚本一样，当然其位格是不可同日而
喻的。而 `systemctl` 命令可以与 `systemd` 通讯，每次完成执行一个具体的任务，
所以它是辅助的夭寿程序。打个比方，`systemd` 就是上神本尊，一般人看不见摸不着，
而 `systemctl` 恰像可以上达天听的巫师，普通人就能通过祭祀巫师来向上神祈求。

所以对于普通用户而言，`systemctl` 就是拥有很多魔法的工具命令，只要掌握这个命令
的常用用法，以及了解一些 `systemd` 约定的配置方法，就可以很好地管理所需的其他
服务了。如果用过 `docker` ，那么 `systemctl` 与 `systemd` 的关系，就如
`docker` 与 `dockerd` 的关系。

另外，从命名可猜测到，`systemctl` 可能需要 root 登陆或使用 `sudo` 才能执行那些
能影响 `systemd` 的命令。不过为行文简便起见，下面的示例会省略前导 `sudo` ，自
行实践时如果遇到权限问题可酌情加上 `sudo` 。

### 为 systemd 配置简单的恒寿程序服务

为了能让 `systemd` 管理我们上面写的 `hello-forever.exe` 恒寿程序，可以在
`/etc/systemd/system/` 目录下添加一个 `hello-forever.service` 配置文件。按
`systemd` 的术语，这也叫单元文件（unit file），除 `service` 外，它还支持其他多
种单元文件，各用相应不同的后缀区分，暂且不论。

```bash
$ cat /etc/systemd/system/hello-forever.service

[Unit]
Description=The famous hello-world service for ever.

[Service]
WorkingDirectory=/absolute/path/to/cwd
Environment=LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./libs
ExecStart=/absolute/path/to/hello-forever.exe hello-forever.ini
Restart=always
```

这里，`[Unit]` 段只有一行卖萌的描叙字段，其实是可以省略的。但一般都要求有基本
的 `[Unit]` 段，比如实际的服务可能依赖其他服务，就可配置这段下。

服务类单元配置要求有 `[Service]` 段，其中最重要的是 `ExecStart` 字段，表示如何启
动服务的命令行语句。注意几点：

* 可执行程序文件名必须用绝对路径
* 像在命令行执行夭寿程序一样将恒寿程序运行在前台，不用加 `nohup` 与 `&`
* 实际上不允许重定向符、管道符及 `&` ，如有也只是当作普通命令行参数传给程序

环境变量按需添加在 `Environment` 字段（也可用 `EnvironmentFile` 字段）。如果程
序涉及按相对路径读写文件，则要在 `WorkingDirectory` 字段中配上当前工作路径。

重启策略配在 `Restart` 字段，大部分情况下可无脑 `always` 总是重启，比较省心。

如果在添加该配置单元文件之前执行 `systemctl status` ，它应该会提示找不到相关
service 文件：

```bash
$ systemctl status hello-forever
Unit hello-forever.service could not be found.
```

在添加配置后，再次查看状态，则如下：

```bash
$ systemctl status hello-forever
● hello-forever.service
   Loaded: loaded (/etc/systemd/system/hello-forever.service; static; vendor preset: disabled)
   Active: inactive (dead)
```

可见它指明了服务未启动。执行 `start` 子命令后再次 `status` 查看状态，输出形如：

```bash
$ systemctl start hello-forever
$ systemctl status hello-forever
● hello-forever.service
   Loaded: loaded (/etc/systemd/system/hello-forever.service; static; vendor preset: disabled)
   Active: active (running) since Mon 2023-11-27 09:39:31 CST; 2s ago
 Main PID: 593521 (hello-forever.e)
    Tasks: 1
   Memory: 88.0K
   CGroup: /system.slice/hello-forever.service
           └─593521 /absolute/path/to/hello-forever.exe

Nov 27 09:39:31 hostname systemd[1]: Started hello-forever.service.
```

注意，`start` 启动失败会有错误提示输出，但启动成功不会有任何输出，可以用
`status` 子命令确认启动成功并且查看其他有关状态信息及最近几条日志信息。如果是
有颜色的终端，运行状态还会用绿色状态标记出来。

可以通过状态输出的 PID ，用 `kill -9` 杀死服务进程，但不建议这么做。另外由于配
置了重启策略为 `always` ，用 `kill` 杀死服务进程后，或因其他意外崩溃后，会自动
重启服务。如果要停止服务，请用 `stop` 子命令，例如：

```bash
$ systemctl stop hello-forever
```

除了 `start` 与 `stop` 命令外，常用的还有 `restart` 与 `reload` 。其中
`reload` 需要相应的服务本身支持重新加载（配置），并且要配置正确的命令，比如
nginx 就支持 `reload` 。

### 配置 fork 与 daemonize 服务

如前所述，一般的长寿程序想在命令行优雅地启动，需要用 `nohup` 与 `&` 修饰。但通
过一些技巧，可以避免这两个累赘的饰品，直接像夭寿程序那样简单启动。其核心思想就
是先 `fork` 系统调用，父进程夭寿退出（返回 shell），子进程恒寿永存提供服务。大
致代码框架如下：

```c
// hello-daemon.c
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t pid = fork();
    if (pid == 0) { // child process
        while (1)
        {
            printf("Hello World!\n");
            sleep(1);
        }
        return 0;
    }
    else if (pid > 0) { // parent process
        return 0;
    }
    else { // fork failed
        return -1;
    }
}
```

为了有所区分，不妨将这类恒寿程序称为隐世恒寿，父为子隐，父进程虽然亡了，但子进
程延续了所有精神，实现了其价值。相对的，之前介绍的简单在前台运行的可称为入世恒
寿程序。

一些著名的服务软件，如 `nginx` 就可以直接启动服务，盖因在其启动流程中也有类似
的设计。

显然，这个设计也能解决恒寿程序启动命令输入不友好的问题，但如果我们自己每次想写
一个恒寿程序或服务时，都要按步就班写这套 `fork` 的三路 `if else` ，又嫌烦或重
复了。于是，可以专门写个程序，将其他长寿程序转化为后台服务——只需将上述代码中，
在子进程分支的关键业务代码中用 `exce` 系列函数调用目标恒寿程序即可。

`systemd` 能启动其他长寿程序，大致也是使用 `fork` 子进程再用 `exce` 替换进程，
当然还涉及其他许多必要的设置，使得子进程能纳入自身的管理范畴。

然后，`systemd` 也能管理这类带 `fork` 的隐世长寿程序，只要加上配置字段 `Type`
是 `forking` ：

```bash
$ cat /etc/systemd/system/hello-daemon.service

[Unit]
Description=The famous hello-world service with daemon forked self.

[Service]
Type=forking
ExecStart=/absolute/path/to/hello-daemon.exe
Restart=always
```

其中，`Type` 的默认值是 `simple` ，所以上一节的 `hello-forever.service` 可以省
略不配。启动该服务再查看状态：

```bash
$ systemctl start hello-deamon
$ systemctl status hello-deamon
● hello-deamon.service - The famous hello-world service with deamon forked self.
   Loaded: loaded (/etc/systemd/system/hello-deamon.service; static; vendor preset: disabled)
   Active: active (running) since Mon 2023-11-27 14:52:00 CST; 6s ago
  Process: 56600 ExecStart=/absolute/path/to/hello-deamon.exe (code=exited, status=0/SUCCESS)
 Main PID: 56601 (hello-deamon.ex)
    Tasks: 1
   Memory: 196.0K
   CGroup: /system.slice/hello-deamon.service
           └─56601 /absolute/path/to/hello-deamon.exe

Nov 27 14:52:00 hostname systemd[1]: Starting The famous hello-world service with deamon forked self....
Nov 27 14:52:00 hostname systemd[1]: Started The famous hello-world service with deamon forked self..
```

若仔细对比上节 `hello-forever.service` 的启动后的状态输出，能发现有两点主要的
不同：

1. 在 `Active` 与 `Main PID` 两行之间，多了一行指出 `hello-deamon.exe` 进程成
   功退出了，这就是退出的父进程。
2. 末尾的日志，多了一行 `Starting ...` 。这是说 `forking` 类型的 `service`, 
   父进程被 `systemd` 当作了服务启动流程的一部分，当父进程成功退出时，
   `systemd` 才认为该服务状态是 `Started ...` 。而对于 `simple` 服务，
   当它的主进程启动后就认为是 `Started ...` 了。

然后，`hello-daemon.exe` 的父进程终止后，子进程的 PID 被 `systemd` 当成了该服
务的 `Main PID` 记录在案。因为在我们的示例中，只余下一个子进程在跑了，这个假设
是合理。

但如果 `forking` 型服务本身还有复杂的多进程架构，例如 `nginx` 的一个主进程与多
个工作进程。那么 `systemd` 可能就很难正确选择哪个进程 PID 记录为服务的 `Main
PID` 了，于是要求在配置中明确在 `[service]` 段添加 `PIDFile` 表示目标程序会自
己记录 pid 到一个外部文件，例如：

```bash
$ cat >> /etc/systemd/system/hello-deamon.service
PIDFile=/absolute/path/to/hello-deamon.pid
^D
```

注意，`PIDFile` 配置也要求是绝对路径，但并不要求一定与程序文件名在同一个路径下，
这完全取决于目标程序自己写在哪里。`systemd` 会读取该文件，将其内容当成目标服务
的 `Main PID` 记录起来。

问：既然 `forking` 型 service （隐世恒寿程序）自己就能独立地优雅启动，为何还要
用 `systemd` 配置？

答：利用 `fork` 后台化程序，是在 `systemd` 流行之前就广泛使用的技巧或设计方法，
因而 `systemd` 想包罗万象，就得兼容这种服务程序。且由于 `systemd` 提供了额外的
管理便利，故这类服务程序乐于纳入 `systemd` 的管理。

### 配置 socket 触发的服务

涉及网络的服务，还可以通过 socket 来触发启动。也就是说服务本身在没连接业务时不
用一直空跑着，可以让 `systemd` 帮忙监听一个 socket ，以减少资源消耗。当真正有
业务连接进来时，才唤醒目标服务。要达成这样的配置，目标服务程序在实现上也有一定
讲究。

开发一个常规的网络服务，一般有以下几个关键步骤：

1. 创建一个 socket
2. 调用 `bind` 将该 socket 绑定一个端口
3. 调用 `listen` 监听端口，将该 socket 变成监听文件描叙符 fd
4. 调用 `accept` 接收一个客户端连接，得到一个新的连接文件描叙符 fd
5. 读写连接 socket 的 fd，完成业务逻辑

借助 `systemd` 强大且通用的服务功能，它可以帮忙完成前两步，并且将 socket 的 fd
传给被激活的程序，后者就只要从第 3 步开始实现工作。

一个简单的示例如下：

```c
// hello-socket.c
#include <stdio.h>
#include <string.h>
#include <systemd/sd-daemon.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (sd_listen_fds(1) > 0)
    {
        int fd_listen = SD_LISTEN_FDS_START;
        if (listen(fd_listen, 511) < 0) {
            perror("Failed to listen");
            return -1;
        }
        fprintf(stderr, "succeed to listen with fd: %i\n", fd_listen);
        for (int i = 0; i < 8; ++i)
        {
            int fd_accept = accept(fd_listen, NULL, NULL);
            if (fd_accept < 0)
            {
                perror("Failed to accept");
                return -1;
            }
            fprintf(stderr, "succeed to connect with fd:  %i\n", fd_accept);
            char buffer[32] = {0};
            snprintf(buffer, sizeof(buffer), "Hello, World: %d\n", i);
            write(fd_accept, buffer, strlen(buffer));
            close(fd_accept);
        }
    }
    else
    {
        perror("Failed to sd_listen_fds");
        return -1;
    }

    return 0;
}
```

其中， `sd_listen_fds` 函数就能返回 `systemd` 传给被激活程序的 fd 的个数，
并且从 `SD_LISTEN_FDS_START` 开始表示有效的 fd 。这个宏定义的值是 `3` ，因为每
个程序前面 `0-2` 这 3 个文件分别表示标准输入、标准输出与标准错误，所以若新建了
`fd` ，就从 `3` 开始。又因为在这个示例中，`systemd` 将只会传一个 `fd` 过来，所
以 `SD_LISTEN_FDS_START` 就是要 `listen` 的端口了。

在后面的 `accept` 循环中，对每个连接的业务仍然是简单的 "Hello World" 。另外故
意设计成有限循环（8次），正常的实际应用多为无限循环。

编译这个程序依赖 `systemd` 的开发库，可能是默认未装的，可用相应 linux 发行版的
包管理工具先安装，再链接 `systemd` 。例如：

```bash
$ yum install systemd-devel
$ gcc -std=c99 -lsystemd -o hello-socket.exe hello-socket.c
```

然后 `systemd` 的配置文件要有两个，后缀分别是 `.socket` 与 `.service` ，除后缀
外的文件名要相同，这样就能自动关联，这里也取名 `hello-socket` ，如：

```bash
$ cat /etc/systemd/system/hello-socket.socket
[Unit]
Description=Hello Socket

[Socket]
ListenStream=0.0.0.0:1234

$ cat /etc/systemd/system/hello-socket.service
[Unit]
Description=Hello Socket

[Service]
ExecStart=/absolute/path/to/hello-socket.exe
```

如上，`.socket` 的配置，需要有 `[Socket]` 段，`ListenStream` 字段表示了要监听
的地址与端口。相应的 `.service` 配置，与之前例子一样，描叙了如何启动服务。因为
这是想由 socket 激活的 service ，故没有配置重启字段。

在 `systemctl` 的大多数子命令中，如 `start` ，其参数默认是假定 `.service` 单元
配置的。如之前的例子 `systemctl start hello-forever` 等效于
`systemctl start hello-forever.service` 。但在这个例子中，有两种同名单元配置，
且按要求先只启动 `hello-socket.socket` ，所以要写完整的单元名：

```bash
$ systemctl start hello-socket.socket
```

事实上，如果（在另一个 shell 尝试）先启动 `hello-socket.service` 或
`hello-socket` ，那会失败的。

然后再分别查看两种单元的状态，可发发现 `.socket` 是启动的，`.service` 是未启动
的：

```bash
$ systemctl status hello-socket.socket
$ systemctl status hello-socket.service
```

这里就不再贴示例的状态输出了。用 `ps | grep` 应该也查不到 `hello-socket.exe`
程序在运行。但是可以找一个网络客户端来连接这个 socket ，比如就用 `nc` 从相同主
机连接:

```bash
$ echo "" | nc 127.0.0.1 1234
Hello, World: 0
```

因为我们的示例程序不介意从连接 socket 读什么，所以前面加了 `echo |` ，如果不加
前面的管道，也会有输出，但可能不会立即退出 `nc` ，要手动 `ctrl-c` 或 `ctrl-d`
结束。

此时再去查看 `hello-socket.service` 的状态，它就在运行中了。

随后可以重复用 `nc` 命令测试，输出的编号应该会递增。也可以从其他主机连接，只要
指定正确的 ip 。当输出编号到达 `7` 时，再回去查看 `hello-socket.service` 的状
态，会发现它是正常退出的。然后若再用 `nc` 连接去测它，它又会重新激活，从编号
`0` 开始输出。

另外，可注意到程序中向 `stderr` 打印的日志，会出现在 `status` 的输出中，可供调
试分析问题之用。

最后，`.socket` 配置还支持一个 `Accept` 字段，默认 `false` ，就是如上行为。可
以尝试将其改为 `true` （也可复制为另一份再修改），如：

```bash
$ cat /etc/systemd/system/hello-socket.socket
[Unit]
Description=Hello Socket

[Socket]
ListenStream=0.0.0.0:1234
Accept=true
```

如此则表示 `sytemd` 还将接管 socket 的 `accpet()` 步骤，将已接受的连接传给激活
的服务。也即本节开始的前 4 个步骤都被 `systemd` 做了，自已开发的程序只要写第 5
步业务。当然，程序也要做相应的改动：

```c
int main(int argc, char* argv[])
{
    if (sd_listen_fds(1) > 0)
    {
        int fd_accept = SD_LISTEN_FDS_START;
        fprintf(stderr, "succeed to connect with fd:  %i\n", fd_accept);
        char buffer[32] = {0};
        snprintf(buffer, sizeof(buffer), "Hello, World: %d\n", fd_accept);
        write(fd_accept, buffer, strlen(buffer));
        close(fd_accept);
    }
    else
    {
        perror("Failed to sd_listen_fds");
        return -1;
    }

    return 0;
}
```

关闭原来的 `.socket` 与 `.service` ，再重启 `.socket` ：

```bash
$ systemctl stop hello-socket.socket
$ systemctl stop hello-socket.service
$ cd /etc/systemd/system
$ mv hello-socket.service hello-socket@.service
$ systemctl daemon-reload
$ systemctl start hello-socket.socket
$ systemctl status hello-socket.socket
```

注意配置修改后，`systemctl` 可能会要求你执行 `deaon-reload` 重新加载配置使修改
生效。同时，由于每个连接会激活一个服务程序，还要将普通的 `.service` 配置文件名
改为模板配置，点号前加一个 `@` 即 `{basename}@.service`。在模板配置中可以使用
一些变量替换，在这个简单示例中没用到，只改文件即可。

然后就可以用 `nc` 客户端测试了：

```bash
$ echo "" | nc 127.0.0.1 1234
Hello, World: 3
Ncat: Connection reset by peer.

$ echo "" | nc 127.0.0.1 1234
Hello, World: 3
Ncat: Connection reset by peer.
```

`systemd` 的 socket 支持 `Accpet` ，是为了兼容 `inetd` 的用法习惯。但每个连接
激活进程，效率偏低，所以默认是 `false` 的，由目标服务负责 `listen` 与 `accpet`
。

但是只配置 `Accept=true` 还不是完全兼容 `inetd` 启动的服务，因为后者使用的标准
输入与标准输出，这比读写 `fd` 对用户编写程序更友好。但可以在 `[Service]` 中添
加两个字段重定向标准输入输出至 `socket` 就能达到完美兼容，如：

```bash
$ cat /etc/systemd/system/hello-socket@.service
[Unit]
Description=Hello Socket

[Service]
#ExecStart=/absolute/path/to/hello-socket.exe
ExecStart=/absolute/path/to/hello-world.exe
StandardInput=socket
StandardOutput=socket
```

而使用标准输出的程序，`hello-world.exe` 就可以，所以启动程序可用它替换
`hello-socket.exe` 。同时，`hello-world.exe` 的编译显然也不需要依赖与链接
`systemd` 开发库。

当引入了 `systemd` 开发库，除了可用 `sd_listen_fds` 获取传递的 `fd` （数量）外，
还有其他 API 可用。如 `sd_notify` 可向 `systemd` 汇报自身状态，然后可在
`[service]` 段配置 `Type=notify` 让 `systemd` 知道该被管理的目标服务有此特殊能
力，以便更精准地跟踪状态。当然，这就与 `systemd` 深度耦合了，为了兼容性，最好
加入条件编译等手段，让程序在没有或不用 `systemd` 的情况下也能运行。

### 配置定时器及夭寿程序

从上一节配置 socket 激活的示例程序中可见，`systemd` 能配置管理的服务，并非一定
要恒寿程序。诚然，简单的夭寿程序，是没必要纳入什么管理系统的。但再加上期望按条
件自动触发的需求，那就有纳入管理系统的价值了。

让夭寿程序定时运行，也是另一种常见的条件触发需求。`systemd` 为这种情况配置的服
务，指定了专门的类型 `Type=oneshot` 。它就要求启动的程序是夭寿的，能很快或短时
间完成，也只有等它正常退出后，`systemd` 才认为这个 `service` 单元是成功启动的，
而这会影响被依赖的其他单元的启动。

首先配置一个 `.timer` 单元文件，例如：

```bash
$ cat /etc/systemd/system/hello-world.timer
[Unit]
Description=The Hello-World Timer

[Timer]
OnCalendar=*-*-* *:*:00
```

其中，`OnCalendar` 的配置格式同 `crontab` ，上例表示每分钟触发。

然后需要一个同名的 `.service` 单元文件。本文开头编译的 `hello-world.exe` 正好
可作为该定时器启动的程序，例如：

```bash
$ cat /etc/systemd/system/hello-world.service
[Unit]
Description=The Hello-World Timer

[Service]
Type=oneshot
ExecStart=/absolute/path/to/hello-world.exe
StandardOutput=file:/absolute/path/to/stdout-file
```

然后启动定时器，并查看状态：

```bash
$ systemctl start hello-world.timer
$ systemctl status hello-world.timer
```

再次提醒，`start` 子命令接的参数要显式加 `.timer` ，省略后缀时默认是 `.service`。

此外，我们的 `hello-world.exe` 是打印向标准输出的，配置在 `systemd` 启动运行时，
可以重定向一个文件，不是写在 `ExecStart` 字段后面的加 `>` ，而必须额外配在字段
`StandardOutput` 中。标准输出重定向有多个可选值，甚至 `tty` 表示仍打印到终端，
但这并不建议，因为突然触发的输出可能会打乱你当前正在工作的终端。

### 实际项目的 systemd 配置分析

以上用各种花式的 `Hello World` 程序为例介绍了 `systemd` 的服务配置。最后我们选
取两个开源项目的 `systemd` 配置来分析理解下。

一个是流行的 web 服务器 [nginx](https://nginx.org) ，一般会安装在
`/usr/local/nginx` 目录下。因为 `nginx` 程序自己作了后台化，故可以直接启动：

```bash
$ /usr/local/nginx/sbin/nginx
```

如果程序安装到了系统路径（`which nginx` 有输出），甚至可以不用全路径：

```bash
$ nginx
```

但是 nginx 安装包也提供了 `systemd` 配置，一般在 `/usr/lib/systemd/system` 目录下，
可以找到它的配置文件打开参考。也可以直接用 `systemctl cat` 命令查看：

```bash
$ systemctl cat nginx
[Unit]
Description=The nginx HTTP and reverse proxy server
After=network-online.target remote-fs.target nss-lookup.target
Wants=network-online.target

[Service]
Type=forking
PIDFile=/run/nginx.pid
ExecStartPre=/usr/bin/rm -f /run/nginx.pid
ExecStartPre=/usr/sbin/nginx -t
ExecStart=/usr/sbin/nginx
ExecReload=/usr/sbin/nginx -s reload
KillSignal=SIGQUIT
TimeoutStopSec=5
KillMode=process
PrivateTmp=true

[Install]
WantedBy=multi-user.target
```

所以也可以用 `systemctl` 系列命令来管理 `nginx` 了，例如：

```bash
$ systemctl enable nginx
Created symlink from /etc/systemd/system/multi-user.target.wants/nginx.service to /usr/lib/systemd/system/nginx.service

$ systemctl start nginx
$ systemctl status nginx
$ systemctl reload nginx
```

其中，`enable` 是可选的，它的意义就是安装个软链接在 `/etc/systemd/system` 的
某个子目录，也即 `[Install]` 配置段的指示。安装后一般可随系统自动启动。但是没
有 `enable` 也是可以 `start` 的，最多每次机器重启后自己再手动 `start` 一下。

由于 `ExecStartPre` 配置，在用 `start` 子命令启动 nginx 时，还会自动校验其配置
`nginx.conf` 。在修改了 `nginx.conf` 配置后，要重新加载配置，也只要执行
`reload` 子命令即可。这避免了记忆 `nginx` 的命令行参数。

另一个是 [fcgiwrap](https://github.com/gnosek/fcgiwrap) ，可用于将任意读写标准
输入输出的程序或 CGI 脚本封装成 FASTCGI 协议给 nginx 转发使用。因为 nginx 本身
不支持效率不高的 CGI ，只支持更高效的 FASTCGI ，但在对效率要求不高，或仅为快速
原型验证测试之用，CGI 又有便捷的优势，所以这就是 fcgiwrap 存在的意义。

如果要在命令行手动启动 `fcgiwrap` ，就得如下执行：

```bash
$ nohup fcgiwrap -s /var/run/fcgiwrap.sock &
```

因此很多相关介绍还推荐下载另一个程序
[spawn-fcgi](https://redmine.lighttpd.net/projects/spawn-fcgi)
来专门启动 fcgiwrap 。但是 spawn-fcgi 只能管启动。

更好的办法当然是用 `systemd` 管理。在 fcgiwrap 的 github 源码仓库中，有个
`systemd` 目录，有两个配置单元文件，`fcgiwrap.socket` 与 `fcgiwrap.service` ，
直接复制到 `/etc/systemd/systemd/` 目录就行。这两个文件的内容也简单：

```bash
$ cat fcgiwrap.socket
[Unit]
Description=fcgiwrap Socket

[Socket]
ListenStream=/var/run/fcgiwrap.sock

[Install]
WantedBy=sockets.target

$cat fcgiwrap.service
[Unit]
Description=Simple CGI Server
After=nss-user-lookup.target

[Service]
ExecStart=/usr/sbin/fcgiwrap
User=http
Group=http

[Install]
Also=fcgiwrap.socket
```

其中，`[Server]` 段下 `User` 与 `Group` 最好配置成运行 `nginx` 相同的用户与组。
然后就可用 `systemctl` 启动 socket ：

```bash
$ systemctl start fcgiwrap.socket
```

这个命令只启动 socket ，不启动 fcgiwrap 的服务，等有连接时再激活 fcgiwrap 。

如果觉得在输入启动命令时要输入额外的 `.socket` 很烦，也不介意直接预启动服务，
那可以在 `fcgiwrap.service` 单元文件的 `[Unit]` 段下增加
`Requires=fcgiwrap.socket` ，这个依赖表示启动 `fcgiwrap.service` 时会先启动
`fcgiwrap.socket` 。于是 `systemctl start fcgiwrap` 会同时启动 socket 与
service 。不过如果想直接启动服务，不如不要 `fcgiwrap.socket` 单元，只用
`fcgiwrap.service` 单元，然后在它的启动命令中直接加命令行参数创建所需的
`socket` ，即改成这样：

```
ExecStart=/usr/sbin/fcgiwrap -s /var/run/fcgiwrap.sock
```

另外，可参考一下 `fcgiwrap.c` 源码，看它如何用 `sd_listen_fds` 与
`SD_LISTEN_FDS_START` 实现从 `systemd` 传递的 socket 启动，以及当不从
`systemd` 启动时，如何从命令行参数 `-s` 指定监听地址。

然后，还要修改一下 `nginx.conf` ，以支持转发 fcgiwrap ，例如：

```conf
location /cgi-bin/ {
    gzip off;
    root  html;
    fastcgi_pass  unix:/var/run/fcgiwrap.sock;
    include fastcgi_params;
    fastcgi_param SCRIPT_FILENAME  $document_root$fastcgi_script_name;
}
```

这里配置的 socket 地址，要与 `fcgiwrap.socket` 的配置吻合。
`systemctl reload nginx` 后将 cgi 脚本放到 `/usr/local/nginx/html/cgi-bin/` 
目录下，就可以通过浏览器访问了，也可用 curl，如：

```bash
$ curl 127.0.0.1/cgi-bin/some-test.cgi
```

注意 `some-test.cgi` 向标准输出打印的内容，要符合 http 协议，要有头部、空行与
内容。比如：

```bash
#!/bin/bash
echo "Content-type:text/plain"
echo ""
echo "OK test CGI, and content body"
```

`fcgiwrap` 会自动处理多种回车换行格式，所以不必强行输出 `\r\n` ，按一般习惯输
出 `\n` 即可。

## 总结

本文从程序运行寿命出发，讨论了三种典型的程序类型在命令行启动的推荐方式是：

* 夭寿程序：很快能完成的程序，`./command`
* 长寿程序：需要运行较长时间，`./slow-commnd &`
* 恒寿程序：理论上无限循环的循环，长期运行的服务， `nohup ./service &`

然后，为了简化服务的启动方式并进行统一管理，讨论了不同特征类型的服务在
`systemd` 中如何配置，主要包含：

* 简单服务，`Type=simple`，主进程会阻塞在一个无限循环中
* 自行 fork 后台化的服务，`Type=forking` ，常见于多进程架构的服务
* 用 socket 激活的服务，一般用于访问不频繁的服务，节省机器资源
* 与定时器联用的 `oneshot` 型夭寿程序，常用于定时任务

最后，讨论了开源实例 `nginx` 与 `fcgiwrap` 的 `systemd` 配置。

当然了，`systemd` 是个功能繁杂的服务管理系统，它所能管理的单元也不限于服务，所
以其他未尽细节请查看相关手删或帮助：

* `systemctl --help` 命令行工具使用帮助
* `man systemctl` 命令行工具使用手册
* `man systemd` 系统管理总述
* `man systemd.unit` 通用单元配置说明，`[Unit]` 段
* `man systemd.service` 服务配置说明，`[Service]` 段
* `man systemd.exce` 服务启动配置，`[Service]` 段
* `man systemd.socket` socket 类型配置，`[Socket]` 段
* `man systemd.timer` 定时器类型配置，`[Timer]` 段

同时也要注意当前系统所使用的 `systemd` 版本，高版本更新可能支持更多的配置字段
及可选值，也就有更多功能，所以查阅当前版本配套的 `man` 手册是最权威的。

## 参考资料

* https://lloydrochester.com/post/unix/systemd_sockets/
* https://lloydrochester.com/post/unix/systemd_timers/

