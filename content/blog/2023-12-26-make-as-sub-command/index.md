+++
title = "make 不干编译事的清奇妙用：子命令模式脚本"
draft = false
[taxonomies]
categories = ["工具使用"]
tags = ["linux","make","bash"]
[extra]
toc = true
+++

## 引言

说起 make ，大家的印象想必都是 Linux 下用于编译构建 C/C++ 项目的原始工具。后来
随着 cmake 及其他多种构建工具的发展与流行，手搓 makefile 的情况应该越来越少
了。而且除了 cmake ，有些构建工具是根本不必生成 makefile 就直接自己处理依赖编
译项目了，这比借助 makefile 路径依赖还更高效些。于是 make 在构建系统的作用与地
位就逐渐淡出了。

不过再追溯一下 make 本源，它似乎并不是专为编译构建而诞生的。早期 Linux 的许多
经典工具都只有简单到令人吃惊的小功能，贯彻着“一个程序只做好一件事”的哲学思想。
所以 make 的核心功能也非常简单，就只是检测目标与依赖（文件）的关系，当依赖有更
新时，执行相应的命令让目标也得以更新。而这个功能，明面上完全看不出来它与用 gcc
编译 C/C++ 项目有什么必然联系。只过在 Linux 上工作的程序员先驱们，竟然能在这么
简单的 make 基础上，发展出完备而复杂的构建系统，也是令我们后学惊叹的。

所以我有时在想，在如今不一定需要用 make 来构建 C/C++ 项目的时代，利用 make 的
核心功能与思想，还能否玩出其他有趣的花样？本文就介绍笔者在实际工作中遇到的一个
常用场景，利用 make 来组织一堆简单小脚本，提供统一命令行用法，让 make 命令华丽
转身，活像个能带自定义子命令的命令行工具。而子命令模式，正是当前流行的命令行工
作方式，像 git docker systemctl 等强大的流行工具就是子命令模式。

## 用 make 组织子命令脚本

### 典型开发场景的日常任务

先假设一个应该很常见的 Linux 开发场景。在开发一个 C++ 项目，也许是不大不小的微
服务。修改一部分代码后，首先要编译通过，至少保证没有语法错误，总不能依靠现代编
辑器的智能提示说它正确就信了。然后，抛开单元测试不谈，可能也要求在本地试下启动
能跑起来，尤其是涉及动态链接库的程序，能编译链接成功还不一定能保证能成功加载，
得在启动时才能报找不到符号之类的错误。也许 debug 版本就在本地跑个服务了，与其
他同事的服务联调。等差不多了，才打包部署到其他专门的机器上统一测试。

乍听起来，这好像是持续集成与部署（CICD）的事儿。但 CICD 系统一般是从 git 仓库
上拉代码下来再进行后续的一键编译部署吧，而新鲜出撸的代码没经过基本的验证就能提
交 git 的吗？这里主要想说的是，AI 取代程序猿开发的梦想仍然很远，我们在日常开发
中仍然有很多重复、琐碎的任务要做。这些任务或许只是敲一条或几条 bash 命令的事，
如果总是涉及多条命令，封装为一个简短的 bash 脚本是个好主意。所以，总结一下典型
开发场景，可能涉及如下一些日常任务，并且暂用 bash 脚本补充命名：

* 编译：build.sh
* 启动：start.sh
* 停止：stop.sh
* 重启：restart.sh
* 查看日志：lastlog.sh
* ……

### Makefile 统一管理

为每个任务写几行小脚本，是个可行方案，但也有些不足问题。比如这些小脚本放哪就是
个事。太多小脚本放项目根目录总归不好，单独放在 `script/` 子目录会好些，只是调
用时要多敲些字符。然后可用一个 makefile 来管理这些小脚本，最简单的示例如下：

```make
.PHONY: build start stop restart log

build:
    script/build.sh

start:
    script/start.sh

stop:
    script/stop.sh

restart:
    script/restart.sh

log:
    script/lastlog.sh
```

将每种任务当作目标，不写依赖，那 make 它时必然会调用下面的命令。`.PHONY` 的作
用是声明伪目标，这样 `make build` 时，就不会检查是否存在名叫 `build` 的文件或
目录，直接调用命令。如果 `build` 不是伪目标，当 make 发现存在这个文件或目录时，
就认为该目标是最新的，不会执行命令。

在很多情况下，重启的操作，应该就是简单的先停止，再启动。那么 `restart` 目标在
makefile 可以改写如下，明确这种顺序“依赖”关系，不需单独的脚本：

```make
restart: stop start
```

还可以根据需要串联更多的动作。比如，在改过一些代码后，希望编译、重启、查看日志
一键完成，那么可以加个目标，`all` 也许就合适，改动如下：

```make
.PHONY: all build start stop restart log

all: build restart log

build:
    script/build.sh

start:
    script/start.sh

stop:
    - script/stop.sh

restart: stop start

log:
    script/lastlog.sh
```

现在 `all` 目标写在最前面了，所以在无参数执行 `make` 就会默认执行第一个目标，
即等效于 `make all` 。另外，`stop` 目标的命令前加了一个短横 `-` ，它表示允许失
败的意思。即假如服务不在运行时，执行 `stop.sh` 脚本可能是非 0 退出，当然也可以
把 `stop.sh` 写得更优雅点，没什么可停止时也能正常退出。

### 帮助信息 make help

最后，还可以为这组 makefile 脚本加个帮助说明，并且在空参数 `make` 不默认执行
`make all` ，而是 `make help` 打印说明，则可以加个 `help` 目标：

```make
.PHONY: help all build start stop restart log

help:
all: build restart log

# ...

help:
    @echo make build: 编译项目
    @echo make start: 启动服务
    @echo make stop:  停止服务
    @echo make restart: 重启服务
    @echo make log: 查看最近的日志
    @echo make all: 一键编译、重启、查看日志
    @echo make help: 打印这些帮助信息
```

打印帮助的命令可以写在文件最后，但若要成为默认目标，需要在前面添加一行 `help:`
不带命令块，相当于提前声明，后面再定义。当然也可以直接将定义写在最前面，看个人
喜好。`@echo ...` 命令前面加 `@` 前缀是表示不回显命令本身。对比常规的目标命令，
比如 `make build` ，会在终端打印它执行的命令即 `script/build.sh` ，但 `echo`
命令，没必要把自己多打印一次，只打印 `echo` 的结果（即其后参数）即可。

## 子命令脚本优化探讨

用上 make 统一管理后，再回头审视一下各个任务（子命令）脚本的编写，或可作些优化。

### 构建 make build

先看构建目标 `build` ，如果项目源代码是用 cmake 工具链编译的，那其实就典型的几
句话，或可不必额外写 `build.sh` 了：

```make
build:
    @mkdir -p build
    cd build
    cmake ..
    make
    make install
```

如果也就用 make 编译，那更简单，不过并不适合直接放在根目录下的这个主控 makefile 。
假如在 `src/` 子目录有专门一个 makefile 来编译，则可写成：

```make
build:
    make -C src
```

如果编译的 makefile 也想放在根目录，那要改个文件名，比如叫 `build.mk` ，则写成：

```make
build:
    make -f build.mk
```

选项 `-f` 指定读取的 makefile 文件，默认就是名字为 `makefile` 或 `Makefile` 的
那个。`-C` 是切换当前工作目录，再解析 makefile，当前工作目录对解析相对路径有影
响。

### 运行 make start stop

假如在构建阶段，将可执程序放到了 `bin/` 子目录下，那么 `start` 目标也不外是执
行这个程序，当然，服务程序可能要用 `nohup` 在后台启动：

```make
SERVER_NAME=my-program
start:
    nohup bin/$(SERVER_NAME) &
```

这里还为服务名定义一个变量，以便能多处复用。

如果服务依赖的一些动态链接库（自己项目内编译的或第三方的）放在了 `lib/` 子目录，
则还要添加 `LD_LIBRARY_PATH` 。为当前命令添加环境变量可以这么写：

```make
start:
    LD_LIBRARY_PATH=./lib nohup bin/$(SERVER_NAME) &
```

如果启动阶段还有其他较为复杂的定制步骤，就写在单独的 `script/strat.sh` 也好。

停止服务最简单粗暴的方法是 `kill` 大法，虽不推荐但可作为示例。如果服务启动会写
个 pid 文件，可以这么写：

```make
stop:
    kill -9 `cat $(SERVER_NAME).pid`
```

不写 pid 文件的话，可以借鉴手动 `ps | grep` 大法列出，再用 `sed` 或 `awk` 提取
pid 。这可能就有点丑陋与复杂了。

### 使用 systemd 管理服务运行

事实上，我更推荐用 `systemd` 来管理服务的启动与停止等操作。这需要先配置
my-program.service 单元文件，这不是本文件的重点，但不妨先假设配好了，就可以用
`systemctl` 命令来管理服务了。那就可以改写 `start` 与 `stop` 目标的实现了，或
者若不想覆盖之前的，也可加点后缀另起个目标名字，如：

```make
start.sd: 
    sudo systemctl start $(SERVER_NAME)

stop.sd: 
    sudo systemctl stop $(SERVER_NAME)

status.sd: 
    systemctl status $(SERVER_NAME)

restart.sd: 
    sudo systemctl restart $(SERVER_NAME)
```

以上，相当于用 `make start.sd` 来简化 `sudo systemctl start ...` 的命令行输入。
正常来说，`systemctl` 命令需要 root 权限，所以注意要加 `sudo` ，不过 `status`
命令可不用 `sudo` 毕竟是只读的。systemctl 也有 `restart` 子命令，但若没有特殊
配置，默认动作也就相当于先 stop 再 start 。

既然以上 `systemctl` 命令依赖配置文件 `/etc/systemd/system/my-program.service`，
那也可以将这依赖写在 `start.sd` 等目标之后，当第一次没配置就执行时肯定就会报错，
意味着提醒你该手动配置一次这个文件。再进一步，假设有许多微服务的 `.service` 配
置都长差不多，可以通过一个模板配置由简单脚本生成针对当前服务的配置。那就可以
把这些前置动作都写进 makefile 依赖：

```make
SYSTEMD_UNIT=/etc/systemd/system/$(SERVER_NAME).service
$(SYSTEMD_UNIT): script/systemd-service-generator.sh script/systemd-template.service
    script/systemd-service-generator.sh script/systemd-template.service
# 假设脚本先将 .service 配置生成在当前目录，再开 sudo 移动到 /etc/...
    sudo mv $(SERVER_NAME).service $(SYSTEMD_UNIT)
    sudo systemctl daemon-reload

start.sd: $(SYSTEMD_UNIT)
    sudo systemctl start $(SERVER_NAME)
stop.sd: $(SYSTEMD_UNIT)
    sudo systemctl stop $(SERVER_NAME)
# ...
```

这样，假设后来若有需要优化 `.service` 配置，修改了生成脚本或模板，那也只要执行
`make restart.sd` ，它前面的依赖配置也会自动再生成更新。

### 打包任务 make tar

现在假设在开发本地自测验证过了，需要打个压缩包给出去。如果已经写过脚本来做这事，
那也只是在 makefile 中加一个目标的事：

```make
tar:
    script/tar-pack.sh
```

或者在简单的情况下，根本用不着打包脚本，就一个 `tar` 命令行的事，也可写进
makefile 以便更简单地输入 `make tar`：

```make
tar:
    tar czvf $(SERVER_NAME).tar.gz bin/ lib/
```

这里是简单假设打包 `bin/` 与 `lib/` 两个子目录。如果情况稍复杂点，比如还要打包
配置文件，或只要打包 `bin/` 或 `lib/` 下面部分文件，例如想排除单元测试程序……
如此，这个单行 `tar` 命令就会变得复杂起来，尤其是后面还改动时，单长行修改也对
`git diff` 不友好。

现在用 make 的思路来分析打包这个任务。将打包过程分成两个阶段会更清晰，先是将要
打包的各个文件拷到一个临时目录，再调用 `tar` 对这个临时目录整体打包。这就对应
着 make 的两个目标，写 makefile 时对目标顺序无关，故也可自底而上写，先写后一步
简单的：

```make
.PHONY: tar

TAR_DIR=$(SERVER_NAME)-release
TAR_NAME=$(SERVER_NAME)-release.tar.gz

tar: $(TAR_NAME)
$(TAR_NAME): $(TAR_DIR)
    tar czvf $(TAR_NAME) $(TAR_DIR)
```

以上，假设待打包的“临时”目录叫 `release` ，并且加上本服务名前缀，这是考虑将压
缩包拷出去后在别的机器上解压出来不能只叫光溜溜的 `release` 名字。伪目标 `tar`
依赖一个真实存在或将存在的压缩文件，而该压缩文件依赖一个目录，而这个目录应该依
赖待要拷入它的其他文件。据此可继续写 makefile 如下：

```make
# 待打包文件
FILE_LIST = \
    bin/my-program \
    lib/libmyutil.so \
    conf/my-program.ini \
    makefile

# 各打包文件添加目录前缀 -release
FILE_INTAR = $(patsubst %,$(TAR_DIR)/%,$(FILE_LIST))

# 每个 -release 目录内文件，依赖上层目录的源文件，拷贝更新
$(FILE_INTAR): $(TAR_DIR)/% : %
    cp $< $@

# 整个待压缩目录依赖拷入的每个文件
$(TAR_DIR): $(FILE_INTAR)
```

以上，每行语句添加了注释。我将待打包的每个文件显式列出来，没有用变量替换（如之
前有定义 `SERVER_NAME=my-program`），是考虑那几行文件名可当作文件清单的文档
参考，真实项目这个清单可能会更长些。唯一不爽的是限于 makefile 语法，每行末尾要
有反斜杠续行。

其后就是 makefile 函数与语法的典型应用了。其中 `cp $< $@` 那行命令，结构就很像
在写编译 C/C++ 项目的 makefile 时，将每个 `.cpp` 编译为相应的 `.o` 的命令，只
不过在那里用的是 `g++` 命令及一些复杂选项，这里只是简单的 `cp` 命令。将它展开，
就相当于如下几个目标与命令：

```make
my-program-release/bin/my-program : bin/my-program
    cp bin/my-program my-program-release/bin/my-program
# ...
my-program-release/makefile : makefile
    cp makefile my-program-release/makefile
```

是的，这里将主控 makefile 文件也打包进去了。虽然解压后没源码用不了 `make build`
，但还可用 `make start` 与 `make stop` 之类的呀，后者才是本文所讲的 makefile
核心功能。

在常规的打包脚本中，打包压缩后可能会将拷贝收集用的临时目录删掉。在这里我却觉得
没必要将 `my-program-release` 目录删掉，在 makefile 的依赖链控制下，它里面的文
件是能同步更新的，故可留着作为参考。

另外提一点，这里没特别在意通过 `make build` 编译后放在 `bin/` 目录下的可执行程
序是 debug 版还是 release 版，就直接打包了。如有需要，额外建个 `build.rel` 目
标也可，这里不再详细展开了。

这几段打包的 makefile 语句，可能有点偏长了，但是打包也非使用最频繁的命令，放在
主控 makefile 文件中如果觉得有喧宾夺主之嫌，则可以单独移到一个文件，比如命名为
`tar.mk` ，然后在主控文件中只留下一个 `tar` 目标转调：

```make
tar:
    make -f tar.mk
```

### 容器化应用任务

现在，假设要赶容器化的潮流，想把我们的服务程序也放在容器中去运行。容器运行的前
提是先要做个镜像。严肃点说，编译也该放到容器中去，更容易保证环境一致性。不过本
文为了故事的简单性，就仍用之前通过 `make build` 编译出的程序，打成镜像，也相当
于另一种方式的打包。

于是也就可再利用上节打包过程中建立的 `my-program-release/` 目录，这里收集了我
们程序运行所需的文件，拷进镜像就成了。也就说把这个目录当成 `docker build` 命令
的 context （路径参数），只把这个目录的内容发到 docker 引擎中作为构建镜像的原
料。这比把当前目录（整个项目根目录）发过去更高效，虽然也可以用 `.dockerignore`
忽略大量不必要文件（如 `src/` 与 `log/`），但仍不如搞一个清爽的目录。把这个目
录当成制作镜像的依赖，那就容易写出 makefile 了：

```make
IMAGE_TAG = $(SERVER_NAME):latest
image: $(TAR_DIR)
    docker build $(TAR_DIR) -t $(IMAGE_TAG) -f ./Dockerfile
```

而 Dockerfile 的内容也可以很简单，如：

```dockerfile
FROM contos:7
RUN mkdir /workspace
COPY ./* /workspace
WORKDIR /workspace
```

就是选个操作系统镜像基底，在根目录下建个 `/workspace` 作为容器的工作目录，再把
context 即 `my-program-release/` 目录下的所有内容拷到镜像的 `/workspace` 中。
当然可以根据项目需要在 Dockerfile 中添加更多内容，这不多说。其实笔者向来推荐把
Dockerfile 本身也打进镜像中，将来运行容器时可以进去查看到这个文件，作为参考大
致了解该镜像是怎么来的。于是，可以把 Dockerfile 就放在 my-program-release 子目
录，而不是放在项目根目录。这样，在上面的 `make image` 目标下的 `docker build`
命令就不必加 `-f ./Dockerfile` 参数了。

另一个问题，`image` 目标依赖的 `TAR_DIR` ，也即 -release 目录的依赖与准备，若
按前面的叙述，分离到单独的 `tar.mk` 文件中了，在主控 makefile 文件中看不到它了
。有两个解决方案，一种是在 makefile 用中 `include tar.mk` 命令将那个文件包含进
来；一种是将 `image` 目标也写到 `tar.mk` 中，然后在 makefile 转调命令
`make image -f tar.mk` 。

镜像做好之后，需要先本地启个容器来测试。也就是说 `make start` 时不再跑 `bin/`
目录下的程序，而是运行容器，执行容器中的 `/workspace/bin/my-program` 。可以先
建个 `start.docker` 目标来表示这种启动方式，大概写法如下：

```make
CONTAINER_NAME = --name $(SERVER_NAME)
CONTAINER_VOL = -v log:/workspace/log
CONTAINER_CMD = -d bin/$(SERVER_NAME)
start.docker:
    docker run $(CONTAINER_VOL) $(CONTAINER_NAME) $(IMAGE_TAG) $(CONTAINER_CMD)
```

容器启动往往涉及相当多的选项，为此我们定义几个变量，最后组成 `docker run` 命令
行参数，主要包括如下几部分：

* -v 挂载卷，把日志目录挂载为当前的 log/ 子目录，
* --name 为容器取个名字，同服务程序名，方便其他 docker 命令引用
* 镜像名
* 要在容器中执行的命令，让程序在容器内的前台运行即可，不需 `nohup ... &`，
  `-d` 是 `docker run` 的选项，将容器置于后台运行。

在容器中跑的服务，一般还会涉及端口映射，如用 `-p` 选项，这里也不多说了。

与 `start` 相对应的 `stop` 任务就简单多了，调个 `docker stop` 的事：

```make
stop.docker:
    docker stop $(CONTAINER_NAME)
```

在本地的单机容器通过基本的验证测试后，就可以通过 `docker push` 推送到镜像仓库
了，这个命令与 `git push` 很像，对于商业项目，应该是推送到公司内部的镜像仓库。
这个目标任务的实现也简单：

```make
DOCKER_REGISTRY = my.company.com/my-project
IMAGE_REMOTE = $(DOCKER_REGISTRY)/$(IMAGE_TAG)
push.docker:
    docker tag $(IMAGE_TAG) $(IMAGE_REMOTE)
    docker push $(IMAGE_REMOTE)
```

这里需要配置镜像仓库地址 `DOCKER_REGISTRY` ，用 `docker tag` 命令为本地镜像多
打个新标签，加上仓库地址前缀，然后用 `docker push` 推送新标签。另外提一下，这
里的镜像标签中作为版本的后缀都是默认的 `:latest` ，这在严肃项目中需要对镜像作
版本管理区分时是不良实践。所以在推送 `:latest` 之后，最好额外打个有版本标识的
标签并推送，一般可用时间或 git 最近提交的 hash 码作为版本标识。比如增改如下：

```make
DOCKER_REGISTRY = my.company.com/my-project
CUR_TIME = $(shell date +%s)
IMAGE_REMOTE = $(DOCKER_REGISTRY)/$(IMAGE_TAG)
IMAGE_VERSION = $(DOCKER_REGISTRY)/$(SERVER_NAME):$(CUR_TIME)
push.docker:
    docker tag $(IMAGE_TAG) $(IMAGE_REMOTE)
    docker push $(IMAGE_REMOTE)
    docker tag $(IMAGE_TAG) $(IMAGE_VERSION)
    docker push $(IMAGE_VERSION)
```

### 伪目标的 touch 文件

上节的容器任务，还遗留了一个问题，如何避免重新制作镜像与推送镜像？对比打 `tar`
包就不存在这个问题，因为会生成一个 `.tar.gz` 文件，能与 `-release` 目录比较更
新时间，所以在执行 `make tar` 时只会在需要重新打包时才会执行命令。而 `image`
应该是个伪目标， make 它时不会在当前目录生成文件，生成的镜像被 docker 引擎统一
管理，不是很方便追踪它的更新时间。

为解决伪目标的这个问题，可以在执行完命令后 `touch` 一个文件，专门用于标记最近
执行目标的时间，可用以与依赖比较时间。当然这还需要一个中转，可改写如下：

```make
TOUCH_DIR = .touch
$(TOUCH_DIR):
    mkdir -p $@

image_touch = $(TOUCH_DIR)/image.touch
image: $(image_touch)
$(image_touch): $(TAR_DIR)
    docker build $(TAR_DIR) -t $(IMAGE_TAG) -f ./Dockerfile
    @mkdir -p $(TOUCH_DIR)
    touch $@
```

以上，先规划建一个 `.touch/` 子目录，专门用于收集 `touch` 文件，隐藏目录，平时
眼不见为净。然后让伪目标 `image` 依赖真实文件 `.touch/image.touch` ，而后者，
就是之前第一版写的 `image` 目标与命令实现，只不过之后在执行完 `docker build`
后加条 `touch .touch/.image.touch` 命令，为伪目标 `image` 更新对应的 touch 文
件时间。使用时，仍然执行 `make image` ，只有当检测到 touch 文件比打包目录更旧
了，才需重新制作镜像。

推送镜像也同理可改造一下，它依赖的目标是制作镜像：

```make
push_touch = $(TOUCH_DIR)/push.touch

push.docker: $(push_touch)
$(push_touch): $(image_touch)
    docker tag $(IMAGE_TAG) $(IMAGE_REMOTE)
    # ...
    touch $@
```

这样，在已经推送过最近制作的镜像时，再（由于忘记或不确定时）重复执行
`make push.docker` 是不会触发重复推送的。

其他伪目标，如有需要，都可以按这种方式加个对应的 touch 文件。是否需要重构依赖
链都另说，至少可以作为一个曾经执行与最近执行的证据。比如 `make start` ，就可以
加条 `touch .touch/start.touch` 命令，标记启动时间。当然，如果服务本身会写 pid
文件，pid 文件也是启动时间的一个标记。

顺便提一下，若从头开始只用 `touch` 一个不存在的文件，它只会创建 0 字节的空文件，
0 字节文件不会占用额外硬盘空间，只是在父目录的表项中多添加一记录，相当于只修改
父目录这个“文件”。当然了，现在的硬盘并不值钱，并不需要刻意追求 0 字节文件，所
以也可能改用 `echo ... > .touch/target.touch` 代替 `touch` 命令往目标的 touch
文件写入一些你觉得有意义的内容。

### 多种实现方式的切换

至此，我们在 makefile 中为服务程序先后实现了几种启动方式：

1. 用 bash 脚本或命令行在本地启动；
2. 用 systemctl 启动，纳入 systemd 服务管理系统；
3. 用容器启动。

显然，在某一时期，或某个环境中，你只会用到其中一种方式启动服务。现在，第 1 种
最原始的方式占用了最简短的 `start` 目标，好像不妥当。所以再作个优化，将第 1 种
`start` 改名为 `start.bash` ，再重新建一个 `start` 目标，让它依赖以上三者之一
，例如，默认使用第 3 种的容器启动：

```make
start.bash:
    # script/start.sh ...

start.sd:
    # systemctl start ...

start.docker:
    # docker run ...

start: start.docker
```

这种依赖关系，如果不是伪目标，而是当成真实文件看，就有点像软链接的味道了，相当
于这样：

```bash
ln -s start.docker start
```

停止服务的伪目标，也可以作类似的“软链接”处理。

至此，小结一下，这里讨论的名为 `my-program` 的示例项目文件与目录结构大致如下：

```
my-program/
  - .touch/
  - build/
  - scr/
  - script/
  - bin/
  - lib/
  - log/
  - my-program-release/
  - my-program-release.tar.gz
  - makefile
  - tar.mk
  - CMakeLists.txt
  - Dockerfile
```

其中，`makefile` 文件就是本文重点讨论的，把 make 当作能自定义子命令的管理脚本
或主控脚本，它可能会调用另一个 makefile 文件 `tar.mk` 及 `script/` 目录下的脚
本。一些伪目标的执行，可能在隐藏目录 `.touch` 下更新相应的 touch 文件的时间戳。

## Makefile 脚本常用技巧

从核心功能与原理上讲，makefile 可以是简单的。但随着目标的广度与依赖的深度增加，
makefile 也可能会逐渐变得复杂，尤其是 makefile 也支持变量、函数、条件，甚至循
环等常规编程元素。关于具体的语法，本文不想深入讲太细，请在需要时搜索相应的参考
文档。除了建议对 makefile 保持简单、控制规模外，下面再讲一些笔者认为比较重要的
几个点。

### 调试手段

首先，从脚本编程角度看，大多语言都应该有只检查语法而不执行的手段。make 也有个
`-n` 选项，它只打印目标将要执行的命令，而不实际执行。它在分析 makefile 中目标
依赖关系过程中，自然也能检测出语法错误，而“业务”逻辑错误，则要从它打印的命令
人工分析了，看它是否符合预期想执行的命令序列。

```bash
# 打印默认目标将要执行的命令
make -n

# 打印指定目标将要执行的命令
make -n target

# 检查打印命令无误时，再去掉 -n 实际执行
make target -n
make target
```

按命令行程序解析参数的习惯，`-n` 选项位置无关，我建议写在最后，这更方便输入命
令行过程中的顺手修改。

如果指定的目标按 make 的依赖逻辑“最新的”，`make -n` 也能指出这点，不会打印任何
命令。如果目标依赖链缺失，它能报告错误，这也是重要的诊断信息。例如，我们在前文
介绍的最“复杂”的目标，应该是打包任务。如果从代码库拉下新鲜代码还没编译前就测试
这个目标：

```bash
make tar -f tar.mk -n
```

它就会报错。大致原因是 `tar` 目标依赖 -release/ 目录下的文件，后者依赖编译结果
放在 bin/ 目录下的文件，而这些待打包的各个文件，我们没有在之前的 makefile 中给
出依赖，告诉 make 在缺失时该调用什么命令生成它们。所以 make 报告这是依赖错误。
这个错误可大可小，毕竟正常情况下，我们不会在编译之前就搞打包操作。只要先执行
`make build` 正常编译后，再测试 `make tar -n` 就不会报错了。

当知道这个错误根源后，要修改也就容易了，虽然实际上未必重要。只要增加待打包文件
依赖 `build` 目标就行：

```make
$(FILE_LIST): build
```

这句话告诉 make ，当你发现任何待打包文件缺失时，请先执行 `build` 目标。

其次，print 大法也是调试一切编程语言或脚本的简单粗暴姿式。makefile 也能用几个
内置函数来打印消息，例如：

```make
$(info will tar these files $(FILE_LIST))
$(warning this is just warning message)
$(error fatol error occurs and would exit)
```

其中，`error` 函数被调用时会视为错误，打印后退出 make ，所以这一般应放在条件判
断语句块中，或者可用于标记某个目标为弃用状态，不该再被执行。其他两个函数在打印
消息后会继续处理 makefile 语句，以调试为目的打印建议使用 `info` ，打字省流，不
过 `warning` 有个好处是还会打印所在文件名与行号。例如：

```make
$(warning target err should not be called)
$(info target err should not be called)
err:
    $(error this target is deprressed)
```

消息打印有个恼人的地方是，如果它不是放在条件语句块内，它总是会打印出来，这在调
试完后是每次 make 都可能出现的噪音。除了把打印注释掉，我还推荐一个办法，可以专
门设立一个打印目标，把当前 makefile 中用到的关键变量打印出来，例如：

```make
echo:
    @echo CUR_TIME = $(CUR_TIME)
    @echo FILE_LIST = $(FILE_LIST)
    @echo FILE_INTAR = $(FILE_INTAR)
```

这样，执行 `make echo` 就起到监视变量的作用了，这在你不确认某些变量操作时很有用。

### 参数传递

将复杂任务拆成几个小任务，是编程的另一个基本操作。makefile 的目标，也是一种细
粒度的拆分，或可类比为结构化编程语言最常见的函数或子过程，而目标之间的依赖，也
就相当于过程调用。例如再回顾前面的一个示例：

```make
all: build restart log
restart: stop start
```

当在 shell 中执行 `make all` 时，就相当于依次执行 `make build stop start log`
这几个目标。当其他语言将一个大任务拆成几个任务再组合起来时，那些子任务的执行顺
序也就固定了。而 make 还可以在命令行中单独激活中间某个或几个子任务，这种灵活性
也表明 make 是面向终端用户的。

在同一个 makefile 中，不同目标的过程调用，可认为不涉及参数传递，大家都使用全局
变量。当 makefile 规模增长到一定程度，可以根据业务关联度将一部分代码拆出为独立
的 makefile 文件，就如之前讨论的 `tar.mk` 文件。如果在主文件中用 `include` 指
令将子文件重新包含进来，那在 make 看来，还是同一个文件。

所以这里只讨论真正拆分 makefile 后，在主文件使用 `make -f sub-split.mk` 方式调
用子文件时的一些问题。就比如，之前的 `tar.mk` 文件中，也用到了 `SERVER_NAME`
这个变量，但是未定义。当然，可以在 `tar.mk` 补上定义，但重复代码总归不好。更好
方式是在调用时传递参数，可以这样写：

```make
tar:
    make -f tar.mk SERVER_NAME=$(SERVER_NAME)
```

在 make 的命令行参数约定中，`VARNAME=value` 形式表示在相应的 makefile 中增加变
量定义。这形式有点像环境变量定义，但也像是 `--option=argument` 形式省略了前缀
`--` 的简化。因为带 `-` 或 `--` 前缀的选项，是传递给 make 程序本身使用的，而不
是给 makefile 脚本使用的。另外，虽然 makefile 变量习惯用大写的居多，但使用小写
也没问题。

在上例中，`$(SERVER_NAME)` 先在当前 makefile 解析时被展开为它的值，所以传递给
`tar.mk` 的变量定义就类似 `SERVER_NAME=my-program` ，也就继承了主文件的变量值。
不过在 `tar.mk` 文件中完全没体现有这个变量定义，也是不好，在将来维护修改时可能
感觉困惑，并且无法单独使用。因此，推荐用 `?=` 给它一个默认值，如：

```make
# tar.mk
SERVER_NAME ?= my-program
```

这条语句，只在 `SERVER_NAME` 变量未定义时才执行，正好满足两方面的需求。

对于一些复杂任务，如果不适合削足适履用 makefile 来写，完全可调用一个 bash 脚本
或其他脚本程序来写，尤其是已经有脚本实现功能的情况下，只在 makefile 中为 make
注册一个快速目标入口就好，这可达到简化命令行输入的效果。

当被调用的其他（语言）脚本有处理命令行参数功能时，在 makefile 的某个目标下写命
令时也只要按其要求传入选项参数即可，可适当利用 makefile 的变量展开功能达到简化
与动态传参的效果，比如上文介绍的为 `docker run` 命令传参。

再举个简单例子，我们在监控日志时还经常需要根据某个关键字过滤，只打印匹配行，就
类似如下命令行的意图：

```bash
tail -f last.log | grep key
```

其中，最近的日志可用类似 `ls -rt log/ | tail -1` 找出，拼起来略有复杂，值得封
装成一个脚本，则该脚本显然应该接收一个参数表示 grep 什么 key 。简单实现如下：

```bash
#!/bin/bash
# file: script/lastlogf.sh

last=$(ls -rt log/* | tail -1)
tail -f $last | grep $1
```

直接从命令行调用大约如下，要求实时打印含 Error 的日志：

```bash
script/lastlogf.sh Error
```

如果在 makefile 中设立一个 logf 目标，可写成如下：

```make
logf:
    script/lastlogf.sh $(grepkey)
```

其中，`grepkey` 变量需要通过 `make` 命令行传参，写成如下：

```bash
make logf grepkey=Error
```

与前面直接调用 `script/lastlogf.sh` 输入量差似乎不多。不过当你习惯了 make 的这
种用法，之后专为 make 调用增加新脚本时，也可以借鉴 make 那种通过环境变量的方式
传参。例如，将脚本改写成：

```bash
#!/bin/bash
# file: script/lastlogf.sh

last=$(ls -rt log/* | tail -1)
tail -f $last | grep $grepkey
```

对比前后两个版本，可见就只有 `$1` 与 `$grepkey` 的区别，也就相当于位置参数与命
名参数的区别。它们各有优劣，本文不再额外讨论。此外，为简便起见，这两个版本都没
对参数判空处理，实际中在缺少（位置）参数或环境变量时，最好赋个默认值，否则会出
错。

对于后一个版本的 `lastlogf.sh` 脚本，makefile 为目标传参的写法是：

```make
logf:
    grepkey=$(grepkey) script/lastlogf.sh
```

通过命令行执行 make 的写法却不用变，仍然是：

```bash
make logf grepkey=Error
```

但若要从命令行直接调用脚本，就要写成：

```bash
grepkey=Error script/lastlogf.sh
```

在 bash 命令行中，将 `key=val` 等式写在命令之前，表示专门为该命令（将启的进程
）设置环境变量，而不影响当前 session 的环境变量。当然了，所谓污染当前会话的环
境变量的影响也许没那么大，如果不在意的话，也可以先 `export` 写成：

```bash
export grepkey=Error
script/lastlogf.sh
```

这样，当为 `grepkey` 导出环境变量后，就可免参调用脚本了。这里，我将“命名参数
”用小写字母表示，既为输入方便，也好与常规环境变量区分。

而在 makefile 中，也是能导出环境变量的，故也能这样写：

```make
# grepkey 的值由 make 的命令行参数设定
# export 只声明要将其导出为环境变量
export grepkey
logf:
    script/lastlogf.sh
```

只不过在 makefile ，由于只要一次写定，所以按之前的写法，为单行命令局部导出环境
变量可能是更优的实现。另外，在输入 make 的命令行参数时，其实也可以将“命名参数
”写在 `make` 前面，如：

```bash
grepkey=Error make logf
```

这种写法，也是为 make 进程设定环境变量。只不过在随后 make 进程解析 makefile 时
，makefile 定义的变量与环境变量几乎是可互用的。但是很显然，“命名参数”前置的
写法看起来有点奇怪，make 能将其后置，才是更符合直觉的写法。

这里讨论这一番，主要是想表达一个观点，当要开发 bash 脚本与 makefile 联用时，采
用环境变量的“命名参数”法会使风格会更统一。在 bash 脚本，引用环境变量是很方便
的，对比其他通用脚本语言要获取环境变量可能还要经过库函数调用。不过其他编程语言
，也大多有很好用的命令行参数解析库，能支持长横短横引导选项，有参无参混合等复杂
情况的解析。而 bash 脚本，虽然也有内置库函数能解析命令行参数，但我觉得很不优雅
，不如就用环境变量，再通过 make 中转一下，就妥妥的命名参数的风格。

如果觉得给 make 提供命名参数，仍然要多敲不少字符，那还有一种情况，可以根据脚本
最常用参数，在 makefile 中为脚本的典型用法设立不同的目标，以达到简化输入的目的
。

例如，也是常见的任务，假如开发机硬盘空间告急，需要删除旧日志文件。虽然写个定时
任务也是个可行方案，但是只在必要时手动删也是个不同的选择。思路也简单，先用
`find` 命令找到更新时间在几天的日志文件，再用 `rm` 删除之。但是我从来记不住
find 的具体用法，所以把它封装成 bash 脚本，通过命令行输入“多少天前”的参数。
脚本实现且不论，就说写好了取名为 `rmlog.sh` 吧。然后在 makefile 中建立三个目标
，分别表示删除 1 天前的日志，删除 7 天前的日志，与删除 30 天前的日志：

```make
log.rm1:
    script/rmlog.sh 1
log.rm7:
    script/rmlog.sh 7
log.rm30:
    script/rmlog.sh 30
```

使用时就只要执行类似 `make log.rm7` 的命令。仅管脚本支持输入任意正整数表示几天
前，但我表示没必要，有这三档删除给我日常使用就够了。而且 makefile 屏蔽了脚本对
参数格式的具体要求，即使后来 `rmlog.sh` 优化升级了，增加了许多高级功能，也只要
在 makefile 中对传参作适配，不影响 make 的终端用户。即使偶尔用户有输入非常规参
数的需求，也可以用 `make -n` 把示例命令打印出来，让用户拷下来，在那基础上改改
参数值，并不太需要记忆背后脚本的参数格式约定。

## 结语

本文结合 Linux 开发的日常任务，介绍与探讨了将 make 当作自定义子命令的管理工具
的清奇用法。核心要点包括：

* makefile 设定的目标成为 make 可执行的子命令；
* makefile 建立的目标依赖代表了 make 子命令的顺序与组合；
* make 子命令以伪目标为主，可以更新对应的 touch 文件标记时间及传递依赖；
* 可以利用 makefile 的诸多编程特性，从简单的变量与函数，到复杂的分支与循环；
* 目标指令可调用其他 makefile 或脚本，简化常用任务的命令行输入。

通过 makefile 及其组织、管理的一系列可大可小的脚本，能够简捷地在 bash 脚本层面
实现子命令模式，从而让 make 这个古老的工具焕发现代的气息。而若想使用纯 bash 脚
本实现子命令模式，那可能是相当复杂与困难的。

## 附录：参考链接

* GNU make 官方手册 [https://www.gnu.org/software/make/manual/html_node/index.html](https://www.gnu.org/software/make/manual/html_node/index.html)

## 附录：本文集源码

依 makefile 的语法要求，目标下面的命令行是制表符缩进，网页渲染的代码很可能转为
空格，不能直接复制使用，故提供本文的参考源码下载。

[makefile](makefile) [tar.mk](tar.mk)
