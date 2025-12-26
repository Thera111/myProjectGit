#ifndef LATE_DATA_HANDLER_CPP
#define LATE_DATA_HANDLER_CPP

#include <queue>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// 前向声明 wordEntry（已在 hotWord.cpp 中定义）
class wordEntry;

// 时间戳比较器：用于优先队列的排序
// 小顶堆：时间戳小的元素优先级高（先出队）
class CompareTimestamp
{
public:
    bool operator()(const wordEntry &a, const wordEntry &b) const
    {
        return a.timeStamp > b.timeStamp; // 时间戳小的优先
    }
};

/**
 * 迟到/乱序数据处理器
 * 
 * 功能：
 * 1. 接收乱序到达的数据
 * 2. 使用优先队列按时间戳排序
 * 3. 通过水位线机制确定哪些数据可以安全处理
 * 4. 提供有序的数据流供下游处理
 * 
 * 核心概念：
 * - 水位线（Watermark）：表示"早于此时间的数据都已到达"的时间戳
 * - 允许延迟（Allowed Lateness）：系统能容忍的最大数据延迟
 * - 排序缓冲区（Ordered Buffer）：暂存乱序数据的优先队列
 */
class LateDataHandler
{
private:
    // 排序缓冲区：按时间戳从小到大排序
    priority_queue<wordEntry, vector<wordEntry>, CompareTimestamp> orderedBuffer;

    // 当前水位线：表示已处理到的时间点
    long long watermark;

    // 允许的最大延迟时间（秒）
    long long allowedLateness;

    // 观察到的最大时间戳
    long long maxObservedTimestamp;

    // 缓冲区最大容量（避免内存无限增长）
    size_t maxBufferSize;

    // 统计信息
    long long totalProcessed;    // 已处理的数据总数
    long long totalDropped;      // 丢弃的迟到数据数
    long long totalBuffered;     // 当前缓冲区中的数据数

public:
    /**
     * 构造函数
     * @param allowedLateness 允许的最大延迟时间（秒）
     * @param maxBufferSize 缓冲区最大容量
     * @param out 输出流（用于日志）
     */
    LateDataHandler(long long allowedLateness = 30, 
                    size_t maxBufferSize = 10000,
                    ostream &out = cout)
        : allowedLateness(allowedLateness),
          maxBufferSize(maxBufferSize),
          watermark(0),
          maxObservedTimestamp(0),
          totalProcessed(0),
          totalDropped(0),
          totalBuffered(0)
    {
        out << "=== 迟到/乱序数据处理器初始化 ===" << endl;
        out << "允许最大延迟: " << allowedLateness << " 秒" << endl;
        out << "缓冲区最大容量: " << maxBufferSize << " 条" << endl;
    }

    /**
     * 添加数据到缓冲区
     * @param entry 数据条目（包含词和时间戳）
     * @param out 输出流
     * @return true 如果成功添加，false 如果被丢弃
     */
    bool addData(const wordEntry &entry, ostream &out)
    {
        // 更新最大观察时间戳
        if (entry.timeStamp > maxObservedTimestamp)
        {
            maxObservedTimestamp = entry.timeStamp;
        }

        // 检查数据是否过于迟到（早于当前水位线 - 窗口大小）
        // 这些数据已经无法影响结果，直接丢弃
        if (entry.timeStamp < watermark - allowedLateness)
        {
            totalDropped++;
            out << "[警告] 数据过于迟到，已丢弃。时间戳: " << entry.timeStamp 
                << ", 当前水位线: " << watermark << endl;
            return false;
        }

        // 检查缓冲区是否已满
        if (orderedBuffer.size() >= maxBufferSize)
        {
            out << "[警告] 缓冲区已满，强制推进水位线" << endl;
            // 强制推进水位线，释放一些数据
            forceFlush(out);
        }

        // 添加到缓冲区
        orderedBuffer.push(entry);
        totalBuffered++;
        return true;
    }

    /**
     * 更新水位线
     * 水位线 = 最大观察时间戳 - 允许延迟
     * 表示：所有小于等于水位线的数据都已到达（或不再等待）
     */
    void updateWatermark()
    {
        long long newWatermark = maxObservedTimestamp - allowedLateness;
        if (newWatermark > watermark)
        {
            watermark = newWatermark;
        }
    }

    /**
     * 获取所有可处理的数据（时间戳 <= 水位线）
     * @param out 输出流
     * @return 按时间戳排序的数据向量
     */
    vector<wordEntry> getProcessableData(ostream &out)
    {
        vector<wordEntry> result;

        // 从缓冲区中取出所有时间戳 <= watermark 的数据
        while (!orderedBuffer.empty() && 
               orderedBuffer.top().timeStamp <= watermark)
        {
            result.push_back(orderedBuffer.top());
            orderedBuffer.pop();
            totalProcessed++;
            totalBuffered--;
        }

        if (!result.empty())
        {
            out << "[处理] 从缓冲区取出 " << result.size() 
                << " 条数据进行处理" << endl;
        }

        return result;
    }

    /**
     * 强制清空缓冲区（用于缓冲区满或程序结束时）
     * @param out 输出流
     */
    void forceFlush(ostream &out)
    {
        out << "[强制清空] 清空缓冲区，共 " << orderedBuffer.size() << " 条数据" << endl;
        
        // 推进水位线到最大观察时间戳
        watermark = maxObservedTimestamp;
        
        // 取出所有数据
        vector<wordEntry> all;
        while (!orderedBuffer.empty())
        {
            all.push_back(orderedBuffer.top());
            orderedBuffer.pop();
        }
        
        totalBuffered = 0;
    }

    /**
     * 获取当前水位线
     */
    long long getWatermark() const
    {
        return watermark;
    }

    /**
     * 获取缓冲区当前大小
     */
    size_t getBufferSize() const
    {
        return orderedBuffer.size();
    }

    /**
     * 获取最大观察时间戳
     */
    long long getMaxObservedTimestamp() const
    {
        return maxObservedTimestamp;
    }

    /**
     * 打印统计信息
     */
    void printStatistics(ostream &out) const
    {
        out << "=== 迟到/乱序处理器统计 ===" << endl;
        out << "已处理数据: " << totalProcessed << " 条" << endl;
        out << "丢弃数据: " << totalDropped << " 条" << endl;
        out << "缓冲区剩余: " << totalBuffered << " 条" << endl;
        out << "当前水位线: " << watermark << " 秒" << endl;
        out << "最大观察时间戳: " << maxObservedTimestamp << " 秒" << endl;
        if (totalProcessed > 0)
        {
            double dropRate = (double)totalDropped / (totalProcessed + totalDropped) * 100;
            out << "丢弃率: " << dropRate << "%" << endl;
        }
    }

    /**
     * 检查是否启用（基于配置）
     */
    bool isEnabled() const
    {
        return allowedLateness > 0;
    }
};

#endif // LATE_DATA_HANDLER_CPP
