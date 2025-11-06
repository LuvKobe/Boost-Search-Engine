#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include "cppjieba/Jieba.hpp"
#include "log.hpp"

// 对文件进行解析的工具类
class FileUtil
{
public:
    static bool ReadFile(const std::string &file_path, std::string *out)
    {
        // 打开文件
        std::ifstream in(file_path, std::ios::in);
        if (!in.is_open())
        {
            std::cerr << "open file " << file_path << " error!" << std::endl;
            return false;
        }

        // 读取文件
        // 如何理解getline读取到文件结束呢？？getline的返回值是一个&, while(bool), 本质是因为重载了强制类型转化
        std::string line;
        while (std::getline(in, line)) 
        {
            *out += line;
        }

        // 关闭文件
        in.close();
        return true;
    }
};

// 正排索引的字符串切分
class StringUtil
{
public:
    // Split方法如果以 "类" 的方式被外部使用, 则需要添加static
    static void Split(const std::string &target, std::vector<std::string> *out, const std::string &sep)
    {
        // 采用boost库函数
        boost::split(*out, target, boost::is_any_of(sep), boost::token_compress_on);
    }
};

// 引入词库路径
const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const HMM_PATH = "./dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
const char* const IDF_PATH = "./dict/idf.utf8";
const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";    // 这里面存放的就是暂停词

// 第一版
class JiebaUtil
{
private:
    static cppjieba::Jieba jieba; // 类内的静态jieba成员

public:
    // 分词
    static void CutString(const std::string &src, std::vector<std::string> *out)
    {
        jieba.CutForSearch(src, *out);
    }
};
cppjieba::Jieba JiebaUtil::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);

// 为什么用第一版, 而不用第二版呢？
// 因为move即在boost库中, 又在暂停词中！

// 第二版(去掉暂停词)
/*
class JiebaUtil
{
private:
    cppjieba::Jieba jieba;
    std::unordered_map<std::string, bool> stop_words;

private:
    // 构造
    JiebaUtil()
        : jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
    {}

    // 删除拷贝构造和赋值重载
    JiebaUtil(const JiebaUtil&) = delete;
    JiebaUtil& operator=(const JiebaUtil&) = delete;

    // 定义静态单例对象(注意: 需要在类外进行初始化)
    static JiebaUtil *instance;
    static std::mutex mtx;
public:
    // 创建单例模式
    static JiebaUtil* get_instance()
    {
        if (nullptr == instance)
        {
            mtx.lock();
            if (nullptr == instance)
            {
                instance = new JiebaUtil();
                instance->InitJiebaUtil();
            }
            mtx.unlock();
        }
        return instance;
    }

    // 初始化
    void InitJiebaUtil()
    {
        std::ifstream in(STOP_WORD_PATH);
        if (!in.is_open())
        {
            logMsg(FATAL, "load stop words file error...");
            return;
        }

        // 把所有的暂停词放入stop_words中
        std::string line;
        while (std::getline(in, line))
        {
            stop_words.insert({line, true});
        }
        in.close();
    }

    // 切词
    void CutStringHelper(const std::string &src, std::vector<std::string> *out)
    {
        // 1. 先对原内容进行切词
        jieba.CutForSearch(src, *out); 

        // 2. 凡是在vector中出现的暂停词都要去掉
        for (auto iter = out->begin(); iter != out->end(); )
        {
            // 从stop_words中查找有没有暂停词it, 
            // 如果没有, 那么it肯定是指向stop_words结尾的
            // 如果有, 那么it此时肯定不指向stop_words结尾的
            auto it = stop_words.find(*iter);
            if (it != stop_words.end()) // 找到了, 是暂停词
            {
                // 说明当前的string 是暂停词，需要去掉
                iter = out->erase(iter);
            }
            else  // 没找到, 是常规词
            {
                iter++;
            }
        }

    }
public:
    // 分词
    static void CutString(const std::string &src, std::vector<std::string> *out)
    {
        JiebaUtil::get_instance()->CutStringHelper(src, out);
    }
};

JiebaUtil *JiebaUtil::instance = nullptr;
std::mutex JiebaUtil::mtx;
*/