//
// Created by simon on 2019/2/2.
//

#ifndef TEST_GRAMMAR_H
#define TEST_GRAMMAR_H

#endif //TEST_GRAMMAR_H

#include "common.h"

enum table_type{        // 预测分析表项的类型
    shift = 0,
    reduce,
    acc,
    t_goto,
};
struct Table {          // 预测分析表表项
    int type;
    int value;
};

struct Rule
{
    int left;
    vector<int> right;
};

class Syntax
{
private:
    vector<Rule> rules;
    int **table;

    vector<Rule> string_to_rules(vector<string> lines);


public:
    Syntax();
    ~Syntax();
    void analyze(list<Word> words);
    void save_result();
};
