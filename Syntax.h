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

struct Symbol           // 文法符号
{
    int num;            // 符号对应的序号，在分析过程中只使用序号，不考虑string
    string content;     // 保存string用于输出
    bool is_terminal;   // 是否为终结符
};

class Rule              // 单条文法规则
{
private:
    Symbol left;
    vector<Symbol> right;
public:
    Rule(const string &left_symbol, const vector<string> &right_symbols, map<string, int> *symbol_map_int);
};


/* ------------------------ Symtax ------------------------ */

class Syntax
{
private:
    vector<Rule> rules;
    map<string, int> symbol_map_int;
    int **table;

    void make_rules(vector<string> lines);


public:
    Syntax();
    ~Syntax();
    void analyze(list<Word> words);
    void save_result();
};
