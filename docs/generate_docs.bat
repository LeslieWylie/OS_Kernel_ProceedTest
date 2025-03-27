@echo off
REM 生成操作系统内核项目文档

echo ========================================
echo 操作系统内核文档生成工具
echo ========================================

REM 检查Doxygen是否安装
where doxygen > nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo 错误: 未找到Doxygen。请安装Doxygen后再运行此脚本。
    echo 下载地址: https://www.doxygen.nl/download.html
    exit /b 1
)

echo 正在清理旧的API文档...
if exist api\html rd /s /q api\html

echo 正在使用Doxygen生成API文档...
doxygen Doxyfile
if %ERRORLEVEL% neq 0 (
    echo 错误: Doxygen生成文档失败。
    exit /b 1
)

echo API文档生成完成，位于 api\html 目录。

echo 正在复制模块说明文档...
if not exist api\modules mkdir api\modules
xcopy /E /Y modules api\modules\
if %ERRORLEVEL% neq 0 (
    echo 警告: 复制模块文档时出错。
)

echo ========================================
echo 文档生成完成！
echo 您可以通过以下方式浏览文档:
echo  - API文档: api\html\index.html
echo  - 模块文档: api\modules\
echo ========================================

exit /b 0 