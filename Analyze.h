#pragma once
#include "common.h"
#include "Lexical.h"
#include "Syntax.h"

class Analyze
{
private:
    vector<string> source_code;
    list<Word> code_words;

    Lexical lexical;
    Syntax grammar;
//    Semantic semantic;
//    Optimize optimize;
public:
    Analyze(const string filename);
//    void lexical_analyze();
    void write_result();
};