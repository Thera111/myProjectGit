# 性能测试数据和结果

本目录包含热词统计系统的性能测试相关文件。

## 目录结构

```
performance_tests/
├── README.md           # 本文件
├── data/              # 测试数据目录
│   ├── small.txt      # 小规模测试数据（100句）
│   ├── medium.txt     # 中规模测试数据（1000句）
│   └── large.txt      # 大规模测试数据（5000句）
├── results/           # 测试结果目录
│   ├── *_elapsed.txt      # 执行时间
│   ├── *_summary.txt      # 性能汇总
│   ├── *_output.txt       # 程序输出
│   └── performance_summary.md  # 总结报告
└── logs/              # 测试日志目录
    ├── compile.log        # 编译日志
    └── *_time.log         # 详细性能日志
```

## 使用说明

### 运行性能测试

在项目根目录执行：

```bash
bash performance_test.sh
```

该脚本会自动：
1. 编译程序
2. 生成测试数据（如果不存在）
3. 运行所有测试
4. 生成性能报告

### 查看测试结果

测试完成后，查看以下文件：

- **快速查看**: `results/performance_summary.md` - 自动生成的性能总结
- **详细报告**: 项目根目录下的 `性能测试报告.md`
- **原始数据**: `results/` 目录下的各个文本文件

### 手动运行单个测试

```bash
# 编译程序
make clean && make

# 生成测试数据（如需要）
# 可以使用脚本中的函数或自己编写

# 运行测试
/usr/bin/time -v ./hotword performance_tests/data/small.txt \
    performance_tests/results/small_output.txt \
    2> performance_tests/logs/small_time.log

# 查看结果
cat performance_tests/logs/small_time.log
```

## 测试数据说明

### 数据生成方式

测试数据通过 `performance_test.sh` 中的 `generate_test_data()` 函数生成：

- **词汇库**: 37 个中文词汇（人工智能、机器学习等）
- **句子模板**: 8 种句子结构
- **时间间隔**: 每句间隔 1-3 秒（随机）
- **格式**: `[HH:MM:SS]句子内容`

### 数据规模

| 文件 | 句子数 | 大小 | 时间跨度 |
|------|--------|------|---------|
| small.txt | 100 | ~8 KB | ~3分钟 |
| medium.txt | 1,000 | ~80 KB | ~30分钟 |
| large.txt | 5,000 | ~400 KB | ~2.5小时 |

### 数据特征

- 平均句子长度: 约 30 字符
- 词汇多样性: 包含 37 个不同词汇
- 高频词重复率: 约 30-40%
- 时间分布: 均匀分布

## 测试指标

性能测试收集以下指标：

- **执行时间**: 程序运行的总时间
- **吞吐量**: 每秒处理的句子数
- **内存占用**: 程序的峰值内存使用
- **CPU 使用率**: CPU 利用率百分比

## 清理测试文件

如需清理测试数据和结果：

```bash
# 仅清理结果和日志（保留测试数据）
rm -rf performance_tests/results/* performance_tests/logs/*

# 清理所有测试文件（包括测试数据）
rm -rf performance_tests/data/* performance_tests/results/* performance_tests/logs/*
```

## 自定义测试

### 添加新的测试场景

1. 在 `performance_test.sh` 中添加新的测试数据生成逻辑
2. 调用 `run_performance_test` 函数运行测试
3. 使用 `analyze_results` 函数分析结果

示例：

```bash
# 生成超大规模数据
generate_test_data "超大规模" "${DATA_DIR}/xlarge.txt" 10000

# 运行测试
run_performance_test "xlarge" "${DATA_DIR}/xlarge.txt" "${RESULTS_DIR}/xlarge_output.txt"

# 分析结果
analyze_results "xlarge" 10000
```

### 修改测试参数

编辑 `performance_test.sh` 中的以下变量：

- 句子数量: 修改 `generate_test_data` 调用的第三个参数
- 词汇库: 修改 `words` 数组
- 句子模板: 修改 `templates` 数组
- 时间间隔: 修改时间递增逻辑

## 性能基准

基于 AMD EPYC 7763 处理器的测试基准：

| 规模 | 句子数 | 执行时间 | 吞吐量 | 内存占用 |
|------|--------|---------|--------|---------|
| 小 | 100 | ~0.5s | 182句/s | ~150MB |
| 中 | 1,000 | ~0.5s | 1,887句/s | ~150MB |
| 大 | 5,000 | ~0.6s | 8,772句/s | ~151MB |

*注意: 实际性能取决于硬件环境*

## 故障排除

### 测试脚本执行失败

1. 检查是否有执行权限: `chmod +x performance_test.sh`
2. 确保在项目根目录执行
3. 检查依赖工具是否安装: `time`, `bc`

### 内存测量不准确

- 确保使用 `/usr/bin/time -v`（不是 shell 内置的 time）
- 在 macOS 上，time 命令输出格式不同，可能需要调整脚本

### 无法生成图表

- 脚本默认使用文本图表（ASCII）
- 如需生成真实图表，请使用 Python + matplotlib 或其他工具

## 相关文档

- **性能测试指南**: `../性能测试指南.md` - 详细的测试方法说明
- **测试报告模板**: `../性能测试报告模板.md` - 报告撰写模板
- **测试报告示例**: `../性能测试报告.md` - 完整的报告示例
- **主 README**: `../README.md` - 项目整体说明

## 贡献

如果你改进了测试脚本或添加了新的测试场景，欢迎提交贡献！

---

**最后更新**: 2024-01-15
