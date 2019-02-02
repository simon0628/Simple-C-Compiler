#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#include <iostream>
#include <fstream>
#define OK 1
#define FAILURE 0

using std::vector;
using std::string;
using std::list;
using std::map;
using std::set;

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

enum lexical_type               // 所有词法种类，0则表示出错
{
    t_error = 0,
    t_keyword,
    t_operator,
    t_delimiter,
    t_bracket,
    t_function,
    t_variable,
    t_const,
    t_string,
    t_unknown
};
struct Word
{
    Location location;          // 词在源代码中的位置
    lexical_type type;          // 类型
    string content;             // 具体内容
    int value = 0;              // 数值记录
};

const int TAB_SPACE = 4;

// output filenames
const string LEXICAL_RESULT_FILE = "lexical_result.dat";

// input filenames
const string SYNTAX_RULE_FILE = "test.dat";


bool is_letter(char ch);
vector<string> read_file(const string &filename);
Status write_file(const string &filename, vector<string> lines);
vector<string> str_split(const string &s, const string &seperator);

//class Lexical;
//class Syntax;
//class Semantic;
//class Optimize;
//class Analyze;