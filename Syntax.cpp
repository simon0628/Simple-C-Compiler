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
        cout << rule.left_id << " -> ";
        for (int right_symbol:rule.right_ids)
            cout << right_symbol << ' ';
        cout << endl;
    }

    init_first();

    for(Symbol symbol:symbols)
    {
        cout<<symbol.content<<": ";
        for(int symbol_firsts: symbol.first)
            cout<<symbols[symbol_firsts].content<<' ';
        cout<<endl;
    }

    init_follow();
    cout<<endl;

    for(Symbol symbol:symbols)
    {
        cout<<symbol.content<<": ";
        for(int symbol_firsts: symbol.follow)
            cout<<symbols[symbol_firsts].content<<' ';
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

        new_rule.left_id = symbol_num;

        str_map_symbol[left_symbol] = new_left;
        symbols.push_back(*new_left);

        symbol_num++;
    }
    else
    {
        new_rule.left_id = str_map_symbol[left_symbol]->id;
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

            new_rule.right_ids.push_back(symbol_num);

            str_map_symbol[right_symbol] = new_right;
            symbols.push_back(*new_right);

            symbol_num++;
        }
        else
        {
            new_rule.right_ids.push_back(str_map_symbol[right_symbol]->id);
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
    set<int> back_patch;
    for (Symbol &X:symbols)
    {
        if (X.is_terminal)
        {
            X.first.insert(X.id);
        }
        else
        {
            for (Rule &rule:rules)
            {
                if (rule.left_id == X.id)
                {
                    if (symbols[rule.right_ids[0]].is_terminal)
                    {
                        X.first.insert(rule.right_ids[0]);
                    }
                    else
                    {
                        X.first.insert(rule.right_ids[0]);
                        back_patch.insert(X.id);

                        auto Y_iter = rule.right_ids.begin();
                        while (!symbols[(*Y_iter)].is_terminal && Y_iter != rule.right_ids.end())
                        {
                            set<int> firsts_of_Y = symbols[(*Y_iter)].first;
                            if (firsts_of_Y.find(epsilon_id) == firsts_of_Y.end())
                                break;
                            Y_iter++;
                        }

                        if(Y_iter == rule.right_ids.end())
                        {
                            X.first.insert(epsilon_id);
                        }
                        else if (Y_iter != rule.right_ids.begin())
                        {
                            X.first.insert(*Y_iter);
                            back_patch.insert(X.id);
                        }
                    }
                }
            }
        }
    }

    bool is_updated = false;
    do
    {
        is_updated= false;
        for (auto back_patch_iter = back_patch.begin(); back_patch_iter!= back_patch.end(); back_patch_iter++)
        {
            for(int Y:symbols[*back_patch_iter].first)
            {
                if(!symbols[Y].is_terminal)
                {
                    for(int first_of_symbols : symbols[Y].first)
                    {
                        if(first_of_symbols!=epsilon_id)
                        {
                            if(symbols[*back_patch_iter].first.find(first_of_symbols) == symbols[*back_patch_iter].first.end())
                            {
                                is_updated = true;
                                symbols[*back_patch_iter].first.insert(first_of_symbols);
                            }
                        }
                    }
                }
            }
        }
    }while(is_updated);

    for(int back_patch_id : back_patch)
    {
        vector<int> delete_symbols;
        for(int symbol_of_first : symbols[back_patch_id].first)
        {
            if(!symbols[symbol_of_first].is_terminal)
            {
                delete_symbols.push_back(symbol_of_first);
            }
        }
        for(int symbol_to_delete: delete_symbols)
            symbols[back_patch_id].first.erase(symbol_to_delete);
    }
}

/*
（1） 置 FIRST(α) = FIRST (X1) - {ε};
（2） 若对 1<=j<= i -1,  ε∈FIRST (Xj), 则把
    	FIRST(Xi) -{ε}加到FIRST(α)中；
（3） 若对1<= j <=n, ε∈FIRST (Xj), 则把
       ε加到FIRST(α)中。
 */
set<int> Syntax::get_first(vector<int> alpha)
{
    vector<int> back_patch;
    set<int> first;

    for(int symbol_of_first : symbols[alpha[0]].first)
    {
        if(symbols[symbol_of_first].id != epsilon_id)
            first.insert(symbols[symbol_of_first].id);
    }
    for(int symbol:alpha)
    {
        auto Y_iter = alpha.begin();
        while (!symbols[(*Y_iter)].is_terminal && Y_iter != alpha.end())
        {
            set<int> firsts_of_Y = symbols[(*Y_iter)].first;
            if (firsts_of_Y.find(epsilon_id) == firsts_of_Y.end())
                break;
            Y_iter++;
        }

        if(Y_iter == alpha.end())
        {
            first.insert(epsilon_id);
        }
        else if (Y_iter != alpha.begin())
        {
            for(int symbol_of_first : symbols[(*Y_iter)].first)
            {
                if(symbols[symbol_of_first].id != epsilon_id)
                    first.insert(symbols[symbol_of_first].id);
            }
        }
    }
    return first;
}

/*
（1）若A为文法开始符号，置#于FOLLOW(A）中；
（2）若有产生式B→αAβ,
	则将FIRST(β) - {ε}加到FOLLOW (A)中;
（3）若有B→αA或B →αAβ, 且β -*-> ε
	则将FOLLOW(B)加到FOLLOW(A)中；
（4）反复使用以上规则, 直至 FOLLOW(A)不再增大为止。
*/
void Syntax::init_follow()
{
    set<int> back_patch;

    symbols[start_symbol_id].follow.insert(sharp_id);
    for(Rule rule:rules)
    {
        auto A_iter = rule.right_ids.begin();
        for(; A_iter != rule.right_ids.end()-1; A_iter++)
        {
            if(!symbols[*A_iter].is_terminal)
            {
                vector<int> beta;
                for(auto beta_iter = A_iter; beta_iter != rule.right_ids.end(); beta_iter++)
                    beta.push_back(*beta_iter);
                for(int symbol_of_first : get_first(beta))
                    symbols[*A_iter].follow.insert(symbol_of_first);
            }
        }
        if(!symbols[*A_iter].is_terminal)
        {
            back_patch.insert(*A_iter);
            symbols[*A_iter].follow.insert(rule.left_id);
        }
    }

    bool is_updated = false;
    do
    {
        is_updated= false;
        for (auto back_patch_iter = back_patch.begin(); back_patch_iter!= back_patch.end(); back_patch_iter++)
        {
            for(int Y:symbols[*back_patch_iter].follow)
            {
                if(!symbols[Y].is_terminal)
                {
                    for(int follow_of_symbols : symbols[Y].follow)
                    {
                        if(follow_of_symbols!=epsilon_id)
                        {
                            if(symbols[*back_patch_iter].follow.find(follow_of_symbols) == symbols[*back_patch_iter].follow.end())
                            {
                                is_updated = true;
                                symbols[*back_patch_iter].follow.insert(follow_of_symbols);
                            }
                        }
                    }
                }
            }
        }
    }while(is_updated);

    for(int back_patch_id : back_patch)
    {
        vector<int> delete_symbols;
        for(int symbol_of_follow : symbols[back_patch_id].follow)
        {
            if(!symbols[symbol_of_follow].is_terminal)
            {
                delete_symbols.push_back(symbol_of_follow);
            }
        }
        for(int symbol_to_delete: delete_symbols)
            symbols[back_patch_id].follow.erase(symbol_to_delete);
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

    if(str_map_symbol.find(EPSILON)!= str_map_symbol.end())
        epsilon_id = str_map_symbol[EPSILON]->id;

    if(str_map_symbol.find(START_SYMBOL)!= str_map_symbol.end())
        start_symbol_id = str_map_symbol[START_SYMBOL]->id;

    if(str_map_symbol.find(SHARP)!= str_map_symbol.end())
        sharp_id = str_map_symbol[SHARP]->id;
}
