# Boost-Search-Engine
基于正倒排索引的Boost搜索引擎


# 🧭 Boost 搜索引擎项目

一个基于 **C++** 实现的轻量级站内搜索引擎，
使用 `Boost`、`cppjieba`、`cpp-httplib`、`jsoncpp` 等组件完成文档解析、索引建立与 HTTP 服务，
支持前后端完整交互，提供搜索、摘要提取与结果展示功能。

---

## 🚀 项目简介

本项目模拟真实搜索引擎的核心流程，从本地 HTML 文档中提取内容、建立索引，并通过浏览器提供搜索接口。

主要功能包括：

1. **HTML 解析与清洗**：去除标签、保留 `title / content / url` 核心信息。
2. **正排与倒排索引**：建立 `DocInfo` 与关键字 → 文档映射关系。
3. **中文分词**：使用 `cppjieba` 对文本进行分词与词频统计。
4. **检索与排序**：根据关键词匹配与权重计算返回结果。
5. **HTTP 搜索接口**：`cpp-httplib` 实现简易 Web 服务。
6. **前端展示页面**：基于 HTML + CSS + jQuery 实现搜索界面与结果展示。

---

## 🏗️ 项目结构

```
boost_search/
│
├── data/                     # 数据目录
│   ├── input/                # 原始 HTML 文件
│   ├── raw_html/             # 去标签后的文本文件
│   └── raw.txt               # 合并后的文本数据
│
├── dict/                     # cppjieba 分词词典
│
├── wwwroot/                  # 前端网页根目录
│   └── index.html            # 前端搜索页面
│
├── parser.cpp                # HTML 解析模块（Parser）
├── index.hpp                 # 索引模块（Index）
├── searcher.hpp              # 搜索模块（Searcher）
├── debug.cc                  # 控制台测试程序
├── http_server.cc            # HTTP 服务程序
├── util.hpp                  # 工具类函数
├── log.hpp                   # 日志工具
└── README.md
```

---

## ⚙️ 环境依赖

* **编译器**：GCC / G++  ≥ 7.0
* **C++ 标准**：C++11
* **依赖库**：

  * [Boost](https://www.boost.org/)
  * [cppjieba](https://github.com/yanyiwu/cppjieba)
  * [cpp-httplib](https://github.com/yhirose/cpp-httplib)
  * [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

---

## 🧩 使用步骤

### 1️⃣ 准备数据

下载 Boost 官方文档：

```bash
wget https://boostorg.jfrog.io/artifactory/main/release/1.89.0/source/boost_1_89_0.tar.gz
tar -xzf boost_1_89_0.tar.gz
cp -r boost_1_89_0/doc/html/* data/input/
```

---

### 2️⃣ 生成原始文本（去标签）

```bash
g++ parser.cpp -o parser -lboost_system -lboost_filesystem -std=c++11
./parser
```

生成的结果文件在：

```
data/raw_html/raw.txt
```

---

### 3️⃣ 建立索引与检索测试

```bash
g++ debug.cc -o debug -ljsoncpp -std=c++11
./debug
```

运行后输入关键字，例如：

```
Please Enter Your Search Query# regex
```

即可看到 JSON 格式的搜索结果。

---

### 4️⃣ 启动 HTTP 服务端

```bash
g++ http_server.cc -o http_server -std=c++11 -ljsoncpp
./http_server
```

默认监听：

```
http://localhost:8081
```

---

### 5️⃣ 打开前端页面

在浏览器中访问：

```
http://localhost:8081/index.html
```

即可看到 Boost 风格的搜索界面。

---

## 🧠 搜索原理简介

1. **正排索引 (Forward Index)**

   * 存储文档信息：`doc_id → {title, content, url}`
2. **倒排索引 (Inverted Index)**

   * 存储词到文档的映射关系：`word → [(doc_id, weight)]`
3. **分词与权重计算**

   * 对标题、正文分词并统计词频；
   * 权重计算公式：

     ```
     weight = 10 * title_count + 1 * content_count
     ```
4. **搜索阶段**

   * 对 query 分词；
   * 触发倒排拉链；
   * 合并、去重、排序；
   * 返回 JSON 格式结果。

---

## 💡 前端说明

* **HTML + CSS + jQuery** 实现；
* 搜索输入框调用 Ajax 请求 `/s?word=xxx`；
* 后端返回 JSON；
* 前端解析后动态渲染结果列表。

简洁风格、即时搜索体验。

---

## 🧱 项目扩展方向

1. 整站搜索：支持多网站数据抓取。
2. 在线更新：爬虫 + 信号机制实现索引热加载。
3. 自研模块：手写 HTTP、JSON、分词等核心组件。
4. 竞价排名：增加广告排序逻辑。
5. 热词统计：字典树 + 优先队列实现智能联想。
6. 用户系统：接入 MySQL，实现登录注册与个性化搜索。

---

## 🏁 作者与许可

* **Author**：Edison
* **Language**：C++
* **License**：MIT
* **Repository**：[GitHub Link](https://github.com/yourname/boost-search-engine)

---

是否希望我帮你再生成一个带 **中英文双语版 README**？（适合开源展示、吸引 star 的那种）

