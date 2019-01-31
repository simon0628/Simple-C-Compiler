#pragma once
#include "common.h"
#include "Lexical.h"

class Analyze
{
private:
    vector<string> source_code;
    vector<Word> code_words;

    Lexical lexical;
//    Grammar grammar;
//    Semantic semantic;
//    Optimize optimize;
public:
    Analyze(char *filename);
    vector<string> read_file(char *filename);
//    void lexical_analyze();
    void write_result();
};