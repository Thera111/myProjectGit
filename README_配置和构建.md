# 配置文件和自动构建说明
# Configuration File and Build Automation Guide

## 一、配置文件使用说明

### 1.1 配置文件概述

项目提供了 `config.txt` 配置文件，用于集中管理所有可配置参数，无需修改源代码即可调整程序行为。

### 1.2 配置文件格式

配置文件采用简单的 `key=value` 格式：

```
# 这是注释行，以 # 开头
parameterName=value
```

**注意事项**：
- 以 `#` 开头的行为注释
- 等号 `=` 两边不要添加空格
- 空行会被自动忽略

### 1.3 可配置参数

#### 输入输出配置

| 参数 | 说明 | 默认值 | 示例 |
|------|------|--------|------|
| `inputFile` | 输入文件路径 | `input1.txt` | `input2.txt` |
| `outputFile` | 输出文件路径 | `output.txt` | `result.txt` |

#### 迟到/乱序数据处理配置

| 参数 | 说明 | 默认值 | 可选值 |
|------|------|--------|--------|
| `enableLateDataHandling` | 是否启用乱序处理 | `true` | `true`/`false` |
| `allowedLateness` | 允许的最大延迟（秒） | `30` | 整数值 |
| `maxBufferSize` | 缓冲区最大容量（条） | `10000` | 整数值 |

#### 时间窗口配置

| 参数 | 说明 | 默认值 | 示例 |
|------|------|--------|------|
| `windowSize` | 时间窗口大小（秒） | `600` | `300`, `1200` |

#### 词典文件配置

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `dictPath` | jieba 分词主词典路径 | `dict/jieba.dict.utf8` |
| `modelPath` | HMM 模型文件路径 | `dict/hmm_model.utf8` |
| `userDictPath` | 用户自定义词典路径 | `dict/user.dict.utf8` |
| `idfPath` | IDF 词典路径 | `dict/idf.utf8` |
| `stopWordPath` | 停用词文件路径 | `dict/stop_words.utf8` |

### 1.4 使用示例

#### 示例 1：使用默认配置运行

```bash
./main
```

程序会自动读取 `config.txt` 中的配置。

#### 示例 2：命令行参数优先

```bash
./main input2.txt output2.txt
```

命令行参数的优先级高于配置文件。这个命令会：
- 使用 `input2.txt` 作为输入（覆盖配置文件）
- 使用 `output2.txt` 作为输出（覆盖配置文件）
- 其他参数仍从配置文件读取

#### 示例 3：创建不同场景的配置文件

**标准模式配置** (`config_standard.txt`)：
```
enableLateDataHandling=false
inputFile=input1.txt
outputFile=output_standard.txt
```

**乱序处理模式配置** (`config_late.txt`)：
```
enableLateDataHandling=true
allowedLateness=60
inputFile=test_late_data_severe.txt
outputFile=output_late.txt
```

使用不同配置：
```bash
cp config_standard.txt config.txt
./main

cp config_late.txt config.txt
./main
```

## 二、Makefile 自动构建说明

### 2.1 Makefile 概述

项目提供了完整的 Makefile，支持自动编译、运行、测试和清理。

### 2.2 基本命令

#### 编译程序

```bash
make          # 编译程序（默认目标）
make all      # 同上
```

#### 清理文件

```bash
make clean      # 清理编译文件（.o, .exe, main等）
make clean-all  # 清理所有文件（包括测试输出）
```

#### 查看帮助

```bash
make help       # 显示所有可用命令和说明
```

### 2.3 运行程序

#### 使用默认配置运行

```bash
make run
```

等同于：
```bash
./main
```

#### 使用指定参数运行

```bash
make run-with-args INPUT=input2.txt OUTPUT=output2.txt
```

等同于：
```bash
./main input2.txt output2.txt
```

### 2.4 自动化测试

#### 运行单个测试

```bash
make test-normal  # 测试正常有序数据
make test-late    # 测试轻度乱序数据
make test-severe  # 测试严重乱序数据
```

#### 运行所有测试

```bash
make test-all
```

这会依次运行所有测试，并生成以下输出文件：
- `output_normal.txt` - 正常有序数据的处理结果
- `output_late.txt` - 轻度乱序数据的处理结果
- `output_severe.txt` - 严重乱序数据的处理结果

### 2.5 调试和检查

#### 编译调试版本

```bash
make debug
```

编译包含调试信息的版本（-g -DDEBUG）。

#### 语法检查

```bash
make check
```

只检查代码语法，不生成可执行文件。

#### 查看编译器版本

```bash
make version
```

### 2.6 完整工作流示例

#### 示例 1：首次使用

```bash
# 1. 查看帮助
make help

# 2. 编译程序
make

# 3. 运行所有测试
make test-all

# 4. 查看测试结果
cat output_normal.txt
cat output_late.txt
cat output_severe.txt
```

#### 示例 2：修改代码后重新编译

```bash
# 1. 清理旧的编译文件
make clean

# 2. 重新编译
make

# 3. 运行测试验证
make test-normal
```

#### 示例 3：处理自定义数据

```bash
# 1. 编译程序
make

# 2. 修改配置文件
vim config.txt

# 3. 运行程序
make run
```

## 三、高级用法

### 3.1 批量测试不同配置

创建脚本 `test_configs.sh`：

```bash
#!/bin/bash

# 测试不同的延迟容忍度
for lateness in 10 30 60; do
    echo "enableLateDataHandling=true" > config.txt
    echo "allowedLateness=$lateness" >> config.txt
    echo "inputFile=test_late_data_severe.txt" >> config.txt
    echo "outputFile=output_lateness_${lateness}.txt" >> config.txt
    
    make run
done
```

### 3.2 性能测试

```bash
# 编译优化版本
make clean
make

# 测试大文件处理时间
time make run-with-args INPUT=input1.txt OUTPUT=perf_test.txt
```

### 3.3 持续集成

在 CI/CD 环境中使用：

```bash
# 编译
make clean all

# 运行测试
make test-all

# 检查是否生成了预期的输出文件
[ -f output_normal.txt ] && echo "测试通过" || echo "测试失败"
```

## 四、常见问题

### 4.1 配置文件未生效

**问题**：修改了 `config.txt` 但程序行为没有变化

**解决**：
1. 确认配置文件名为 `config.txt`
2. 检查配置文件格式是否正确（`key=value`，无空格）
3. 查看程序输出的"配置信息"部分，确认参数值
4. 注意命令行参数优先级高于配置文件

### 4.2 Make 命令不可用

**问题**：`make: command not found`

**解决**：
- **Linux/Mac**: 安装 make 工具
  ```bash
  # Ubuntu/Debian
  sudo apt-get install build-essential
  
  # macOS
  xcode-select --install
  ```
- **Windows**: 使用 MinGW 或 WSL

### 4.3 编译错误

**问题**：编译时出现错误

**解决**：
1. 确保安装了 g++ 编译器
2. 确保编译器支持 C++11：
   ```bash
   g++ --version  # 检查版本
   ```
3. 检查所有必需文件是否存在：
   - `main.cpp`
   - `hotWord.cpp`
   - `lateDataHandler.cpp`
   - `cppjieba/` 目录
   - `dict/` 目录

### 4.4 找不到词典文件

**问题**：运行时提示无法打开词典文件

**解决**：
1. 确认 `dict/` 目录存在
2. 检查配置文件中的词典路径是否正确
3. 确保词典文件存在且有读取权限

## 五、总结

### 5.1 配置文件的优势

✅ **集中管理**：所有参数在一个文件中，便于维护  
✅ **无需重编译**：修改配置无需重新编译代码  
✅ **多场景切换**：可以创建多个配置文件应对不同场景  
✅ **易于分享**：配置文件可以独立分享和版本控制  

### 5.2 Makefile 的优势

✅ **自动化构建**：一条命令完成编译  
✅ **标准化流程**：统一的构建和测试流程  
✅ **提高效率**：减少重复性手动操作  
✅ **易于集成**：方便集成到 CI/CD 流程中  

### 5.3 推荐工作流

1. **开发阶段**：
   ```bash
   vim main.cpp        # 修改代码
   make clean          # 清理
   make                # 编译
   make test-normal    # 快速测试
   ```

2. **测试阶段**：
   ```bash
   make test-all       # 运行所有测试
   diff output_normal.txt expected_output_normal.txt  # 对比结果
   ```

3. **生产运行**：
   ```bash
   vim config.txt      # 配置参数
   make run            # 运行程序
   ```

---

**更多信息**：
- 功能测试指南：`功能测试指南.md`
- 实现原理：`迟到乱序数据处理实现原理.md`
- 实现总结：`实现总结.md`
