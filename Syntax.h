//
// Created by simon on 2019/2/2.
//

#ifndef TEST_GRAMMAR_H
#define TEST_GRAMMAR_H

#endif //TEST_GRAMMAR_H

#include "common.h"
typedef int SymbolID;

/* ------------------------ 文法规则 ------------------------ */

const string SYNTAX_RULE_FILE = "test.dat";

//const string start_symbol_str = "translation_unit";
const string start_symbol_str = "E";

const string epsilon_str = "\'$\'";
const string sharp_str = "\'#\'";

struct Symbol           // 文法符号
{
    SymbolID id;             // 符号对应的序号，在分析过程中只使用序号，不考虑string
    string content;     // 保存string用于输出
    bool is_terminal;   // 是否为终结符
    set<SymbolID> first;
    set<SymbolID> follow;
};

struct Rule              // 单条文法规则
{
    SymbolID left_id;
    vector<SymbolID> right_ids;
};

/* ------------------------ 预测分析表 ------------------------ */

struct item
{
    Rule *rule_ptr;
    int dot;
};
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


/* ------------------------ Syntax ------------------------ */

class Syntax
{
private:
    vector<Rule> rules;
    vector<Symbol> symbols;
    map<string, SymbolID> str_map_id;

    int epsilon_id;
    int start_symbol_id;
    int sharp_id;

    int **table;

    void add_rule(const string &left_symbol, const vector<string> &right_symbols);
    SymbolID add_symbol(const string &symbol_str);
    void init_rules(const string &filename);
    void init_first();
    set<int> get_first(const vector<int> alpha);
    void init_follow();


public:
    Syntax();
    ~Syntax();
    void analyze(const list<Word> words);
    void save_result();
};
