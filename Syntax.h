//
// Created by simon on 2019/2/2.
//

#ifndef TEST_GRAMMAR_H
#define TEST_GRAMMAR_H

#endif //TEST_GRAMMAR_H

#include "common.h"
typedef int symbol_id;

/* ------------------------ 文法规则 ------------------------ */

//const string SYNTAX_RULE_FILE = "../data_files/syntax_rules.dat";
//const string start_symbol_str = "translation_unit";

const string SYNTAX_RULE_FILE = "../data_files/easy_test2.dat";
const string start_symbol_str = "S";

const string epsilon_str = "\'$\'";
const string sharp_str = "\'#\'";

struct Symbol           // 文法符号
{
    symbol_id id;       // 符号对应的序号，在分析过程中只使用序号，不考虑string
    string content;     // 保存string用于输出
    bool is_terminal;   // 是否为终结符
    set<symbol_id> first;
    set<symbol_id> follow;
};

struct Rule              // 单条文法规则
{
    symbol_id left_id;
    vector<symbol_id> right_ids;
};

/* ------------------------ 预测分析表 ------------------------ */

struct Item
{
    Rule rule;
    int dot;
};


typedef set<int> ItemSet;
typedef set<int> RuleSet;

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
    map<string, symbol_id> str_map_id;

    vector<Rule> all_rules;
    vector<Symbol> all_symbols;
    vector<Item> all_items;

    vector<ItemSet> itemsets;

    int epsilon_id;
    int start_symbol_id;
    int sharp_id;

    int **table;

    void add_rule(const string &left_symbol, const vector<string> &right_symbols);
    symbol_id add_symbol(const string &symbol_str);
    void init_rules(const string &filename);
    void init_first();
    set<int> get_first(const vector<int> alpha);
    void init_follow();

    int find_item(int dot, Rule rule);
    ItemSet get_closure(ItemSet I);
    ItemSet get_go(ItemSet I, Symbol X);


    void init_itemsets();
    void init_table();


public:
    Syntax();
    ~Syntax();
    void analyze(const list<Word> words);
    void save_result();
};
