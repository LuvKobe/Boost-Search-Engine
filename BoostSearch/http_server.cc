#include "searcher.hpp"
#include "httplib.h"
#include "log.hpp"

const string input = "data/raw_html/raw.txt";
const std::string root_path = "./wwwroot";

int main()
{
    // 获取单例, 建立索引
    Searcher search;
    search.InitSearcher(input);

    httplib::Server svr;

    svr.set_base_dir(root_path.c_str()); // 引入wwwroot目录
    svr.Get("/s", [&search](const httplib::Request &req, httplib::Response &rsp){ 
        if (!req.has_param("word")) 
        {
            rsp.set_content("必须要有搜索关键字!", "text/plain; charset=utf-8");
            return;
        }
        std::string word = req.get_param_value("word");
        //std::cout << "用户在搜索: " << word << std::endl;
        logMsg(NORMAL, "用户搜索的: %s", word.c_str());
        std::string json_string;
        search.Search(word, &json_string);
        rsp.set_content(json_string.c_str(), "application/json"); // 给用户返回的结果
        });
    logMsg(NORMAL, "服务器启动成功...");
    svr.listen("0.0.0.0", 8081);
    return 0;
}