#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include "log.hpp"
#include "util.hpp"

using namespace std;

struct DocInfo
{
    string title;      //文档的标题
    string content;    //文档内容(去标签之后)
    string url;        //该文档在官网中的url
    uint64_t doc_id;   //文档的ID
};

// 倒排的文件元素
struct InvertedElem
{
    uint64_t doc_id;    // ID
    string word;        // 关键字
    int weight;         // 权重

    InvertedElem()
        : weight(0)
    {}
};

//倒排拉链
typedef vector<InvertedElem> InvertedList;

class Index
{
private:
    // 正排索引的数据结构用数组, 数组的下标天然是文档的ID
    vector<DocInfo> forward_index; // 正排索引

    // 倒排索引一定是一个关键字和一组(个)InvertedElem对应【关键字和倒排拉链的映射关系】
    unordered_map<string, InvertedList> inverted_index;

private:
    Index() {} // 这里一定要有函数体，不能delete
    Index(const Index&) = delete;
    Index& operator=(const Index&) = delete;

    static Index* instance;
    static mutex mtx;

public:
    ~Index() {}

public:
    // 创建单例模式
    static Index *GetInstance()
    {
        if (nullptr == instance)
        {
            mtx.lock(); // 加锁保护
            if (nullptr == instance)
            {
                instance = new Index();
            }
            mtx.unlock(); // 解锁
        }
        return instance;
    }

    // 根据doc_id找到找到文档内容(获得正排)
    DocInfo *GetForwardIndex(uint64_t doc_id)
    {
        if (doc_id >= forward_index.size())
        {
            cerr << "doc_id out range error!" <<endl;
            return nullptr;
        }
        return &forward_index[doc_id];
    }

    // 根据关键字string, 获得倒排拉链
    InvertedList *GetInvertedList(const string &word)
    {
        auto iter = inverted_index.find(word);
        if (iter == inverted_index.end())
        {
            cerr << word << " have no InvertedList!" << endl;
            return nullptr;
        }
        return &(iter->second);
    }

    // 根据去标签，格式化之后的文档，构建正排和倒排索引
    // data/raw_html/raw.txt
    bool BuildIndex(const string &input) //parse处理完毕的数据交给我
    {
        ifstream in(input, std::ios::in | std::ios::binary);
        if (!in.is_open())
        {
            cerr << "sorry, " << input << " open error!" << endl;
            return false;
        }

        // 按照一行来读取
        // 从in当中按行来读取, 然后写入file中
        string line;
        int count = 0; // 用于测试
        while (getline(in, line))
        {
            // 建立正排索引
            DocInfo *doc = BuildForwardIndex(line);
            if (nullptr == doc)
            {
                std::cerr << "build " << line << " error" << std::endl; //for deubg
                continue;
            }

            // 建立倒排索引
            BuildInvertedIndex(*doc);

            // 测试代码
            count++;
            if(0 == count % 50)
            {
                logMsg(NORMAL, "当前已经建立的索引文档: %d", count);
            }
        }
        //
        in.close();
        return true;
    }

private:
    // 一次构建正排索引的过程
    DocInfo *BuildForwardIndex(const string &line)
    {
        // 1. 解析line, 字符串切分
        // 把每一行line --> 3个string: title, content, url
        vector<string> results;
        const string sep = "\3";    // 行内分隔符
        StringUtil::Split(line, &results, sep); // 从line中切分, 把结果放到results中
        if (results.size() != 3)
        {
            return nullptr;
        }

        // 2. 把字符串进行填充到DocIinfo中
        DocInfo doc;
        doc.title = results[0];     // title
        doc.content = results[1];   // content
        doc.url = results[2];       // url
        doc.doc_id = forward_index.size(); // 先进行保存id, 再插入, 对应的id就是当前doc在vector中的下标!

        // 3. 插入到正排索引的vector中
        forward_index.push_back(move(doc)); // doc: html文件内容
        return &forward_index.back(); // back()是vector中的最后一个元素(我们每次都要返回最新的)
    }

    // 一次构建倒排索引的过程
    bool BuildInvertedIndex(const DocInfo &doc)
    {
        // 此时DocInfo中包含: {title, content, url, doc_id}
        // 然后要根据【word】 --> 【倒排拉链】之间建立映射。

        // 词频统计
        struct word_cnt 
        {
            int title_cnt;
            int content_cnt;
            // 初始化
            word_cnt()
                : title_cnt(0), content_cnt(0)
            {}
        };
        unordered_map<string, word_cnt> word_map; //用来暂存词频的映射表

        // 对标题进行分词
        vector<string> title_words;
        JiebaUtil::CutString(doc.title, &title_words);
        // 调试
        // if (doc.doc_id == 1007)
        // {
        //     for (auto &s : title_words)
        //     {
        //         cout << "title: " << s << endl;
        //     }
        // }

        // 对文档标题进行词频统计
        for (string s : title_words)
        {
            boost::to_lower(s);     // 需要统一转化成为小写
            word_map[s].title_cnt++; // 如果存在就获取，如果不存在就新建
        }

        // 对文档内容进行分词
        vector<string> content_words;
        JiebaUtil::CutString(doc.title, &content_words);
        // 调试
        // if (doc.doc_id == 1007)
        // {
        //     for (auto &s : content_words)
        //     {
        //         cout << "content: " << s << endl;
        //     }
        // }

        // 对内容进行词频统计
        for (string s : content_words)
        {
            boost::to_lower(s);   // 需要统一转化成为小写
            word_map[s].content_cnt++;
        }

// 自定义相关性
#define X 10
#define Y 1
        // 把统计好的词频设置进倒排拉链中
        for (auto &word_pair : word_map)
        {
            InvertedElem item;
            item.doc_id = doc.doc_id;
            item.word = word_pair.first;
            item.weight = (X * word_pair.second.title_cnt) + (Y * word_pair.second.content_cnt);
            InvertedList &inverted_list = inverted_index[word_pair.first];
            inverted_list.push_back(move(item));
        }

        return true;
    }
};

Index* Index::instance = nullptr;
mutex Index::mtx;