// ============================= demo.cpp =====================================
// 目的：展示 cppjieba 的功能，包括分词、词性标注、关键词提取等。
// 示例场景：处理中文文本文件中的句子（或直接在代码中定义句子），
// 并将处理结果输出到文件中。
// 要求：程序适用于 Windows 命令行，避免直接在终端输出中文，
// 仅在文件中保存处理结果（UTF-8 编码）。
// ----------------------------------------------------------------------------
// 功能概述：
// 1. 初始化 cppjieba 分词器（加载 5 个必要的词典文件）。
// 2. 从输入文件中读取句子。
// 3. 执行以下操作：
//    - 分词（支持 HMM 模型和非 HMM 模式）。
//    - 搜索引擎模式分词。
//    - 词性标注。
//    - 关键词提取（基于 TF-IDF）。
// 4. 将处理结果写入输出文件。
// ----------------------------------------------------------------------------
// 使用说明：
// - 编译： g++ -std=c++11 -O2 -I. -I./cppjieba -o demo
// - 运行： ./demo                # 使用默认输入/输出文件
//          ./demo input.txt out.txt  # 指定输入和输出文件
// 默认输入文件： test_sentences.txt
// 默认输出文件： output.txt
// 确保 dict/ 目录下的分词词典文件路径正确。
// ----------------------------------------------------------------------------
// 主要功能函数：
// - Cut           : 分词（可选是否使用 HMM 模型）。
// - CutForSearch  : 搜索引擎模式分词（更细粒度）。
// - Tag           : 词性标注。
// - Keyword (TF-IDF): 关键词提取（基于权重）。
// - InsertUserWord: 动态添加用户自定义词汇。
// ----------------------------------------------------------------------------

#include "cppjieba/Jieba.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "hotWord.cpp"

using namespace std;

// 从配置文件中读取配置项
bool ReadConfig(const std::string &configFile, std::map<std::string, std::string> &config)
{
    std::ifstream ifs(configFile);
    if (!ifs.is_open())
    {
        return false;
    }
    
    std::string line;
    while (std::getline(ifs, line))
    {
        // 去掉 Windows 换行符中的 \r
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        
        // 跳过空行和注释行
        if (line.empty() || line[0] == '#')
        {
            continue;
        }
        
        // 解析 key=value 格式
        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            config[key] = value;
        }
    }
    return true;
}

// 从文件中读取 UTF-8 编码的每一行，并存储到 vector 中。
bool ReadUtf8Lines(const std::string &filename, std::vector<std::string> &lines)
{
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open())
    {
        return false; // 文件无法打开
    }
    std::string line;
    while (std::getline(ifs, line))
    {
        // 去掉 Windows 换行符中的 \r
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (!line.empty())
        {
            lines.push_back(line); // 将非空行存储到 vector 中
        }
    }
    return true;
}


int main(int argc, char *argv[])
{
    // 读取配置文件
    std::string configFile = "config.txt";
    std::map<std::string, std::string> config;
    
    if (!ReadConfig(configFile, config))
    {
        std::cerr << "[WARN ] 无法读取配置文件 '" << configFile << "'，将使用默认配置和命令行参数" << std::endl;
    }
    else
    {
        std::cout << "[INFO ] 成功读取配置文件: " << configFile << std::endl;
    }
    
    // 从配置文件或命令行参数获取输入输出文件路径
    std::string inputFile = config.count("inputFile") ? config["inputFile"] : "input1.txt";
    std::string outputFile = config.count("outputFile") ? config["outputFile"] : "output.txt";
    
    // 命令行参数优先级高于配置文件
    if (argc >= 2)
        inputFile = argv[1];
    if (argc >= 3)
        outputFile = argv[2];

    // 打开输出文件
    std::ofstream outFile(outputFile);
    if (!outFile.is_open())
    {
        std::cerr << "[ERROR] 无法打开输出文件: " << outputFile << std::endl;
        return EXIT_FAILURE;
    }

    // 从输入文件中读取句子
    std::vector<std::string> lines;
    if (!ReadUtf8Lines(inputFile, lines))
    {
        std::cerr << "[ERROR] 无法打开输入文件: " << inputFile << std::endl;
        std::cerr << "[HINT ] 请创建一个 UTF-8 编码的文件，命名为 '" << inputFile << "'，并写入中文句子。" << std::endl;
        outFile.close();
        return EXIT_FAILURE;
    }
    if (lines.empty())
    {
        std::cerr << "[WARN ] 输入文件为空。" << std::endl;
    }

    // ========== 配置参数 ==========
    // 从配置文件读取参数，如果配置文件中没有则使用默认值
    
    // 是否启用迟到/乱序数据处理功能
    bool enableLateDataHandling = true;  // 默认值
    if (config.count("enableLateDataHandling"))
    {
        enableLateDataHandling = (config["enableLateDataHandling"] == "true");
    }
    
    // 允许的最大延迟时间（秒）
    long long allowedLateness = 30;  // 默认值
    if (config.count("allowedLateness"))
    {
        allowedLateness = std::stoll(config["allowedLateness"]);
    }
    
    // 时间窗口大小（秒）
    long long windowSize = 600;  // 默认值
    if (config.count("windowSize"))
    {
        windowSize = std::stoll(config["windowSize"]);
    }
    
    // 词典文件路径
    std::string dictPath = config.count("dictPath") ? config["dictPath"] : "dict/jieba.dict.utf8";
    std::string modelPath = config.count("modelPath") ? config["modelPath"] : "dict/hmm_model.utf8";
    std::string userDictPath = config.count("userDictPath") ? config["userDictPath"] : "dict/user.dict.utf8";
    std::string idfPath = config.count("idfPath") ? config["idfPath"] : "dict/idf.utf8";
    std::string stopWordPath = config.count("stopWordPath") ? config["stopWordPath"] : "dict/stop_words.utf8";
    
    // 输出配置信息
    outFile << "========== 配置信息 ==========" << endl;
    outFile << "配置文件: " << configFile << endl;
    outFile << "输入文件: " << inputFile << endl;
    outFile << "输出文件: " << outputFile << endl;
    outFile << "时间窗口大小: " << windowSize << " 秒" << endl;
    outFile << "启用迟到数据处理: " << (enableLateDataHandling ? "是" : "否") << endl;
    if (enableLateDataHandling)
    {
        outFile << "允许最大延迟: " << allowedLateness << " 秒" << endl;
    }
    outFile << "================================" << endl << endl;
    // ===============================

    // 初始化hotWord类
    hotWord hw(
        dictPath,
        modelPath,
        userDictPath,
        idfPath,
        stopWordPath,
        windowSize,
        outFile,
        enableLateDataHandling,     // 是否启用迟到数据处理
        allowedLateness             // 允许的最大延迟
    );

    // 处理每个句子
    string currTime;
    for (auto &line : lines)
    {
        if (line.find("ACTION") != string::npos)
        {
            int Kpos = line.find("K=");
            if (Kpos != string::npos)
            {
                int k = stoi(line.substr(Kpos + 2));
                outFile << currTime << "，请求获取前 " << k << " 个热词：" << endl;
                // 获取并显示 top k 热词
                hw.getTopK(k, outFile);
            }
        }
        else
        {
            // outFile << "处理句子: " << line << endl;
            currTime = hw.processSentence(line, outFile);
        }
    }

    // 输出统计信息
    outFile << "================ 统计信息 ================" << endl;
    hw.printStats(outFile);

    // 如果启用了迟到数据处理，在程序结束前强制清空缓冲区
    if (enableLateDataHandling)
    {
        outFile << endl << "================ 程序结束，强制处理缓冲区数据 ================" << endl;
        hw.forceFlushBuffer(outFile);
        outFile << endl << "================ 最终统计信息 ================" << endl;
        hw.printStats(outFile);
    }

    outFile.close();
    return 0;
}