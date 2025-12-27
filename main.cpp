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
// 从config文件中读参数
bool ReadConfig(const string &configFile, map<string, string> &config)
{
    ifstream ifs(configFile);
    if(!ifs.is_open())  return false;
    string line;
    while(getline(ifs, line))
    {
        // 去掉 Windows 换行符中的 \r
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if(line.empty() || line[0] == '#') continue; // 跳过空行和注释行

        size_t pos = line.find('=');
        if(pos == string::npos) continue;
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        config[key] = value;
    }
    return true;
}

// 从文件中读取 UTF-8 编码的每一行，并存储到 vector 中。
bool ReadUtf8Lines(const string &filename, vector<string> &lines)
{
    ifstream ifs(filename, ios::binary);
    if (!ifs.is_open())
    {
        return false; // 文件无法打开
    }
    string line;
    while (getline(ifs, line))
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
    //从config文件读取参数
    string configFile = "config.txt";
    map<string, string> config;
    if(ReadConfig(configFile, config)==true)    cout << "[INFO ] 配置文件 '" << configFile << "' 读取成功。" << endl;
    else    cerr << "[WARN ] 配置文件 '" << configFile << "' 读取失败，使用默认参数。" << endl;
    
    // 输入输出文件路径
    string inputFile = config.count("inputFile") ? config["inputFile"] : "input1.txt";
    string outputFile = config.count("outputFile") ? config["outputFile"] : "output.txt";
    // 如果有命令行参数，则覆盖配置文件中的设置
    if (argc >= 2)    inputFile = argv[1];
    if (argc >= 3)    outputFile = argv[2];
        
    // 是否启用迟到数据处理
    bool    enableLateDataHandling = config.count("enableLateDataHandling") ? (config["enableLateDataHandling"] == "true") : true;
    
    // 允许的最大延迟时间（秒）
    long long allowedLateness = config.count("allowedLateness") ? std::stoll(config["allowedLateness"]) : 30;
    
    // 时间窗口大小（秒）
    long long windowSize = config.count("windowSize") ? std::stoll(config["windowSize"]) : 600;
    
    // 词典文件路径
    std::string dictPath = config.count("dictPath") ? config["dictPath"] : "dict/jieba.dict.utf8";
    std::string modelPath = config.count("modelPath") ? config["modelPath"] : "dict/hmm_model.utf8";
    std::string userDictPath = config.count("userDictPath") ? config["userDictPath"] : "dict/user.dict.utf8";
    std::string idfPath = config.count("idfPath") ? config["idfPath"] : "dict/idf.utf8";
    std::string stopWordPath = config.count("stopWordPath") ? config["stopWordPath"] : "dict/stop_words.utf8";
    
    // 打开输出文件
    ofstream ofs(outputFile, ios::binary);
    if (!ofs.is_open())
    {
        cerr << "[ERROR] 无法打开输出文件: " << outputFile << endl;
        return EXIT_FAILURE;
    }
    
    // 从输入文件中读取句子
    vector<string> lines;
    if (!ReadUtf8Lines(inputFile, lines))
    {
        cerr << "[ERROR] 无法打开输入文件: " << inputFile << endl;
        cerr << "[HINT ] 请创建一个 UTF-8 编码的文件，命名为 '" << inputFile << "'，并写入中文句子。" << endl;
        ofs.close();
        return EXIT_FAILURE;
    }
    if (lines.empty())    cerr << "[WARN ] 输入文件为空。" << endl;

    // 初始化hotWord类
    hotWord hw(
        dictPath,
        modelPath,
        userDictPath,
        idfPath,
        stopWordPath,
        windowSize,
        ofs,
        enableLateDataHandling,
        allowedLateness
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
                ofs << currTime << "，请求获取前 " << k << " 个热词：" << endl;
                // 获取并显示 top k 热词
                hw.getTopK(k, ofs);
            }
        }
        else
        {
            currTime = hw.processSentence(line, ofs);
        }
    }


    // 如果启用了迟到数据处理，在程序结束前强制清空缓冲区
    if (enableLateDataHandling)
    {
        ofs << endl << "================ 程序结束，强制处理缓冲区数据 ================" << endl;
        hw.forceFlushBuffer(ofs);
    }
    ofs << endl << "================ 统计信息 ================" << endl;
    hw.printStats(ofs);
    ofs.close();
    return 0;
}