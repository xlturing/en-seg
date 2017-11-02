英文分词、分句
================================
[![Build Status](https://travis-ci.org/seomoz/simhash-cpp.svg?branch=master)](https://travis-ci.org/seomoz/simhash-cpp)
该语料库利用纯规则的方式，能够对英文进行分词和分局，非常的小巧，仅仅需要依赖boost_regex作为正则包。特别注意的是，这段代码仅仅适合于Linux内核的系统，因为在涉及到utf8编码格式的处理时，用到了locale，别的如mac系统需要对代码做些修改。

![Status: Production](https://img.shields.io/badge/status-production-green.svg?style=flat)
![Team: Big Data](https://img.shields.io/badge/team-big_data-green.svg?style=flat)
![Scope: External](https://img.shields.io/badge/scope-external-green.svg?style=flat)
![Open Source: Yes](https://img.shields.io/badge/open_source-MIT-green.svg?style=flat)
![Critical: Yes](https://img.shields.io/badge/critical-yes-red.svg?style=flat)

Usage
=====
Dependency
-------
需要依赖libboost-regex，安装命令如下：
```
sudo apt-get install libboost-regex-dev
```

Library
-------
提供了两个接口
```
/*分词*/
void tokenize(const std::wstring &text, std::list<std::wstring> &lw) const; 
/*分句*/
void split(session_id ses, const std::list<std::wstring> &lw, bool flush, std::list<std::list<std::wstring>> &ls) const;
```

Example
=======
src/Main.cc给出了使用的方式，可以通过如下命令编译执行
```
cmake .
make
bin/en_seg_test
```

Reference
=======
Freeling：https://github.com/TALP-UPC/FreeLing
