#pragma once
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#define OK 1
#define FAILURE 0

using std::vector;
using std::string;
using std::list;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

typedef int Status;

struct Location
{
    int line;
    int colum;
};
struct Word
{
    Location location;          // 词在源代码中的位置
    int type = 0;               // 类型
    string content;             // 具体内容
    int value = 0;              // 数值记录
};

vector<string> read_file(const string filename);
Status write_file(const string filename, vector<string> lines);


//class Lexical;
//class Syntax;
//class Semantic;
//class Optimize;
//class Analyze;