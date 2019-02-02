//
// Created by simon on 2019/2/2.
//

#ifndef TEST_GRAMMAR_H
#define TEST_GRAMMAR_H

#endif //TEST_GRAMMAR_H

#include "common.h"

enum table_type{
    shift,
    reduce,
    acc,
    t_goto,
};
struct table_type {

};
class Grammar
{
private:
    int **predict_table;
public:
    void analyze(list<Word> words);
    void save_result();
};
