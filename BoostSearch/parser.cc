#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "log.hpp"
#include "util.hpp"

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::vector;

// 是一个目录, 该目录下面放的是所有的html网页
const string src_path = "data/input";          // 原始html网页的路径
const string output = "data/raw_html/raw.txt";    // 去标签后存放的文件

//
typedef struct DocInfo
{
    string title;      //文档的标题
    string content;    //文档内容
    string url;        //该文档在官网中的url
}DocInfo_t;

/*
每个方块代表一个结构体对象（DocInfo_t）。
整个 results 是一个 动态数组（std::vector），
可以存放任意数量的文档对象。

results（std::vector<DocInfo_t>）
│
├── [0] DocInfo_t
│     ├── title   : "乔布斯发布了苹果手机"
│     ├── content : "乔布斯在发布会上介绍了第一代 iPhone..."
│     └── url     : "https://apple.com/iphone"
│
└── [1] DocInfo_t
      ├── title   : "苹果手机销量创新高"
      ├── content : "苹果公司宣布 iPhone 销量突破十亿台..."
      └── url     : "https://apple.com/news"
*/

/*
统一一下函数形参的传参规则:
- const &: 输入
- *: 输出
- &: 输入输出
*/

bool EnumFile(const string &src_path, vector<string> *files_list);
bool ParseHtml(const vector<string> &files_list, vector<DocInfo_t> *results);
bool SaveHtml(const vector<DocInfo_t> &results, const string &output);

int main()
{
    vector<string> files_list; // 存储所有的html文件名

    //第一步: 递归式的把每个html文件名(带路径), 保存到file_list数组中, 方便后期进行一个一个的文件进行读取
    if (!EnumFile(src_path, &files_list))
    {
        cerr << "enum file name error!" << endl;
        return 1;
    }

    //第二步: 按照files_list读取每个文件的内容，并进行解析
    vector<DocInfo_t> results;
    if (!ParseHtml(files_list, &results))
    {
        cerr << "parse html error!" << endl;
        return 2;
    }

    //第三步: 把解析完毕的各个文件内容, 写入到output中, 并按照\3作为每个文档的分割符
    if (!SaveHtml(results, output))
    {
        cerr << "sava html error" << endl;
        return 3;
    }

    return 0;
}
  
// 使用boost库中filesystem模块的功能, 来对文件进行操作

// 使用boost库中的函数来枚举文件名为【.html】的文件, 并保存到【files_list】数组中
bool EnumFile(const string &src_path, vector<string> *files_list) 
{
    namespace fs = boost::filesystem;
    fs::path root_path(src_path);

    // 1.判断路径是否存在, 不存在, 就没有必要再往后走了
    if (!fs::exists(root_path))
    {
        cerr << src_path << " not exists!" << endl;
        return false;
    }

    // 2. 文件存在, 对文件进行递归遍历
    fs::recursive_directory_iterator end; //定义一个空的迭代器，用来进行判断递归结束
    for (fs::recursive_directory_iterator iter(root_path); iter != end; iter++)
    {
        // 判断文件是否是普通文件，html都是普通文件, 诸如: .jpg / .png 等等这种就不行
        if (!fs::is_regular_file(*iter)) 
        {
            continue; // 如果不是普通文件, 那么就跳过, 继续遍历
        }

        // 如果是普通文件, 那么需要判断该文件后缀是否以【.html】结尾的
        if (iter->path().extension() != ".html") 
        {
            continue;
        }

        // 走到这里, 当前的路径一定是一个合法的, 以【.html】结束的普通网页文件
        //logMsg(DEBUG, "%s", iter->path().string().c_str()); // 测试
        files_list->push_back(iter->path().string()); //将所有带路径的html保存在files_list,方便后续进行文本分析
    }
    return true;
}

// <title>HelloWorld</title>
static bool ParseTitle(const string &file, string *title)
{
    // <title>HelloWorld</title>
    // 此时 begin 指向 <title> 中的 <
    size_t begin = file.find("<title>");
    if (begin == string::npos)
    {
        return false;
    }

    // <title>HelloWorld</title>
    // 此时 end 指向 </title> 中的 <
    size_t end = file.find("</title>");
    if (end == string::npos)
    {
        return false;
    }

    // <title>HelloWorld</title>
    // 此时 begin 指向 H
    begin += string("<title>").size();

    if (begin > end) 
    {
        return false;
    }

    // 此时就提取出了HelloWorld
    *title = file.substr(begin, end - begin);

    return true;
}

//
static bool ParseContent(const string &file, string *content)
{
    // 去标签, 基于一个简易的状态机
    enum status
    {
        LABLE,
        CONTENT
    };

    enum status s = LABLE;
    for (char c : file)
    {
        switch (s)
        {
        case LABLE:
            if (c == '>') // 遇到 '>' 表示标签结束，进入内容区
                s = CONTENT;
            break;
        case CONTENT:
            if (c == '<') // 遇到 '<' 表示又开始一个新标签
                s = LABLE;
            else
            {
                // 我们不想保留原始文件中的\n, 因为我们想用\n作为html解析之后文本的分隔符
                if (c == '\n')
                    c = ' ';
                content->push_back(c);
            }
            break;
        default:
            break;
        }
    }

    return true;
}

// 
static bool ParseUrl(const string &file_path, string *url)
{
    // string url_head = "https://www.boost.org/doc/libs/1_89_0/doc/html";
    string url_head = "https://www.boost.org/doc/libs/latest/doc/html";
    string url_tail = file_path.substr(src_path.size());
    *url = url_head + url_tail;

    return true;
}

// 测试函数1
void ShowDoc(const DocInfo_t &doc)
{
    cout << "title: " << doc.title << endl;
    cout << "content: " << doc.content << endl;
    cout << "url: " << doc.url << endl;

}

// 对【files_list】数组中的每个文件进行解析
bool ParseHtml(const vector<string> &files_list, vector<DocInfo_t> *results)
{
    int cnt = 1; // 测试打印函数, 只打印3次
    for (const string &file : files_list)
    {
        
        // 1.读取文件, Read();
        string result; // 存放读取到的文件
        if (!FileUtil::ReadFile(file, &result))
        {
            continue; // 读取失败, 则继续处理下一个
        }

        // 2.解析指定的文件, 提取title
        DocInfo_t doc;
        if (!ParseTitle(result, &doc.title))
        {
            continue;
        }

        // 3.解析指定的文件, 提取content(就是去标签)
        if (!ParseContent(result, &doc.content))
        {
            continue;
        }

        // 4.解析指定的文件路径, 构建url
        if(!ParseUrl(file, &doc.url))
        {
            continue;
        }

        // 测试
        //logMsg(DEBUG, "title: %s, content: %s, url: %s", doc.title.c_str(), doc.content.c_str(), doc.url.c_str());
        if (cnt--)
        {
            //ShowDoc(doc); // 测试函数1
        }

        // 走到这里, 一定是完成了解析任务, 当前文档的相关结果都保存在了【doc】里面
        // 然后把解析完之后的【doc】放入到数组【results】中
        // results->push_back(doc); // bug: 细节, 本质会发生拷贝, 效率可能会比较低
        results->push_back(std::move(doc)); // 移动语义

    }
    return true;
}

bool SaveHtml(const vector<DocInfo_t> &results, const string &output)
{
#define SEP '\3'
    // 按照二进制方式进行写入
    std::ofstream out(output, std::ios::out | std::ios::binary);
    if (!out.is_open())
    {
        cerr << "open " << output << " failed!" << endl;
        return false;
    }
    
    // 开始进行文件内容的写入了
    // 写入到txt中的第一行为: title\3content\3url
    for (auto &item : results)
    {
        string out_string;
        // title
        out_string = item.title;
        out_string += SEP;

        // content
        out_string += item.content;
        out_string += SEP;

        // url
        out_string += item.url;
        out_string += '\n';

        // 把字符串的内容写入到文件中
        out.write(out_string.c_str(), out_string.size());
    }

    // 关闭
    out.close();
    return true;
}