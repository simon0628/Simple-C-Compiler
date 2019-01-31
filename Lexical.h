#pragma once
#include "common.h"

class Lexical
{
private:
    vector<Word> words;
public:
    vector<Word> analyze(vector<string> lines);
    void prepare(vector<string> lines);
};