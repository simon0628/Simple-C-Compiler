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
    init_rules(SYNTAX_RULE_FILE);

//    map<string, int>::iterator iter;
//    iter = str_map_symbol.begin();
//    while (iter != str_map_symbol.end())
//    {
//        cout << iter->first << " : " << iter->second << endl;
//        iter++;
//    }

    for(Symbol symbol:symbols)
    {
        if(symbol.is_terminal)
            cout<<symbol.id<<" : ["<<symbol.content<<"]"<<endl;
        else
            cout<<symbol.id<<" : "<<symbol.content<<endl;
    }

    for (Rule rule:rules)
    {
        cout << rule.left->id << " -> ";
        for (Symbol* symbol:rule.right)
            cout << symbol->id << ' ';
        cout << endl;
    }

    init_first();

    for(Symbol symbol:symbols)
    {
        cout<<symbol.content<<": ";
        for(Symbol *first_symbol: symbol.first)
            cout<<first_symbol->content<<' ';
        cout<<endl;
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
void Syntax::add_rule(const string &left_symbol, const vector<string> &right_symbols)
{
    static int symbol_num = 0; // 符号-int索引，每个不同的符号对应一个新的int值

    Rule new_rule;


    /* ------------------ 对规则左半边的处理 ------------------ */

    // 若map中不存在此项，则为第一次遇到此符号
    if (str_map_symbol.find(left_symbol) == str_map_symbol.end())
    {
        Symbol* new_left = new Symbol;
        new_left->content = left_symbol;
        new_left->is_terminal = false;
        new_left->id = symbol_num;

        new_rule.left = new_left;

        str_map_symbol[left_symbol] = new_left;
        symbols.push_back(*new_left);
        symbol_num++;
    }
    else
    {
        new_rule.left = str_map_symbol[left_symbol];
    }

    /* ------------------ 对规则右半边的处理 ------------------ */
    for (const string &right_symbol : right_symbols)
    {
        // 若map中不存在此项，则为第一次遇到此符号
        if (str_map_symbol.find(right_symbol) == str_map_symbol.end())
        {
            Symbol* new_right = new Symbol;
            new_right->id = symbol_num;
            // 规定非终结符必须使用单引号注明
            if (right_symbol[0] == '\'')
            {
                int j = 1;
                while (right_symbol[j] != '\'' && j < right_symbol.length())
                    j++;
                new_right->content = right_symbol.substr(1, j - 1);
                new_right->is_terminal = true;
            }
            else
            {
                new_right->content = right_symbol;
                new_right->is_terminal = false;
            }

            new_rule.right.push_back(new_right);

            str_map_symbol[right_symbol] = new_right;
            symbols.push_back(*new_right);
            symbol_num++;
        }
        else
        {
            new_rule.right.push_back(str_map_symbol[right_symbol]);
        }
    }
    rules.push_back(new_rule);

}

/*
（1）若X∈VT,则FIRST(X）={X}；
（2）若X∈VN,且有产生式X→a…,a∈VT,则把a加入到FIRST（X）中，若有X→ε，则把ε加入FIRST(X)；
（3）若X∈VN,且X→Y… ,Y∈VN, 则把
                   FIRST(Y) - {ε}加到FIRST(X)中，
             若X→Y1Y2 … Yk,Y1, Y2, … ,Yi-1 ∈ VN, ε∈FIRST (Yj)，则把
					   (1<=j<=i-1)
                   FIRST(Yi) - {ε}加到FIRST (X)中。
          特别地，若ε∈FIRST(Yj) (1<=j<=k)，则 ε∈FIRST(X)
 */
void Syntax::init_first()
{
    for(Symbol X:symbols)
    {
        if(X.is_terminal)
        {
            X.first.insert(&X);
        }
        else
        {
            for(Rule rule:rules)
            {
                if(rule.left->id == X.id)
                {
                    if(rule.right[0]->is_terminal)
                    {
                        X.first.insert(rule.right[0]);
                    }
                    else
                    {
                        for(Symbol* first_symbol : rule.right[0]->first)
                        {
                            if(first_symbol != epsilon)
                                X.first.insert(first_symbol);
                        }

                        auto Y_iter = rule.right.begin();
                        while(!(*Y_iter)->is_terminal && Y_iter!= rule.right.end())
                        {
                            if((*Y_iter)->first.find(epsilon) == (*Y_iter)->first.end())
                                break;
                            Y_iter++;
                        }

                        if(Y_iter != rule.right.begin())
                        {
                            for (Symbol *first_symbol : (*Y_iter)->first)
                            {
                                if (first_symbol != epsilon)
                                    X.first.insert(first_symbol);
                            }
                        }
                    }
                }
            }
        }

    }
}

/*
 * 功能：处理文法文件中所有文法规则
 */
void Syntax::init_rules(const string &filename)
{
    vector<string> lines = read_file(filename);
    auto line_iter = lines.begin();

    while (line_iter != lines.end())
    {
        if (is_letter((*line_iter)[0]))
        {
            string left_symbol = (*line_iter);

            line_iter++;

            int i = 0;
            while ((*line_iter)[i] == ' ' || (*line_iter)[i] == '\t')
                i++;

            while ((*line_iter)[i] != ';') // 规定一组文法规则以分号结尾
            {
                string right_str = (*line_iter).substr(i+1);
                vector<string> right_symbols = str_split(right_str, " ");

                add_rule(left_symbol, right_symbols);

                line_iter++;
            }
        }
        line_iter++;
    }

    epsilon = str_map_symbol[EPSILON];
}
