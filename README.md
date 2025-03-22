# os_kernel
our job.

## 环境安装
首先我们需要输入安装包：
``` bash
sudo apt update
sudo apt install build-essential
sudo apt install nasm
sudo apt install grub-common
sudo apt install xorriso
sudo apt install mtools
sudo apt install qemu-system-x86
sudo rm -rf /snap/core20/
sudo apt install --reinstall libc6

```

大家可以去fork仓库然后新建文件夹，写下自己的那一部分。

直接构建并且运行make文件，也就是：

``` bash
make clean && make && make run
```

come on.

## 文件结构（并不可以及时更新）：


# 最重要！！！内核

我们的kernel/目录下，存放着有关整个系统的运行模块。比如我们的kernel.c，就是我们的主模块。大家写完某个模块之后需要耦合，就需要和kernel/文件夹下的某些模块耦合，比如shell。

所以大家要做的，就是去写自己的那一块~
比如我们设备管理，就是上面的drivers/的文件夹，下面存放我们的各种设备，比如内核等等。


## 中断处理

我们在arch/x86的目录下，设置了如此的文件结构：


## 文件管理

## 内存管理