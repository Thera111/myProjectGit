#!/bin/bash
# Demonstration script showing config file and Makefile usage

echo "=========================================="
echo "配置文件和 Makefile 功能演示"
echo "Configuration File and Makefile Demo"
echo "=========================================="
echo ""

# Step 1: Show help
echo "1. 显示 Makefile 帮助信息 / Show Makefile help"
echo "   命令: make help"
echo ""
make help
echo ""
echo "按回车继续... / Press Enter to continue..."
read -r

# Step 2: Build
echo ""
echo "2. 编译程序 / Build the program"
echo "   命令: make"
echo ""
make
echo ""
echo "按回车继续... / Press Enter to continue..."
read -r

# Step 3: Show config
echo ""
echo "3. 查看当前配置文件 / Show current configuration"
echo "   配置文件: config.txt"
echo ""
head -20 config.txt
echo "..."
echo ""
echo "按回车继续... / Press Enter to continue..."
read -r

# Step 4: Run with config
echo ""
echo "4. 使用配置文件运行 / Run with configuration file"
echo "   命令: make run-with-args INPUT=test_late_data.txt OUTPUT=demo_output.txt"
echo ""
make run-with-args INPUT=test_late_data.txt OUTPUT=demo_output.txt
echo ""
echo "按回车继续... / Press Enter to continue..."
read -r

# Step 5: Show output
echo ""
echo "5. 查看输出结果（前30行） / Show output (first 30 lines)"
echo ""
head -30 demo_output.txt
echo "..."
echo ""

# Step 6: Clean
echo ""
echo "6. 清理文件 / Clean files"
echo "   命令: make clean"
echo ""
make clean
echo ""

echo "=========================================="
echo "演示完成！/ Demo completed!"
echo "=========================================="
