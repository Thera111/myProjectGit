# 性能测试说明 (Performance Testing Guide)

本目录包含热词统计系统的性能测试数据和结果。

## 目录结构

```
performance_tests/
├── test_small.txt       # 小规模测试数据 (100句)
├── test_medium.txt      # 中规模测试数据 (1000句)
├── test_large.txt       # 大规模测试数据 (5000句)
├── output_*.txt         # 测试输出文件 (自动生成，不提交)
├── *_metrics.txt        # 性能指标文件 (自动生成，不提交)
└── performance_summary.txt  # 性能汇总报告 (自动生成，不提交)
```

## 运行性能测试

### 自动运行所有测试

在项目根目录执行：

```bash
bash performance_test.sh
```

该脚本会：
1. 编译程序
2. 生成测试数据（如果不存在）
3. 运行小、中、大规模测试
4. 运行启用迟到数据处理的测试
5. 收集性能指标（时间、内存、CPU使用率）
6. 生成性能汇总报告

### 手动运行单个测试

```bash
# 编译程序
make

# 运行小规模测试
./hotword performance_tests/test_small.txt performance_tests/output_small.txt

# 运行中规模测试
./hotword performance_tests/test_medium.txt performance_tests/output_medium.txt

# 运行大规模测试
./hotword performance_tests/test_large.txt performance_tests/output_large.txt
```

## 测试数据说明

### 小规模测试 (test_small.txt)
- **句子数**: 100句
- **用途**: 基础功能验证
- **特点**: 人工精心设计的科技领域句子
- **查询**: 包含2次Top-K查询 (K=5, K=10)

### 中规模测试 (test_medium.txt)
- **句子数**: 1000句
- **用途**: 常规负载测试
- **特点**: 程序生成，覆盖5个技术领域
- **查询**: 每100句一次Top-K查询 (K=5)

### 大规模测试 (test_large.txt)
- **句子数**: 5000句
- **用途**: 高负载压力测试
- **特点**: 程序生成，覆盖8个技术领域
- **查询**: 每500句一次Top-K查询 (K=10, K=20)

### 数据特征

所有测试数据包含以下技术主题：
- 人工智能与机器学习
- 云计算与大数据
- 区块链技术
- 量子计算
- 自动驾驶
- 生物科技
- 新能源
- 航天技术

## 性能指标说明

### 测试指标

每个测试会收集以下指标：

1. **执行时间** (Elapsed time)
   - 程序从启动到结束的总时间
   - 单位: 秒 (s)

2. **内存占用** (Maximum resident set size)
   - 程序运行期间的最大内存使用量
   - 单位: KB (千字节)

3. **CPU利用率** (Percent of CPU)
   - CPU使用百分比
   - 范围: 0-100%

4. **吞吐量** (Throughput)
   - 每秒处理的句子数
   - 单位: 句/秒

5. **延迟** (Latency)
   - 处理单个句子的平均时间
   - 单位: 毫秒 (ms)

### 查看结果

测试完成后，查看性能汇总：

```bash
cat performance_tests/performance_summary.txt
```

查看详细指标：

```bash
cat performance_tests/small_metrics.txt
cat performance_tests/medium_metrics.txt
cat performance_tests/large_metrics.txt
cat performance_tests/large_late_metrics.txt
```

## 预期性能基准

基于测试环境（AMD EPYC 7763，16GB RAM，Linux），预期性能：

| 测试规模 | 句子数 | 预期时间 | 预期内存 | 预期吞吐量 |
|---------|--------|---------|---------|-----------|
| 小规模 | 100 | <1秒 | ~150MB | 150-200句/秒 |
| 中规模 | 1000 | <1秒 | ~150MB | 1500-2000句/秒 |
| 大规模 | 5000 | <1秒 | ~150MB | 8000-10000句/秒 |

**注意**: 实际性能会根据硬件配置有所差异。

## 故障排除

### 测试失败

如果测试失败，请检查：

1. **编译问题**: 确保程序能够成功编译
   ```bash
   make clean
   make
   ```

2. **词典文件**: 确保 `dict/` 目录存在且包含必要的词典文件

3. **Python环境**: 测试数据生成需要Python 3
   ```bash
   python3 --version
   ```

4. **磁盘空间**: 确保有足够的磁盘空间存储测试结果

### 性能差异

如果性能与预期差异较大：

1. **检查CPU负载**: 确保系统没有运行其他高负载任务
   ```bash
   top
   ```

2. **检查编译优化**: 确保使用 `-O2` 优化编译
   ```bash
   make clean
   make  # 默认使用 -O2
   ```

3. **检查系统资源**: 确保有足够的内存
   ```bash
   free -h
   ```

## 自定义测试

### 创建自定义测试数据

可以创建自己的测试数据文件，格式如下：

```
[HH:MM:SS]文本内容
[HH:MM:SS]文本内容
ACTION K=10
```

要求：
- UTF-8编码
- 每行以时间戳 `[HH:MM:SS]` 开头
- 使用 `ACTION K=数字` 触发Top-K查询

### 运行自定义测试

```bash
./hotword 你的测试文件.txt 输出文件.txt
```

## 相关文档

- 完整性能测试报告: `../性能测试报告.md`
- 项目README: `../README.md`
- 系统设计文档: （待补充）

## 许可证

本测试套件遵循项目主许可证。
