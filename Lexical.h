#pragma once
#include "common.h"


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

        "for",
        "include",
        "char",
        "bool",
        "break",
        "continue",
        "const",

        "string",
        "auto",
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
    void save_result();
    void print_words();
};
