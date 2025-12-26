# How to Verify Expected Output Files

## Overview

This document explains how to verify that the late/out-of-order data processing functionality is working correctly by comparing actual outputs with expected outputs.

## Expected Output Files

We provide two expected output files:

1. **expected_output_normal.txt** - Expected output when late data handling is DISABLED
2. **expected_output_late_data.txt** - Expected output when late data handling is ENABLED

## How to Generate Actual Outputs for Comparison

### Step 1: Compile the Program

```bash
g++ -std=c++11 -O2 -I. -o main main.cpp
```

### Step 2: Test Standard Mode (Disabled Late Data Handling)

Edit `main.cpp`:
```cpp
bool enableLateDataHandling = false;  // DISABLE late data handling
long long allowedLateness = 5;
```

Recompile and run:
```bash
g++ -std=c++11 -O2 -I. -o main main.cpp
./main test_late_data.txt output_actual_standard.txt
```

Compare with expected:
```bash
# The hot word counts should match expected_output_normal.txt
# (Note: exact word counts may vary slightly due to jieba segmentation)
cat output_actual_standard.txt
cat expected_output_normal.txt
```

### Step 3: Test Late Data Handling Mode (Enabled)

Edit `main.cpp`:
```cpp
bool enableLateDataHandling = true;   // ENABLE late data handling
long long allowedLateness = 5;
```

Recompile and run:
```bash
g++ -std=c++11 -O2 -I. -o main main.cpp
./main test_late_data.txt output_actual_late.txt
```

Compare with expected:
```bash
cat output_actual_late.txt
cat expected_output_late_data.txt
```

## What to Look For in the Output

### Standard Mode Output Should Contain:

✅ `停用词加载完成，总共 1279 个停用词。`  
✅ `迟到/乱序数据处理功能未启用（标准模式）`  
✅ Hot word query results at timestamps [0:00:09], [0:00:14], [0:00:17]  
✅ `================ 统计信息 ================`  
✅ Final statistics (sentences, words, unique words)  
❌ NO `[处理] 从缓冲区取出...` messages  
❌ NO `=== 迟到/乱序处理器统计 ===`  

### Late Data Handling Mode Output Should Contain:

✅ `停用词加载完成，总共 1279 个停用词。`  
✅ `=== 迟到/乱序数据处理器初始化 ===`  
✅ `允许最大延迟: 5 秒`  
✅ `迟到/乱序数据处理功能已启用！`  
✅ **`[处理] 从缓冲区取出 N 条数据进行处理`** ← KEY INDICATOR  
✅ Hot word query results at timestamps [0:00:09], [0:00:14], [0:00:17]  
✅ `================ 统计信息 ================`  
✅ **`=== 迟到/乱序处理器统计 ===`** ← KEY INDICATOR  
✅ Statistics showing processed data, dropped data, buffer status  
✅ `================ 程序结束，强制处理缓冲区数据 ================`  
✅ `[强制清空] 清空缓冲区，共 N 条数据`  
✅ Final statistics after buffer flush  
✅ `丢弃率: 0%`  

## Key Differences Between Modes

| Feature | Standard Mode | Late Data Handling Mode |
|---------|--------------|------------------------|
| Initialization message | ❌ No | ✅ Yes (detailed config) |
| Processing logs | ❌ No | ✅ Yes (`[处理] ...`) |
| Statistics section | ⚠️ Basic | ✅ Detailed (watermark, buffer, drop rate) |
| Final buffer flush | ❌ No | ✅ Yes |
| Watermark tracking | ❌ No | ✅ Yes |

## Success Criteria

The functionality is successfully implemented if:

1. ✅ **Compilation succeeds** with no errors
2. ✅ **Standard mode output** matches the structure of `expected_output_normal.txt`
3. ✅ **Late data handling mode output** includes:
   - Initialization section
   - `[处理] 从缓冲区取出...` messages
   - Late data handler statistics section
   - Final buffer flush section
4. ✅ **Hot word counts are accurate** (same for ordered data with or without late handling)
5. ✅ **No data dropped** (丢弃率: 0%) for test_late_data.txt with allowedLateness = 5
6. ✅ **Final word count matches** after buffer flush (60 words for test_late_data.txt)

## Example: Successful Output Verification

When you run with late data handling enabled (`allowedLateness = 5`), you should see output like this:

```
停用词加载完成，总共 1279 个停用词。
=== 迟到/乱序数据处理器初始化 ===
允许最大延迟: 5 秒
缓冲区最大容量: 10000 条
迟到/乱序数据处理功能已启用！

[处理] 从缓冲区取出 9 条数据进行处理  👈 This proves buffer is working
[处理] 从缓冲区取出 5 条数据进行处理

[0:00:09]，请求获取前 5 个热词：
当前热词前 5 名：
1. 公园 (出现次数: 2)
...

=== 迟到/乱序处理器统计 ===  👈 This proves statistics tracking
已处理数据: 44 条
丢弃数据: 0 条
缓冲区剩余: 16 条
当前水位线: 15 秒
最大观察时间戳: 20 秒
丢弃率: 0%

================ 程序结束，强制处理缓冲区数据 ================
[强制清空] 清空缓冲区，共 16 条数据  👈 This proves final flush
缓冲区已清空，处理了 16 条数据。

================ 最终统计信息 ================
总处理句子数: 20
总处理词数: 60  👈 This should match total words processed
当前不同词数: 43

=== 迟到/乱序处理器统计 ===
已处理数据: 60 条
丢弃数据: 0 条
缓冲区剩余: 0 条  👈 Buffer should be empty
当前水位线: 20 秒
最大观察时间戳: 20 秒
丢弃率: 0%  👈 No data should be dropped with allowedLateness = 5
```

## Troubleshooting

### Issue: No `[处理]` messages in output

**Cause**: Late data handling not enabled

**Fix**: Ensure `enableLateDataHandling = true` in `main.cpp`

### Issue: All data in buffer, not processed until end

**Cause**: `allowedLateness` too large

**Fix**: Reduce `allowedLateness` to 5 seconds for short test data

### Issue: High drop rate

**Cause**: `allowedLateness` too small for the data

**Fix**: Increase `allowedLateness` to 30 or more

### Issue: Output structure differs significantly from expected

**Cause**: May need to rebuild or check configuration

**Fix**: 
1. Clean build: `rm main && g++ -std=c++11 -O2 -I. -o main main.cpp`
2. Verify configuration in `main.cpp`
3. Check that test data files are UTF-8 encoded

## Conclusion

If your output matches the patterns described above, congratulations! You have successfully implemented and verified the late/out-of-order data processing functionality. 🎉

The key indicators of success are:
1. Processing logs appear during execution
2. Statistics section shows buffer management
3. Watermark advances correctly
4. Final flush processes remaining data
5. Total processed word count is correct
