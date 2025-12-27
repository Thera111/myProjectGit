# 快速开始指南 / Quick Start Guide

## 简介 / Introduction

本项目提供了配置文件和 Makefile 来简化编译和运行过程。

## 快速开始 / Quick Start

### 1. 编译程序 / Build

```bash
make
```

### 2. 运行程序 / Run

#### 方式一：使用默认配置
```bash
make run
```

#### 方式二：指定输入输出文件
```bash
make run-with-args INPUT=input2.txt OUTPUT=my_output.txt
```

#### 方式三：直接运行可执行文件
```bash
./main [输入文件] [输出文件]
```

### 3. 运行测试 / Run Tests

```bash
make test-all
```

### 4. 清理文件 / Clean

```bash
make clean        # 清理编译文件
make clean-all    # 清理所有文件（包括测试输出）
```

## 配置文件 / Configuration File

编辑 `config.txt` 来修改程序参数：

```bash
vim config.txt
# 或
nano config.txt
```

### 主要配置项 / Main Configuration Options

| 配置项 | 说明 | 默认值 |
|-------|------|--------|
| inputFile | 输入文件 | input1.txt |
| outputFile | 输出文件 | output.txt |
| enableLateDataHandling | 启用乱序处理 | true |
| allowedLateness | 最大延迟（秒） | 30 |
| windowSize | 时间窗口（秒） | 600 |

## 示例工作流 / Example Workflow

### 场景1：处理正常数据

```bash
# 1. 编译
make

# 2. 编辑配置
cat > config.txt << EOF
inputFile=input1.txt
outputFile=output.txt
enableLateDataHandling=false
windowSize=600
dictPath=dict/jieba.dict.utf8
modelPath=dict/hmm_model.utf8
userDictPath=dict/user.dict.utf8
idfPath=dict/idf.utf8
stopWordPath=dict/stop_words.utf8
EOF

# 3. 运行
make run

# 4. 查看结果
cat output.txt
```

### 场景2：处理乱序数据

```bash
# 1. 编辑配置启用乱序处理
cat > config.txt << EOF
inputFile=test_late_data_severe.txt
outputFile=output_late_handling.txt
enableLateDataHandling=true
allowedLateness=60
windowSize=600
maxBufferSize=10000
dictPath=dict/jieba.dict.utf8
modelPath=dict/hmm_model.utf8
userDictPath=dict/user.dict.utf8
idfPath=dict/idf.utf8
stopWordPath=dict/stop_words.utf8
EOF

# 2. 运行
make run

# 3. 查看结果
cat output_late_handling.txt
```

### 场景3：批量测试

```bash
# 运行所有预设测试
make test-all

# 查看各个测试结果
ls -lh output_*.txt
```

## 获取帮助 / Get Help

```bash
make help
```

## 详细文档 / Detailed Documentation

- 配置和构建详细说明：`README_配置和构建.md`
- 功能测试指南：`功能测试指南.md`
- 实现原理：`迟到乱序数据处理实现原理.md`

## 常见命令速查 / Common Commands Cheat Sheet

```bash
make              # 编译
make clean        # 清理编译文件
make run          # 运行（使用 config.txt）
make test-all     # 运行所有测试
make help         # 显示帮助
make version      # 显示编译器版本
make check        # 语法检查
```

## 故障排查 / Troubleshooting

### 编译失败
```bash
# 检查编译器
make version

# 清理后重新编译
make clean
make
```

### 配置文件未生效
```bash
# 确认配置文件格式正确（key=value，无空格）
cat config.txt

# 检查程序输出的配置信息部分
head -20 output.txt
```

### 找不到词典文件
```bash
# 确认 dict 目录存在
ls -l dict/

# 检查配置文件中的路径
grep "Path" config.txt
```

---

**如有问题，请参阅详细文档或提交 Issue。**
