//
// Created by simon on 2019/2/2.
//

#include "Syntax.h"

/* ------------------------ public ------------------------ */

/*
 * 功能：语法分析构造函数，用于读取文法并构建预测分析表
 */
Syntax::Syntax()
{
    make_rules(read_file(SYNTAX_RULE_FILE));

    map<string, int>::iterator iter;
    iter = symbol_map_int.begin();
    while(iter != symbol_map_int.end())
    {
        cout << iter->first << " : " << iter->second << endl;
        iter++;
    }

}

Syntax::~Syntax()
{}

/*
 * 功能：语法分析主函数，根据已得到的分析表对words进行分析
 */
void Syntax::analyze(list<Word> words)
{

}

void Syntax::save_result()
{

}

/* ------------------------ private ------------------------ */

/*
 * 功能：单条文法规则构造函数
 * 说明：
 *      传入左右规则的string，将其转化为Symbol结构体
 *      由于需要建立 符号-int 的索引，需要传入map用于判断是否已经存在
 */
Rule::Rule(const string &left_symbol, const vector<string> &right_symbols, map<string, int> *symbol_map_int)
{
    static int symbol_num = 0; // 符号-int索引，每个不同的符号对应一个新的int值


    /* ------------------ 对规则左半边的处理 ------------------ */
    left.content = left_symbol;
    left.is_terminal = false;

    // 若map中不存在此项，则为第一次遇到此符号
    if(symbol_map_int->find(left_symbol) == symbol_map_int->end())
    {
        left.num = symbol_num;

        (*symbol_map_int)[left_symbol] = symbol_num;
        symbol_num++;
    }
    else
    {
        left.num = (*symbol_map_int)[left_symbol];
    }

    /* ------------------ 对规则右半边的处理 ------------------ */
    for(const string &right_symbol : right_symbols)
    {

        Symbol new_right;

        // 规定非终结符必须使用单引号注明
        if(right_symbol[0] == '\'')
        {
            int j = 1;
            while(right_symbol[j] != '\'' && j<right_symbol.length())
                j++;
            new_right.content = right_symbol.substr(1,j - 1);
            new_right.is_terminal = true;
        }
        else
        {
            new_right.content = right_symbol;
            new_right.is_terminal = false;
        }

        // 若map中不存在此项，则为第一次遇到此符号
        if(symbol_map_int->find(right_symbol) == symbol_map_int->end())
        {
            new_right.num = symbol_num;

            (*symbol_map_int)[right_symbol] = symbol_num;
            symbol_num++;
        }
        else
        {
            new_right.num = (*symbol_map_int)[right_symbol];
        }
        right.push_back(new_right);
    }
}

/*
 * 功能：处理文法文件中所有文法规则
 */
void Syntax::make_rules(vector<string> lines)
{
    auto line_iter = lines.begin();

    while(line_iter != lines.end())
    {
        if(is_letter((*line_iter)[0]))
        {
            string left_symbol = (*line_iter);

            line_iter++;
            while((*line_iter)[9] != ';') // 规定一组文法规则以分号结尾
            {
                string right_str = (*line_iter).substr(10, string::npos-9);
                vector<string> right_symbols = str_split(right_str, " ");

                Rule new_rule(left_symbol, right_symbols, &symbol_map_int);
                rules.push_back(new_rule);

                line_iter++;
            }
        }
        line_iter++;
    }
}
