#pragma once

#include "index.hpp"
#include "util.hpp"
#include "log.hpp"
#include <algorithm>
#include <jsoncpp/json/json.h>

// 打印倒排拉链的结构体
struct InvertedElemPrint
{
    uint64_t doc_id;
    int weight;
    vector<string> words;

    //
    InvertedElemPrint()
        : doc_id(0), weight(0)
    {}
};

class Searcher
{
private:
    Index *index; // 供系统进行查找的索引

public:
    Searcher() {}
    ~Searcher() {}

public:
    void InitSearcher(const string &input)
    {
        // 1.获取或者创建index对象(根据单例模式去获取)
        index = Index::GetInstance();
        logMsg(NORMAL, "获取index单例成功...");

        // 2.根据index对象建立索引
        index->BuildIndex(input);
        logMsg(NORMAL, "建立正排和倒排索引成功...");
    }

    //query: 搜索关键字
    //json_string: 返回给用户浏览器的搜索结果
    void Search(string &query, string *json_string)
    {
        // 1.[分词]: 对我们的query进行按照searcher的要求进行分词
        vector<string> words;
        JiebaUtil::CutString(query, &words);

        // 2.[触发]: 就是根据分词的各个"词", 进行index查找, 建立index是忽略大小写, 所以搜索, 关键字也需要
        //InvertedList inverted_list_all; // 它的内部是InvertedElem
        vector<InvertedElemPrint> inverted_list_all; // 保存不重复的倒排拉链结点
        // 根据doc_id去重
        unordered_map<uint64_t, InvertedElemPrint> tokens_map;

        for (string word : words)
        {
            boost::to_lower(word); // 先把每个词转为小写
            
            InvertedList *inverted_list = index->GetInvertedList(word); // 获取倒排拉链
            if (nullptr == inverted_list)
            {
                continue;
            }
            //inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end());
            // 遍历所有倒排拉链中的结点
            for (const auto &elem : *inverted_list) // 将所有相同的word所对应的倒排拉链结点 ---合并为---> 一个InvertedElemPrint结点
            {
                auto &item = tokens_map[elem.doc_id]; // []:如果存在直接获取，如果不存在新建
                // item一定是doc_id相同的print节点
                item.doc_id = elem.doc_id;
                item.weight += elem.weight;
                item.words.push_back(elem.word);     
                // 即将一个倒排拉链中的4个结点合并为1个结点, 此时就做到了去重的功能
            }
        }
        // 此时inverted_list_all存放的就是去重之后的结果
        for (const auto &item : tokens_map)
        {
            inverted_list_all.push_back(move(item.second)); // 
        }

        // 3.[合并排序]: 汇总查找结果, 按照相关性(weight)降序排序
        // 第三个参数是: lambda表达式写法(也可以写一个回调函数)
        // sort(inverted_list_all.begin(), inverted_list_all.end(), \
        //     [](const InvertedElem &e1, const InvertedElem &e2){
        //     return e1.weight > e2.weight;
        //     });
        sort(inverted_list_all.begin(), inverted_list_all.end(), \
            [](const InvertedElemPrint &e1, const InvertedElemPrint &e2){
            return e1.weight > e2.weight;
            });

        // 4.[构建]: 根据查找出来的结果, 构建json串 -- jsoncpp -- 通过jsoncpp完成序列化&&反序列化
        Json::Value root; // 进行序列化 ---> 本质就是把K&V转化为JSON字符串
        for (auto &item : inverted_list_all) // 每一个item是InvertedElem
        {
            DocInfo *doc = index->GetForwardIndex(item.doc_id);
            if (nullptr == doc)
            {
                continue;
            }
            Json::Value elem;
            elem["title"] = doc->title;
            //elem["desc"] = doc->content; // content是文档的去标签的结果，但是不是我们想要的，我们要的是一部分
            elem["desc"] = GetDesc(doc->content, item.words[0]); // 提取一小部分内容, 当作摘要
            elem["url"] = doc->url;

            // 可以把id和权值打印出来看看(后续可以删除)
            elem["id"] = (int)item.doc_id;
            elem["weight"] = item.weight; //int->string

            root.append(elem);
        }

        // 构建序列化
        //Json::StyledWriter writer; // 这个是方便调试
        Json::FastWriter writer; // 这个更快
        *json_string = writer.write(root);
    }

    // 获取摘要
    string GetDesc(const string &html_content, const string &word)
    {
        // 找到word在html_content中的首次出现，然后往前找50字节(如果没有，从begin开始)，往后找100字节(如果没有，到end就可以的)
        // 截取出这部分内容

        // 
        const int prev_step = 50;    // 往前走50字节
        const int next_step = 100;   // 往后走100字节

        // 1. 找到首次出现
        // size_t pos = html_content.find(word);
        // if (pos == string::npos) 
        // {
        //     return "None1";
        // }
        auto iter = search(html_content.begin(), html_content.end(), word.begin(), word.end(), [](int x, int y){
                return (tolower(x) == tolower(y));
                });
        if (iter == html_content.end())
        {
            return "None1";
        }
        // 如果找到了, 计算迭代器和html_content开头之间的距离
        int pos = distance(html_content.begin(), iter);

        // 2. 获取start, end, (size_t无符号整数)
        int start = 0;
        int end = html_content.size() - 1;
        
        //  如果之前有50个以上的字符，就更新开始位置
        if (pos > start + prev_step) start = pos - prev_step;
        if (pos < end - next_step) end = pos + next_step;

        // 3. 截取子串, return
        if (start >= end)
            return "None2";
        string desc = html_content.substr(start, end - start);
        desc += "....";
        return desc;
    }
};


