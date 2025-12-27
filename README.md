# 热词统计与分析系统 (Hot Word Statistics System)

基于滑动窗口的热词统计与分析系统，支持中文分词、实时词频统计和Top-K查询。

## 项目结构

```
.
├── main.cpp              # 主程序入口
├── hotWord.cpp           # 热词统计核心逻辑
├── lateDataHandler.cpp   # 迟到/乱序数据处理模块
├── config.txt            # 配置文件
├── cppjieba/             # jieba中文分词库
├── dict/                 # 词典文件目录
├── input*.txt            # 输入文件示例
└── Makefile              # 构建文件

```

## 系统要求

- C++ 编译器（支持 C++11 标准）
- Linux/Unix 环境或 Windows + MinGW/Cygwin
- Make 构建工具

## 快速开始

### 1. 编译项目

```bash
make
```

或者编译 debug 版本：

```bash
make debug
```

### 2. 运行程序

使用默认配置（读取 `config.txt`）：

```bash
./hotword
```

指定输入输出文件：

```bash
./hotword input1.txt output.txt
```

使用 make 运行：

```bash
make run
```

使用指定的输入输出文件运行：

```bash
make run-with INPUT=input2.txt OUTPUT=output2.txt
```

## Makefile 使用说明

### 可用命令

| 命令 | 说明 |
|------|------|
| `make` 或 `make all` | 编译程序（默认优化模式） |
| `make debug` | 编译 debug 版本（包含调试符号） |
| `make run` | 编译并运行程序（使用默认配置） |
| `make run-with INPUT=<file> OUTPUT=<file>` | 编译并运行，指定输入输出文件 |
| `make clean` | 清理编译产物（不删除输出文件） |
| `make clean-all` | 清理所有生成文件（包括 output*.txt） |
| `make install` | 安装到系统路径（需要 sudo） |
| `make uninstall` | 从系统路径卸载 |
| `make help` | 显示帮助信息 |

### 编译选项

- **标准模式**：使用 `-O2` 优化，生成优化的可执行文件
- **Debug模式**：使用 `-g` 和 `-DDEBUG` 标志，生成包含调试信息的可执行文件

## 配置文件说明

编辑 `config.txt` 可以配置以下参数：

```ini
# 输入输出文件
inputFile=input1.txt
outputFile=output.txt

# 迟到/乱序数据处理
enableLateDataHandling=false
allowedLateness=30

# 时间窗口大小（秒）
windowSize=600

# 词典文件路径
dictPath=dict/jieba.dict.utf8
modelPath=dict/hmm_model.utf8
userDictPath=dict/user.dict.utf8
idfPath=dict/idf.utf8
stopWordPath=dict/stop_words.utf8
```

## 功能特性

### 核心功能
- ✅ 中文分词（基于 jieba）
- ✅ 基于时间的滑动窗口
- ✅ 实时词频统计
- ✅ Top-K 热词查询
- ✅ 停用词过滤

### 高级功能
- ✅ 迟到/乱序数据处理
- ✅ 可配置的时间窗口
- ✅ 水位线机制
- ✅ 详细的统计信息输出

## 输入文件格式

输入文件应该是 UTF-8 编码的文本文件，每行格式如下：

```
[HH:MM:SS]文本内容
```

例如：
```
[10:00:00]今天天气真好，阳光明媚。
[10:00:05]我喜欢在阳光下散步。
ACTION K=10
```

特殊命令：
- `ACTION K=<数字>` - 请求获取前 K 个热词

## 开发说明

### 代码结构

- `main.cpp` - 主程序，负责配置读取和流程控制
- `hotWord.cpp` - 热词统计类，包含分词、计数和窗口管理
- `lateDataHandler.cpp` - 模板类，处理迟到和乱序数据

### 编译标志

- `-std=c++11` - 使用 C++11 标准
- `-Wall` - 启用所有警告
- `-O2` - 优化级别 2
- `-I.` - 包含当前目录
- `-I./cppjieba` - 包含 cppjieba 头文件目录

## 故障排除

### 编译错误

如果遇到编译错误，请确保：
1. 已安装 g++ 编译器
2. 编译器支持 C++11 标准
3. cppjieba 目录存在且完整

### 运行时错误

如果程序无法运行，请检查：
1. 输入文件是否存在且为 UTF-8 编码
2. 词典文件是否存在（dict 目录）
3. 配置文件格式是否正确

## 许可证

请参考项目许可证文件。

## 贡献

欢迎提交问题和改进建议！
