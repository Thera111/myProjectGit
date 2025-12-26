#include "cppjieba/Jieba.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map> // 用于哈希表存储词频
#include <queue>         // 用于滑动窗口实现
#include <set>           // 用于存储停用词

using namespace std;
using namespace cppjieba;

// 用于滑动窗口
class wordEntry
{
public:
    string word;
    long long timeStamp;

    wordEntry(const string &word, long long timeStamp)
        : word(word), timeStamp(timeStamp) {}
};

// 在定义 wordEntry 后包含迟到数据处理模块
#include "lateDataHandler.cpp"

// 热词提取类
class wordCount
{
public:
    string word;
    int count;

    wordCount(const string &word, int count)
        : word(word), count(count) {}
};

// 热词比较函数，用于优先队列排序
class cmp
{
public:
    bool operator()(const wordCount &a, const wordCount &b)
    {
        return a.count < b.count;
    }
};

class hotWord
{
private:
    // 分词模块
    cppjieba::Jieba *jieba;

    // 计数器
    unordered_map<string, int> Counter;

    // 滑动窗口
    queue<wordEntry> window;
    long long windowSize = 600; // 时间窗口大小

    // 停用词
    set<string> stopWords;

    // 统计量
    long long totalWords = 0;
    long long totalSentences = 0;

    // 迟到/乱序数据处理器
    LateDataHandler<wordEntry> *lateDataHandler;
    bool enableLateDataHandling; // 是否启用迟到数据处理

public:
    hotWord(const string &dict_path,
            const string &model_path,
            const string &user_dict_path,
            const string &idf_path,
            const string &stop_word_path,
            long long windowSize,
            ostream &out,
            bool enableLateDataHandling = false,
            long long allowedLateness = 30)
        : windowSize(windowSize), enableLateDataHandling(enableLateDataHandling)
    {
        // 分词模块
        jieba = new cppjieba::Jieba(dict_path, model_path, user_dict_path, idf_path, stop_word_path);

        // 加载停用词
        loadStopWords(stop_word_path, out);

        // 初始化迟到数据处理器
        if (enableLateDataHandling)
        {
            lateDataHandler = new LateDataHandler<wordEntry>(allowedLateness, 10000, out);
            out << "迟到/乱序数据处理功能已启用！" << endl;
        }
        else
        {
            lateDataHandler = nullptr;
            out << "迟到/乱序数据处理功能未启用（标准模式）" << endl;
        }
    }

    // 加载停用词函数
    void loadStopWords(const string &stopWordsPath, ostream &out)
    {
        ifstream ifs(stopWordsPath);
        if (!ifs.is_open())
        {
            out << "无法打开停用词文件: " << stopWordsPath << endl;
            return;
        }
        string line;
        while (getline(ifs, line))
        {
            if (!line.empty())
            {
                stopWords.insert(line);
            }
        }
        ifs.close();
        out << "停用词加载完成，总共 " << stopWords.size() << " 个停用词。" << endl;
    }

    // 处理时间戳函数
    long long Timestamp(const string &timeStr)
    {
        // 检查格式
        if (timeStr.length() < 3 || timeStr[0] != '[' || timeStr[timeStr.length() - 1] != ']')
        {
            return -1;
        }
        // 去掉方括号
        string clean = timeStr.substr(1, timeStr.length() - 2);
        // 解析时间
        istringstream iss(clean);
        int hours = 0, minutes = 0, seconds = 0;
        char colon1, colon2;
        if (!(iss >> hours >> colon1 >> minutes >> colon2 >> seconds))
        {
            return -1;
        }
        if (colon1 != ':' || colon2 != ':')
        {
            return -1;
        }
        return hours * 3600 + minutes * 60 + seconds; // 返回总秒数
    }

    // 处理带时间戳的句子函数
    string processSentence(const string &sentence, ostream &out)
    {
        // 获取句子时间戳
        string timestr = sentence.substr(0, sentence.find(']') + 1);
        long long timestamp = Timestamp(timestr);
        if (timestamp == -1)
        {
            out << "时间戳格式错误，跳过该句子处理。" << endl;
            out << timestr << endl;
            return "";
        }
        // 提取句子内容
        string content = sentence.substr(sentence.find(']') + 1);
        // 分词
        vector<string> words;
        jieba->Cut(content, words, true);

        if (enableLateDataHandling)
        {
            // 启用迟到数据处理模式
            processSentenceWithLateHandling(words, timestamp, out);
        }
        else
        {
            // 标准处理模式（原有逻辑）
            processSentenceStandard(words, timestamp, out);
        }

        totalSentences++;
        return timestr;
    }

private:
    // 标准处理模式（原有逻辑）
    void processSentenceStandard(const vector<string> &words, long long timestamp, ostream &out)
    {
        for (const auto &word : words)
        {
            // 跳过停用词
            if (stopWords.find(word) != stopWords.end())
            {
                continue;
            }
            Counter[word]++;
            window.push(wordEntry(word, timestamp));
            totalWords++;
        }
        // 移除过期词（使用当前时间戳）
        while (!window.empty() && (timestamp - window.front().timeStamp) > windowSize)
        {
            string oldWord = window.front().word;
            Counter[oldWord]--;
            if (Counter[oldWord] <= 0)
            {
                Counter.erase(oldWord);
            }
            window.pop();
        }
    }

    // 迟到数据处理模式
    void processSentenceWithLateHandling(const vector<string> &words, long long timestamp, ostream &out)
    {
        // 1. 将所有词条加入迟到数据处理器
        for (const auto &word : words)
        {
            // 跳过停用词
            if (stopWords.find(word) != stopWords.end())
            {
                continue;
            }
            wordEntry entry(word, timestamp);
            lateDataHandler->addData(entry, out);
        }

        // 2. 更新水位线
        lateDataHandler->updateWatermark();

        // 3. 获取所有可处理的数据（按时间戳有序）
        vector<wordEntry> processableData = lateDataHandler->getProcessableData(out);

        // 4. 处理有序数据：更新计数器和窗口
        for (const auto &entry : processableData)
        {
            Counter[entry.word]++;
            window.push(entry);
            totalWords++;
        }

        // 5. 基于水位线移除过期数据
        long long currentWatermark = lateDataHandler->getWatermark();
        while (!window.empty() && (currentWatermark - window.front().timeStamp) > windowSize)
        {
            string oldWord = window.front().word;
            Counter[oldWord]--;
            if (Counter[oldWord] <= 0)
            {
                Counter.erase(oldWord);
            }
            window.pop();
        }
    }

public:

    // 获取topk热词函数
    void getTopK(int k, ostream &out)
    {
        // 使用优先队列获取前 k 个热词
        priority_queue<wordCount, vector<wordCount>, cmp> pq;
        for (const auto &entry : Counter)
        {
            pq.push(wordCount(entry.first, entry.second));
        }

        out << "当前热词前 " << k << " 名：" << endl;
        for (int i = 0; i < k && !pq.empty(); i++)
        {
            wordCount wc = pq.top();
            pq.pop();
            out << i + 1 << ". " << wc.word << " (出现次数: " << wc.count << ")" << endl;
        }
    }

    // 统计信息函数
    void printStats(ostream &out)
    {
        out << "总处理句子数: " << totalSentences << endl;
        out << "总处理词数: " << totalWords << endl;
        out << "当前不同词数: " << Counter.size() << endl;
        
        // 如果启用了迟到数据处理，打印相关统计
        if (enableLateDataHandling && lateDataHandler != nullptr)
        {
            out << endl;
            lateDataHandler->printStatistics(out);
        }
    }

    // 强制清空缓冲区（用于程序结束时）
    void forceFlushBuffer(ostream &out)
    {
        if (enableLateDataHandling && lateDataHandler != nullptr)
        {
            // 强制清空缓冲区并获取所有数据
            vector<wordEntry> remainingData = lateDataHandler->forceFlush(out);
            
            // 处理所有剩余数据
            for (const auto &entry : remainingData)
            {
                Counter[entry.word]++;
                window.push(entry);
                totalWords++;
            }
            
            out << "缓冲区已清空，处理了 " << remainingData.size() << " 条数据。" << endl;
        }
    }

    ~hotWord()
    {
        delete jieba;
        if (lateDataHandler != nullptr)
        {
            delete lateDataHandler;
        }
    }
};