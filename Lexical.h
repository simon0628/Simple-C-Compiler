#pragma once
#include "common.h"

enum Type                     // 所有词法种类，0则表示出错
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
const string type_str[] = {   // 词法种类对应的字符串，输出时用
        "error",
        "keyword",
        "operator",
        "delimiter",
        "bracket",
        "function",
        "variable",
        "const",
        "string",
        "unknown"
};

const string keywords[] = {   // 保留字
        "int",
        "void",
        "if",
        "else",
        "while",
        "return",

        "string",
        "for",
        "include",
        "auto",
        "char",
        "bool",
        "break",
        "continue",
        "const",
        "public",
        "private",
        "vector",
        "list"
};

class Lexical
{
private:
    list<Word> words;
    void prepare(vector<string> lines);
    void split_word(list<Word>::iterator word_iter, int j);

public:
    list<Word> analyze(vector<string> lines);
    void print_words();
};
