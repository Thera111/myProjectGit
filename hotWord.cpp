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

public:
    hotWord(const string &dict_path,
            const string &model_path,
            const string &user_dict_path,
            const string &idf_path,
            const string &stop_word_path,
            long long windowSize)
        : windowSize(windowSize)
    {
        // 分词模块
        jieba = new cppjieba::Jieba(dict_path, model_path, user_dict_path, idf_path, stop_word_path);

        // 加载停用词
        loadStopWords(stop_word_path);
    }

    // 加载停用词函数
    void loadStopWords(const string &stopWordsPath)
    {
        ifstream ifs(stopWordsPath);
        if (!ifs.is_open())
        {
            cout << "无法打开停用词文件: " << stopWordsPath << endl;
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
        cout << "停用词加载完成，总共 " << stopWords.size() << " 个停用词。" << endl;
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
    string processSentence(const string &sentence)
    {
        // 获取句子时间戳
        string timestr = sentence.substr(0, sentence.find(']') + 1);
        long long timestamp = Timestamp(timestr);
        if (timestamp == -1)
        {
            cout << "时间戳格式错误，跳过该句子处理。" << endl;
            cout << timestr << endl;
            return "";
        }
        // 提取句子内容
        string content = sentence.substr(sentence.find(']') + 1);
        // 分词
        vector<string> words;
        jieba->Cut(content, words, true);

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
        // 移除过期词
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
        totalSentences++;

        return timestr;
    }

    // 获取topk热词函数
    void getTopK(int k)
    {
        // 使用优先队列获取前 k 个热词
        priority_queue<wordCount, vector<wordCount>, cmp> pq;
        for (const auto &entry : Counter)
        {
            pq.push(wordCount(entry.first, entry.second));
        }

        cout << "当前热词前 " << k << " 名：" << endl;
        for (int i = 0; i < k && !pq.empty(); i++)
        {
            wordCount wc = pq.top();
            pq.pop();
            cout << i + 1 << ". " << wc.word << " (出现次数: " << wc.count << ")" << endl;
        }
    }

    // 统计信息函数
    void printStats()
    {
        cout << "总处理句子数: " << totalSentences << endl;
        cout << "总处理词数: " << totalWords << endl;
        cout << "当前不同词数: " << Counter.size() << endl;
    }

    ~hotWord()
    {
        delete jieba;
    }
};