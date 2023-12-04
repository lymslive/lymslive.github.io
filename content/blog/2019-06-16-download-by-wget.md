+++
title = "linux 下载工具 wget 使用经验"
[taxonomies]
categories = ["工具使用"]
tags = ["wget"]
+++
<!-- # linux 下载工具 wget 使用经验 -->

## 简介

在 linux 系统中，有两款著名工具，curl 与 wget 可用于发送 http 网络请求及下载网
络文档。在最基本的用法中，这两个命令行工具在功能上似有重叠，都有非常多的选项参
数。但它们侧重点不同，curl 侧重实现单次网络请求，且有个孪生库 libcurl 可植入各
种编程语言。而 wget 就是侧重下载的纯命令行工具，默认将网络请求的回应文档保存为
本地文件，对于大文档支持断点下载，还支持递归下载整个网站至本地镜像，这就具备了
基础的爬虫功能！
<!-- more -->

用 curl 下载单个文件的默认行为与 wget 不同，它将回应输出至终端。要保存文件得指
定参数，例如：

```bash
$ wget http://www.expample.com/index.html
$ curl -O http://www.expample.com/index.html
$ wget -O filename.html http://www.expample.com/index.html
$ curl -o filename.html http://www.expample.com/index.html
```

前两个命令将按服务器文档名保存为 `index.html` ，后两个命令可显示指定保存为其他
文件名。注意最后一个 `curl -o` 是小写的 `o` 。

如果 wget 要将回应输出至终端，而不保存文件，则用 `-` 作为文件名，例如：

```bash
$ wget -O - http://www.expample.com/index.html
$ curl http://www.expample.com/index.html
```

本文剩余部分着重讲 wget 下载用法。详细参数选项请参考 `man wget` 文档。

## 单文件下载

下载单文件是 wget 默认行为，无需多讲。但是下载大文件（如软件安装包之类），加上
`-c` 选项可以支持断点下载，即如果下载到一半因网络中断或系统关闭重启等原因，利
用该 `-c` 可接着下载剩余内容，而无需重新下载。

```bash
$ wget http://www.expample.com/soft/big.tgz
$ wget -c http://www.expample.com/soft/big.tgz
```

也可以从 ftp 站点下载。另外，现在许多 http 服务器是由 cgi 脚本动态生成网页，使
用 `-E` 可将保存文件名改为 `.html` ，只要服务器响应内容类型确实是 `text/html`
之类。例如：

```bash
$ wget -E http://www.expample.com/cgi-bin/hot.cgi
$ wget -E http://www.expample.com/cgi-bin/hot.php
```

如果不加 `-E` 选项，这两个命令下载的内容将分别保存为 `hot.cgi` 与 `hot.php` 。
有了 `-E` 则保存为 `hot.html` 。当然也可以使用 `-O hot.html` 显示指定保存名。

## 避免重复下载

wget 默认使用网络 URL 开尾基部当作保存文件名，如果本地有同名文件存在，则自动加
上 `.1` 后缀改名。但是也有选项避免重复下载。

```bash
$ wget -N http://www.expample.com/file.html
$ wget -nc http://www.expample.com/file.html
```

用 `-N` 选项会比较本地的 `file.html` 与服务器上相应文档的时间戳，只有当服务上
的文件更新了才下载。这在从 ftp 站点更新文件更有用，因为现在的大多 htpp 服务器
都不只是发送静态文件了，动态生成的网页没有时间戳意义。

用 `-nc` 选项则无条件忽略下载本地已存在的同名文档。如果确信网站的文档很少更新
，或动态生成的文档始终内容相同，就没必要使用 `-N` 先检查时间了（因为这反而多了
一步 `HEAD` 请求），直接用 `-nc` 跳过。当然你得保证自己或其他程序不会意义向下
载目录添加无关但恰好同名的文件，那也有可能导致跳过下载。

## 批量下载

如果一次性想下载多个文档，固然可以直接在命令行参数末尾添加多个 URL ，但更方便
的做法是先搜集 URL 地址保存在一个文件中，再用 `-i` 选项指定该文件。

```bash
$ wget -i links.txt
$ cat links.txt | wget -i -
```

这里参数文件 `links.txt` 的内容格式就是每行代表一个 URL。如果文件参数用 `-` 则
表示从标准输入读取，如此可与管道连用，由其他程序或脚本生成一系列 URL 列表，再
传给 wget 下载。

另一种常见情形是参数文件保存的不是单纯的一行行 URL ，而是规范的 html 文档，其
中包含一些 `<a>` 链接。如此可添加 `-F` 选项让 wget 解释该 html 文档，提取其中
`<a>` 元素的 `href` 属性，下载相应的 URL 文档。如果其中包含相对链接，还可以加
入 `-B url` 选项指定基础网址。相当于在该 html 文档头部加入了 `<base href="url">`
声明。当然如果源文档已有 `base` 声明，就无需 `-B` 选项。另外，文件名不需一定以
`.html` 结尾，只要文件内容是规范的 html 文档。

```bash
$ wget -F -i index.html
$ wget -F -B http://www.example.com/ -i index.html
```

## 递归下载网站

既然 wget 能够解释一个本地 html 文档，提取其内的超链接批量下载更多网络文档，那
将该工作流程反复推进一步，就能实现递归下载了。`-r` 选项就是开启递归下载模式，
可以很轻易地下载整个网站（当然会受限于目标网站服务器的反爬虫技术配置）。

```bash
$ wget -r http://www.example.com/
$ wget -rp http://www.example.com/
$ wget -rp -l 5 http://www.example.com/
```

递归下载另有一个关键的选项参数，`-l` 表示递归深度，默认是 5 层，即从初始页面沿
着超链接最多扩展 5 层就停止收手了，避免可能的无限下载。`-l0` 表示只下载命令行
指定的初始页面，`-l1` 表示也同时下载能从初始页面直接访问到的链接。

另外一个有用选项 `-p` 表示下载页面所有需要的内容。这与 `-l1` 下载直接链接文档
不同，`-p` 还下载页面的其他内嵌元素，如图片，css 与 js 脚本等，目标是让下载的
文档可以离线浏览。意图下载整个网站时，`-p` 选项几乎也是必选的，故建议合并写成
`-rp` 。

```bash
$ wget -rp -np http://www.example.com/book/
$ wget -rp -L http://www.example.com/book/
```

选项 `-np` 表示只下载子网站的某个子目录。字母 `p` 是 `parent` 缩写，`-np` 是
`no_parent` 的意思。如上命令若初始参数是 `http://www.example.com/book/` 就只会
下载 `/book/` 子目录下的文档。`-L` 是相对链接之意，与 `-np` 选项意图类似，不过
它取决于下载文档内链接元素 `href` 的写法，只下载写成相对链接的 URL 。该参数不
再建议使用了，请只用 `-np` 。

如前所述，下载单文件时默认保存在当前目录。用 `-i` 批量下载时，也是当作许多并不
相关的单文档依次下载并保存在当前目录。但在递归下载时将要下载一批有组织的文档，
wget 也就默认会保持网站目录结构。首先在当前目录下以初始 URL 参数的域名建个目录
，如 `www.expample.com/` ，然后将本次递归下载的所有的（本域）文档都放在该目录
中，并按网站目录构造各层级子目录，一一对应。

但是可用 `-nH` 选项让 wget 不建立域名顶层目录，直接将网站的层级目录建立在当前
目录下。不过也建议先手动建一个（稍短）的新目录，进入该新目录再下载，避免与之前
工作目录中已有文件混在一起。例如：

```bash
$ mkdir exsite
$ cd exsite
$ wget -rp -nH http://www.example.com
$ wget -rp -nd http://www.example.com
```

如果用 `-nd` 选项代替 `-nH` ，则不仅不建立顶层域名目录，也不建立网站的次级目录
，也即将所有下载的文档都平坦地集中放在当前目录。如果这是符合工作所需，可加上该
选项试试。

递归下载网站时，网站内的跨域链接默认是不会下载的。否则如果某页有个指向百度或
google 的链接，那递归下载就扩大化了，可能下载到大量无关的网页与网站。但是如有
需要，可以加 `-H` 选项允许下载其他域名的网页，如此最好再加上 `-D` 选项明确指定
还接受哪些域名（可以是逗号分隔的多个域名）。例如：

```bash
$ wget -rp -H http://www.example.com
$ wget -rp -H -D images.example.com,www.other.com http://www.example.com
```

将网站下载到本地浏览，还另有一个需求，就是转换本地网页内的相互链接。用 `-k` 选
项可实现此目的，而大小的选项 `-K` 则在转换链接前备份网页文档，加个 `.orig` 后
缀。例如：

```bash
$ wget -rp -k http://www.example.com
$ wget -rp -k -K http://www.example.com
$ wget -rp -k -NK http://www.example.com
```

下载（备份）网站也许不是一次性的操作，后续也许需要更新式重下载，那就加上 `-N`
选项。因为 `-k` 转换链接后会修改文件的时间戳，最好再加上 `-K` 备份，那 wget 在
重新下载时就会拿备份的 `.orig` 原文档与服务器的新文档对比，确认有更新才下载。

如前所叙，另一个选项 `-nc` 是捡漏式地重新下载，因为若网站不是静态文件服务，很
可能时间戳不能良好维护。但该选项在递归下载时似乎会与 `-k` 选项冲突，如果用了
`-k` 转换本地链接，即使再加 `-nc` 还是会重复下载文档。不知这是 feature 还是
bug 。

`-k` 转换链接的另一个问题，是它要下载完所有文档才会修改本地链接，因为 wget 认
为只有到那时才能知晓哪些文档下载到本地了，哪些又没下载，才好有选择地修改指向已
下载的本地链接。这在下载非常大的网站时可能是个问题，因为一时会半会下载不完，又
或者下载到一半被网站管理员识破了爬虫，将其屏蔽了。

当然了，爬虫与反爬虫向来是斗智斗勇的故事。wget 在递归下载中也提供了一些选项使
之更像模拟浏览器访问。例如设置 user-agent （相当于浏览器类型）以及下载间隔：

```bash
$ wget -rp -U "Mozilla/5.0 Firefox/65.0" http://www.example.com
$ wget -rp -w1 http://www.example.com
$ wget -rp -w1 --random-wait http://www.example.com
```

`-w` 表示 wait ，成功下载一个网页后等多少秒再发起另一个请求，`--random-wait`
选项则更进一步，表示等待时间有点随机（平均期望还是 `-w` 指定的参数）。

此外，wget 下载也支持登陆验证、cookie 发送及 https 证书校验。这些概念略复杂，
具体要用到时请查手册，查相关选项参数的用法。

最后，在下载网站时若无特殊需求，也可直接用 `-m` 选项，它大约相当于 `-rp -l inf -N`
等参数的组合。

```bash
$ wget -m http:/www.example.com
```

## 后台下载

默认情况下，wget 在下载过程中，会向标准错误输出一些有关进度等的日志信息。如果
下载时间比较长，可以用 `-b` 将 wget 置于后台下载。此时将自动记录 `wget-log`
日志文件，例如：

```bash
$ wget -m -b http:/www.example.com
$ tail -f wget-log
```

后台下载的意义是，假使用 xshell 登陆远程服务器，执行 `wget -b` 后断开 xshell
链接，在远程服务器上的 wget 也将继续下载。如果没用 `-b` 选项，则 xshell 断开后
，wget 也将中断。虽然在下载大文件时可用 `-c` 继续下载，但 `-m` 下载网站时重新
下载会更麻烦些。

当默认 `wget-log` 日志文件存在当前目录时，会自动重命名 `wget-log.1` 等。不过也
可用 `-o logfile` （小写 `o` ）指定日志文件，这是覆盖式的，另有 `-a logfile`
选项是添加到已有日志文件末尾。

```bash
$ wget -m -b -o wget.log http:/www.example.com
$ wget -m -b -a wget.log http:/www.example.com
```

还有一个问题，wget 参数比较多，对于需要定制下载参数时，若每次在命令行输入大量
参数比较麻烦。一个解决方案是自行写个包装 `shell` 脚本，此时建议在脚本中都使用
长选项名如 `--output-file` `--output-document` 等，这比 `-o` `-O` 短选项更见名
思义，而命令行使用短选项是便于快捷输入。

另一个方案是使用 `.wgetrc` 启动配置文件。wget 的系统配置在 `/etc/wgetrc` ，用
户个人的配置在 `~/.wgetrc` 。wget 每次启动都会读取其中的配置，个人配置在系统配
置之后读取，所以若有冲突，个人配置会覆盖系统配置。

此外，还可以为某个下载任务定制配置文件，例如保存为 `example.wgetrc` ，然后在命
令行中使用 `--config-file` 指定启动配置文件（将略过 `~/.wgetrc`）：

```bash
$ wget --config=./example.wgetrc http:/www.example.com
$ wget --config=./example.wgetrc -e "extra-name=value" http:/www.example.com
```

如此可大量简化命令行参数，可惜 `--config` 没有短选项名。`wgetrc` 配置文件是简
单的 `name = value` 格式配置，支持 `#` 开始的行注释。可用的配置名大致与长选项
名类似，但拼写上可能不完全一致，使用时再核对文档或 `/etc/wgetrc` 示例配置。

命令行还另有一个选项 `-e` ，可以将本该写在 `wgetrc` 的配置项临时写在命令行参数
，这在需要额外补充配置又不想修改配置文件时有用，尤其是有些配置名没有相应的命令
行选项。

## 参考文献

* https://www.gnu.org/software/wget/manual/
* https://daniel.haxx.se/docs/curl-vs-wget.html
