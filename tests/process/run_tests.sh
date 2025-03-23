#!/bin/bash

# 进入正确的目录
cd "$(dirname "$0")"

# 清理和编译
make clean
make

# 运行命令行测试程序
echo "开始运行进程管理测试程序..."
./process_cli

# 如果希望自动运行一系列命令，可以取消下面的注释
# echo -e "init\ncreate test1 1\ncreate test2 2\ncreate-long test3 3\nlist\nschedule\nlist\nexit" | ./process_cli 