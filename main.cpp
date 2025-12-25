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

#include "hotWord.cpp"

using namespace std;

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

// 将 vector<string> 中的元素用指定分隔符拼接成一个字符串。
// 用于展示分词结果，例如 "word1/word2/word3"。
std::string Join(const std::vector<std::string> &items, const std::string &delim)
{
    std::ostringstream oss;
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (i)
            oss << delim; // 添加分隔符
        oss << items[i];
    }
    return oss.str();
}

int main(int argc, char *argv[])
{
    // 默认输入和输出文件路径
    std::string inputFile = "input1.txt";  // 默认输入文件
    std::string outputFile = "output.txt"; // 默认输出文件
    if (argc >= 2)
        inputFile = argv[1]; // 如果提供参数，更新输入文件路径
    if (argc >= 3)
        outputFile = argv[2]; // 如果提供参数，更新输出文件路径

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

    // 初始化hotWord类
    hotWord hw(
        "dict/jieba.dict.utf8",
        "dict/hmm_model.utf8",
        "dict/user.dict.utf8",
        "dict/idf.utf8",
        "dict/stop_words.utf8",
        600, // 时间窗口大小
        outFile
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

    outFile.close();
    return 0;
}