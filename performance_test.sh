#!/bin/bash
# Performance Test Script for Hot Word Statistics System
# 性能测试脚本

set -e  # Exit on error

echo "============================================"
echo "热词统计系统性能测试"
echo "Performance Test for Hot Word Statistics System"
echo "============================================"
echo ""

# Create performance test directory
PERF_DIR="performance_tests"
mkdir -p "$PERF_DIR"

# Compile the program
echo "[1/5] 编译程序..."
echo "      Compiling program..."
make clean > /dev/null 2>&1 || true
make

if [ ! -f "./hotword" ]; then
    echo "错误: 编译失败"
    echo "Error: Compilation failed"
    exit 1
fi

echo "      编译成功 ✓"
echo ""

# Test configurations
echo "[2/5] 生成测试数据..."
echo "      Generating test data..."

# Small load test data (100 sentences)
cat > "$PERF_DIR/test_small.txt" << 'EOF'
[00:00:00]人工智能技术正在快速发展，深度学习算法不断进步
[00:00:01]机器学习在各个领域得到广泛应用
[00:00:02]深度学习模型的性能持续提升
[00:00:03]人工智能将改变我们的生活方式
[00:00:04]大数据技术支撑着人工智能的发展
[00:00:05]云计算为机器学习提供强大算力
[00:00:06]神经网络是深度学习的核心技术
[00:00:07]自然语言处理技术日益成熟
[00:00:08]计算机视觉在人工智能中占据重要地位
[00:00:09]强化学习解决复杂决策问题
ACTION K=5
[00:00:10]人工智能助力医疗诊断准确性提升
[00:00:11]机器学习算法优化推荐系统效果
[00:00:12]深度学习在图像识别领域表现出色
[00:00:13]自动驾驶技术依赖人工智能
[00:00:14]语音识别系统基于深度学习实现
[00:00:15]机器翻译质量显著提高
[00:00:16]人工智能芯片专为深度学习设计
[00:00:17]边缘计算与人工智能结合紧密
[00:00:18]联邦学习保护数据隐私
[00:00:19]迁移学习提高模型训练效率
ACTION K=10
EOF

# Generate medium load test data (1000 sentences)
python3 << 'PYTHON_SCRIPT' > "$PERF_DIR/test_medium.txt"
import random

topics = [
    ["人工智能", "深度学习", "机器学习", "神经网络", "算法"],
    ["云计算", "大数据", "物联网", "边缘计算", "5G技术"],
    ["区块链", "智能合约", "去中心化", "数字货币", "分布式"],
    ["量子计算", "量子通信", "量子加密", "量子芯片", "量子算法"],
    ["自动驾驶", "智能汽车", "车联网", "传感器", "激光雷达"]
]

actions = [
    "技术正在快速发展", "在各个领域得到广泛应用", "的性能持续提升",
    "将改变我们的生活方式", "支撑着科技的发展", "为创新提供强大动力",
    "是未来科技的核心", "日益成熟", "占据重要地位", "解决复杂问题"
]

for i in range(1000):
    hour = i // 3600
    minute = (i % 3600) // 60
    second = i % 60
    time_str = f"[{hour:02d}:{minute:02d}:{second:02d}]"
    
    topic_list = random.choice(topics)
    word1 = random.choice(topic_list)
    word2 = random.choice(topic_list)
    action = random.choice(actions)
    
    print(f"{time_str}{word1}{action}，{word2}也在不断进步")
    
    if i > 0 and i % 100 == 0:
        print(f"ACTION K=5")

print("ACTION K=10")
PYTHON_SCRIPT

# Generate large load test data (5000 sentences)
python3 << 'PYTHON_SCRIPT' > "$PERF_DIR/test_large.txt"
import random

topics = [
    ["人工智能", "深度学习", "机器学习", "神经网络", "算法", "模型", "训练"],
    ["云计算", "大数据", "物联网", "边缘计算", "5G技术", "网络", "服务器"],
    ["区块链", "智能合约", "去中心化", "数字货币", "分布式", "加密", "共识"],
    ["量子计算", "量子通信", "量子加密", "量子芯片", "量子算法", "超导", "纠缠"],
    ["自动驾驶", "智能汽车", "车联网", "传感器", "激光雷达", "导航", "控制系统"],
    ["生物科技", "基因编辑", "医疗", "药物", "诊断", "治疗", "健康"],
    ["新能源", "太阳能", "风能", "电池", "储能", "清洁能源", "环保"],
    ["航天", "卫星", "火箭", "太空探索", "载人航天", "深空探测", "空间站"]
]

actions = [
    "技术正在快速发展", "在各个领域得到广泛应用", "的性能持续提升",
    "将改变我们的生活方式", "支撑着科技的发展", "为创新提供强大动力",
    "是未来科技的核心", "日益成熟", "占据重要地位", "解决复杂问题",
    "推动产业升级", "创造巨大价值", "引领技术革新", "实现突破性进展",
    "提高工作效率", "降低运营成本", "改善用户体验", "促进可持续发展"
]

for i in range(5000):
    hour = i // 3600
    minute = (i % 3600) // 60
    second = i % 60
    time_str = f"[{hour:02d}:{minute:02d}:{second:02d}]"
    
    topic_list = random.choice(topics)
    word1 = random.choice(topic_list)
    word2 = random.choice(topic_list)
    word3 = random.choice(topic_list)
    action1 = random.choice(actions)
    action2 = random.choice(actions)
    
    print(f"{time_str}{word1}{action1}，{word2}和{word3}{action2}")
    
    if i > 0 and i % 500 == 0:
        print(f"ACTION K=10")

print("ACTION K=20")
PYTHON_SCRIPT

echo "      测试数据生成完成 ✓"
echo "      - 小规模: $PERF_DIR/test_small.txt (100 sentences)"
echo "      - 中规模: $PERF_DIR/test_medium.txt (1000 sentences)"
echo "      - 大规模: $PERF_DIR/test_large.txt (5000 sentences)"
echo ""

# Run performance tests
echo "[3/5] 执行性能测试..."
echo "      Running performance tests..."
echo ""

# Function to run a single test
run_test() {
    local test_name=$1
    local input_file=$2
    local output_file=$3
    
    echo "   测试: $test_name"
    echo "   Test: $test_name"
    
    # Measure time and memory
    /usr/bin/time -v ./hotword "$input_file" "$output_file" 2> "$PERF_DIR/${test_name}_metrics.txt" > /dev/null
    
    # Extract metrics
    elapsed_time=$(grep "Elapsed (wall clock) time" "$PERF_DIR/${test_name}_metrics.txt" | awk '{print $8}')
    max_memory=$(grep "Maximum resident set size" "$PERF_DIR/${test_name}_metrics.txt" | awk '{print $6}')
    
    # Get sentences count
    sentence_count=$(grep -c "^\[" "$input_file" || echo "0")
    
    echo "      完成时间: $elapsed_time"
    echo "      最大内存: ${max_memory} KB"
    echo "      句子数量: $sentence_count"
    echo "      ✓"
    echo ""
}

# Test 1: Small load
run_test "small" "$PERF_DIR/test_small.txt" "$PERF_DIR/output_small.txt"

# Test 2: Medium load
run_test "medium" "$PERF_DIR/test_medium.txt" "$PERF_DIR/output_medium.txt"

# Test 3: Large load
run_test "large" "$PERF_DIR/test_large.txt" "$PERF_DIR/output_large.txt"

# Test 4: Large load with late data handling
echo "   测试: large_with_late_handling"
echo "   Test: large_with_late_handling"
cp config.txt config.txt.bak
sed -i 's/enableLateDataHandling=false/enableLateDataHandling=true/' config.txt
/usr/bin/time -v ./hotword "$PERF_DIR/test_large.txt" "$PERF_DIR/output_large_late.txt" 2> "$PERF_DIR/large_late_metrics.txt" > /dev/null
elapsed_time=$(grep "Elapsed (wall clock) time" "$PERF_DIR/large_late_metrics.txt" | awk '{print $8}')
max_memory=$(grep "Maximum resident set size" "$PERF_DIR/large_late_metrics.txt" | awk '{print $6}')
echo "      完成时间: $elapsed_time"
echo "      最大内存: ${max_memory} KB"
echo "      ✓"
mv config.txt.bak config.txt
echo ""

# Generate summary report
echo "[4/5] 生成性能测试总结..."
echo "      Generating performance summary..."

cat > "$PERF_DIR/performance_summary.txt" << 'EOF'
========================================
热词统计系统性能测试总结
Performance Test Summary
========================================

测试配置:
- 处理器: EOF
cat "$PERF_DIR/performance_summary.txt"
lscpu | grep "Model name" | awk -F: '{print $2}' | xargs >> "$PERF_DIR/performance_summary.txt"
cat >> "$PERF_DIR/performance_summary.txt" << 'EOF'
- 内存: EOF
free -h | grep "Mem:" | awk '{print $2}' >> "$PERF_DIR/performance_summary.txt"
cat >> "$PERF_DIR/performance_summary.txt" << 'EOF'
- 操作系统: EOF
uname -s >> "$PERF_DIR/performance_summary.txt"

cat >> "$PERF_DIR/performance_summary.txt" << 'EOF'

测试结果:
EOF

# Extract and format results
for test in small medium large large_late; do
    if [ -f "$PERF_DIR/${test}_metrics.txt" ]; then
        echo "" >> "$PERF_DIR/performance_summary.txt"
        echo "[$test]" >> "$PERF_DIR/performance_summary.txt"
        grep "Elapsed (wall clock) time" "$PERF_DIR/${test}_metrics.txt" >> "$PERF_DIR/performance_summary.txt"
        grep "Maximum resident set size" "$PERF_DIR/${test}_metrics.txt" >> "$PERF_DIR/performance_summary.txt"
        grep "Percent of CPU" "$PERF_DIR/${test}_metrics.txt" >> "$PERF_DIR/performance_summary.txt"
    fi
done

echo ""
echo "      性能测试总结已保存至: $PERF_DIR/performance_summary.txt"
echo ""

echo "[5/5] 测试完成！"
echo "      Test completed!"
echo ""
echo "结果文件位置:"
echo "Result files location:"
echo "  - 性能指标: $PERF_DIR/*_metrics.txt"
echo "  - 输出文件: $PERF_DIR/output_*.txt"
echo "  - 总结报告: $PERF_DIR/performance_summary.txt"
echo ""
echo "============================================"
