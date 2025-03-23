@echo off
REM 进入正确的目录
cd /d "%~dp0"

REM 清理和编译
mingw32-make clean
mingw32-make

REM 运行命令行测试程序
echo 开始运行进程管理测试程序...
process_cli.exe

REM 如果希望自动运行一系列命令，可以取消下面的注释
REM echo init > commands.txt
REM echo create test1 1 >> commands.txt
REM echo create test2 2 >> commands.txt
REM echo create-long test3 3 >> commands.txt
REM echo list >> commands.txt
REM echo schedule >> commands.txt
REM echo list >> commands.txt
REM echo exit >> commands.txt
REM type commands.txt | process_cli.exe
REM del commands.txt

pause 