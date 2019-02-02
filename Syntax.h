//
// Created by simon on 2019/2/2.
//

#ifndef TEST_GRAMMAR_H
#define TEST_GRAMMAR_H

#endif //TEST_GRAMMAR_H

#include "common.h"


/* ------------------------ 预测分析表 ------------------------ */

enum table_type{        // 预测分析表项的类型
    shift = 0,
    reduce,
    acc,
    t_goto,
};

struct Table {          // 预测分析表表项
    table_type type;
    int value;
};


/* ------------------------ 文法规则 ------------------------ */
const string START_SYMBOL = "translation_unit";

struct Symbol           // 文法符号
{
    int num;            // 符号对应的序号，在分析过程中只使用序号，不考虑string
    string content;     // 保存string用于输出
    bool is_terminal;   // 是否为终结符
    vector<int> first;
    vector<int> follow;
};

struct Rule              // 单条文法规则
{
    Symbol left;
    vector<Symbol> right;
};


/* ------------------------ Syntax ------------------------ */

class Syntax
{
private:
    vector<Rule> rules;
    vector<Symbol> symbols;
    map<string, int> symbol_map_int;

    int **table;

    void add_rule(const string &left_symbol, const vector<string> &right_symbols);
    void make_rules(vector<string> lines);
    void make_first();


public:
    Syntax();
    ~Syntax();
    void analyze(list<Word> words);
    void save_result();
};
