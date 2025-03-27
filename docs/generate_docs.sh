#!/bin/bash
# 生成操作系统内核项目文档

echo "========================================"
echo "操作系统内核文档生成工具"
echo "========================================"

# 检查Doxygen是否安装
if ! command -v doxygen &> /dev/null; then
    echo "错误: 未找到Doxygen。请安装Doxygen后再运行此脚本。"
    echo "安装指南: sudo apt-get install doxygen graphviz (Ubuntu/Debian)"
    echo "或: sudo yum install doxygen graphviz (CentOS/RHEL)"
    exit 1
fi

echo "正在清理旧的API文档..."
rm -rf api/html

echo "正在使用Doxygen生成API文档..."
doxygen Doxyfile
if [ $? -ne 0 ]; then
    echo "错误: Doxygen生成文档失败。"
    exit 1
fi

echo "API文档生成完成，位于 api/html 目录。"

echo "正在复制模块说明文档..."
mkdir -p api/modules
cp -r modules/* api/modules/
if [ $? -ne 0 ]; then
    echo "警告: 复制模块文档时出错。"
fi

echo "========================================"
echo "文档生成完成！"
echo "您可以通过以下方式浏览文档:"
echo " - API文档: api/html/index.html"
echo " - 模块文档: api/modules/"
echo "========================================"

exit 0 