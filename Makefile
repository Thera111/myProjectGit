# Makefile for Hot Word Statistics System
# 热词统计系统构建脚本

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -I.

# 目标文件
TARGET = main
SOURCES = main.cpp
HEADERS = hotWord.cpp lateDataHandler.cpp cppjieba/Jieba.hpp

# 默认配置文件
CONFIG = config.txt

# 输入输出文件（可从配置文件读取，这里作为默认值）
INPUT = input1.txt
OUTPUT = output.txt

# ========== 主要目标 ==========

# 默认目标：编译程序
all: $(TARGET)

# 编译主程序
$(TARGET): $(SOURCES) $(HEADERS)
	@echo "正在编译 $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "编译完成！"

# ========== 运行目标 ==========

# 使用默认配置文件运行
run: $(TARGET)
	@echo "使用配置文件 $(CONFIG) 运行程序..."
	./$(TARGET)

# 使用指定输入输出文件运行
run-with-args: $(TARGET)
	@echo "使用参数运行: 输入=$(INPUT), 输出=$(OUTPUT)"
	./$(TARGET) $(INPUT) $(OUTPUT)

# 运行测试数据（正常有序数据）
test-normal: $(TARGET)
	@echo "测试正常有序数据..."
	@if [ -f test_normal_order.txt ]; then \
		./$(TARGET) test_normal_order.txt output_normal.txt; \
	else \
		echo "注意: test_normal_order.txt 不存在，使用 test_late_data.txt 代替"; \
		./$(TARGET) test_late_data.txt output_normal.txt; \
	fi
	@echo "输出文件: output_normal.txt"

# 运行测试数据（轻度乱序数据）
test-late: $(TARGET)
	@echo "测试轻度乱序数据..."
	./$(TARGET) test_late_data.txt output_late.txt
	@echo "输出文件: output_late.txt"

# 运行测试数据（严重乱序数据）
test-severe: $(TARGET)
	@echo "测试严重乱序数据..."
	./$(TARGET) test_late_data_severe.txt output_severe.txt
	@echo "输出文件: output_severe.txt"

# 运行所有测试
test-all: test-normal test-late test-severe
	@echo "所有测试完成！"

# ========== 清理目标 ==========

# 清理编译生成的文件
clean:
	@echo "清理编译文件..."
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.out
	rm -f *.exe
	@echo "清理完成！"

# 清理所有输出文件（包括测试输出）
clean-all: clean
	@echo "清理所有输出文件..."
	rm -f output_*.txt
	@echo "清理完成！"

# ========== 帮助信息 ==========

# 显示帮助信息
help:
	@echo "热词统计系统 Makefile 使用说明"
	@echo "================================"
	@echo "可用目标："
	@echo "  make              - 编译程序（默认）"
	@echo "  make all          - 编译程序"
	@echo "  make run          - 使用默认配置文件运行"
	@echo "  make run-with-args INPUT=<file> OUTPUT=<file> - 使用指定参数运行"
	@echo "  make test-normal  - 测试正常有序数据"
	@echo "  make test-late    - 测试轻度乱序数据"
	@echo "  make test-severe  - 测试严重乱序数据"
	@echo "  make test-all     - 运行所有测试"
	@echo "  make clean        - 清理编译文件"
	@echo "  make clean-all    - 清理所有文件（包括输出）"
	@echo "  make help         - 显示此帮助信息"
	@echo ""
	@echo "示例："
	@echo "  make                              # 编译程序"
	@echo "  make run                          # 运行程序"
	@echo "  make run-with-args INPUT=input2.txt OUTPUT=out2.txt"
	@echo "  make test-all                     # 运行所有测试"
	@echo "  make clean                        # 清理编译文件"

# ========== 调试目标 ==========

# 使用调试模式编译
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(TARGET)
	@echo "调试版本编译完成！"

# 检查代码（编译但不链接）
check:
	@echo "检查代码语法..."
	$(CXX) $(CXXFLAGS) -fsyntax-only $(SOURCES)
	@echo "语法检查通过！"

# 显示编译器版本
version:
	@echo "编译器版本信息："
	$(CXX) --version

# ========== 伪目标声明 ==========
.PHONY: all run run-with-args test-normal test-late test-severe test-all clean clean-all help debug check version
