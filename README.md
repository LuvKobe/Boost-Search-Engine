# Boost-Search-Engine
基于正倒排索引的Boost搜索引擎


## 📘 Project Overview | 项目简介

**English:**

The **Boost Search Engine** is a lightweight C++ search engine powered by the **Boost** libraries.
It demonstrates the core principles of modern search engines — including **data parsing**, **forward indexing**, **inverted indexing**, and **efficient query retrieval** — through a concise and practical implementation.

The project provides an end-to-end workflow:

1. **Web Data Parsing** — Extracts and cleans HTML files, retaining only titles, contents, and URLs.
2. **Forward Index Construction** — Builds a document-level index mapping document IDs to word lists.
3. **Inverted Index Construction** — Creates a term-to-document mapping for fast keyword lookup.
4. **Query and Ranking** — Implements keyword search and result relevance ranking.
5. **Logging & Utilities** — Custom logging (`log.hpp`) and Boost-based file management tools.

By combining **Boost.System**, **Boost.Filesystem**, and modern **C++11** features,
this project provides a compact yet realistic example of how search engines organize and retrieve information efficiently.



**中文：**

**《基于正倒排索引的 Boost 搜索引擎》** 是一个使用 **C++** 与 **Boost 库** 实现的轻量级搜索引擎项目。
它完整展示了现代搜索引擎的核心工作原理，包括 **网页数据解析**、**正排索引构建**、**倒排索引建立**、以及 **关键词查询与结果排序** 的完整流程。

项目包含以下主要功能模块：

1. **网页解析模块**：对 HTML 文件进行解析与清洗，仅保留标题、正文、URL 等核心信息；
2. **正排索引模块**：为每个文档建立正排索引，用于存储文档与词语的对应关系；
3. **倒排索引模块**：构建从词语到文档的倒排表结构，实现快速关键词检索；
4. **查询与排序模块**：支持多关键字查询与相关性打分排序；
5. **日志与工具模块**：自定义日志系统（`log.hpp`）与基于 Boost 的文件遍历工具。

通过结合 **Boost.System**、**Boost.Filesystem** 与 **C++11 标准库**，
本项目以简洁的方式实现了搜索引擎的核心索引与检索机制，适合作为学习搜索引擎原理的参考示例。



## 💡 Example Usage | 使用示例

```bash
make
./parser ./data
./search "乔布斯 发布 苹果 手机"
```


## 🧱 Tech Stack | 技术栈

| 模块   | 技术                             |
| ---- | ------------------------------ |
| 语言   | C++11                          |
| 核心库  | Boost.System, Boost.Filesystem |
| 索引结构 | 正排索引 & 倒排索引                    |
| 工具模块 | 自定义日志（log.hpp）、文件系统操作、文本清洗     |

---

## 🧩 Summary | 总结

This project provides an educational and practical demonstration of how search engines organize and retrieve information efficiently using indexing techniques.

本项目旨在通过实际实现的方式，展示搜索引擎如何利用索引技术高效地组织与检索信息，是学习信息检索与 C++ 实践的优秀入门项目。
