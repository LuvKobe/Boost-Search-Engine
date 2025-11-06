#include "searcher.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>

const string input = "data/raw_html/raw.txt";

int main()
{
    // 测试
    Searcher *search = new Searcher();
    search->InitSearcher(input);

    string query;
    string json_string;
    char buffer[1024];
    while (true)
    {
        cout << "Please Enter Your Search Query# ";
        fgets(buffer, sizeof(buffer)-1, stdin);
        buffer[strlen(buffer) - 1] = 0; // 把 '\n' 设置为 '0'
        query = buffer;
        search->Search(query, &json_string);
        cout << json_string << endl;
    }


    return 0;
}