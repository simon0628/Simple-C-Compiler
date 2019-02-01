#pragma once
#include "common.h"

enum Type
{
    t_keyword = 1,
    t_operator,
    t_delimiter,
    t_bracket,
    t_function,
    t_variable,
    t_const,
    t_unknown
};

const string keywords[6] = {"int", "void", "if", "else", "while", "return"};

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
