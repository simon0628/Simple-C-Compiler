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
    epsilon_id = add_symbol(epsilon_str);
    sharp_id = add_symbol(sharp_str);

    init_rules(SYNTAX_RULE_FILE);

    if (str_map_id.find(start_symbol_str) != str_map_id.end())
    {
        start_symbol_id = str_map_id[start_symbol_str];
    }
    else
    {
        cerr << "Undefined start symbol" << endl;
        exit(EXIT_FAILURE);
    }

    init_first();
    init_follow();
    init_DFA();

    for (Symbol symbol:symbols)
    {
        if (symbol.is_terminal)
            cout << symbol.id << " : \'" << symbol.content << "\'" << endl;
        else
            cout << symbol.id << " : " << symbol.content << endl;
    }
//
//    for (Rule rule:rules)
//    {
//        cout << rule.left_id << " -> ";
//        for (int right_symbol:rule.right_ids)
//            cout << right_symbol << ' ';
//        cout << endl;
//    }


//    cout << endl << "First: " << endl;
//
//    for (Symbol symbol:symbols)
//    {
//        cout << symbol.content << ": ";
//        for (int symbol_firsts: symbol.first)
//            cout << symbols[symbol_firsts].content << ' ';
//        cout << endl;
//    }
//
//    cout << endl << "Follow: " << endl;
//
//    for (Symbol symbol:symbols)
//    {
//        cout << symbol.content << ": ";
//        for (int symbol_firsts: symbol.follow)
//            cout << symbols[symbol_firsts].content << ' ';
//        cout << endl;
//    }
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
 * 功能：对所有symbol求first集
 * 算法：
 *（1）若X∈VT,则FIRST(X）={X}；
 *（2）若X∈VN,且有产生式X→a…,a∈VT,则把a加入到FIRST（X）中，若有X→ε，则把ε加入FIRST(X)；
 *（3）若X∈VN,且X→Y… ,Y∈VN, 则把
 *                  FIRST(Y) - {ε}加到FIRST(X)中，
 *            若X→Y1Y2 … Yk,Y1, Y2, … ,Yi-1 ∈ VN, ε∈FIRST (Yj)，则把
 *					   (1<=j<=i-1)
 *                  FIRST(Yi) - {ε}加到FIRST (X)中。
 *         特别地，若ε∈FIRST(Yj) (1<=j<=k)，则 ε∈FIRST(X)
 * (4) 反复使用以上规则, 直至 FIRST(A)不再增大为止。
 */
void Syntax::init_first()
{
    set<int> back_patch;
    for (Symbol &X:symbols)
    {
        //（1）若X∈VT,则FIRST(X）={X}；
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
                        //（3）若X∈VN,且X→Y… ,Y∈VN, 则把
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

                        //（2）若X∈VN,且有产生式X→a…,a∈VT,则把a加入到FIRST（X）中，若有X→ε，则把ε加入FIRST(X)；
                        if (Y_iter == rule.right_ids.end())
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

    //（4）反复使用以上规则, 直至 FIRST(A)不再增大为止。
    bool is_updated = false;
    do
    {
        is_updated = false;
        for (int back_patch_id : back_patch)
        {
            for (int Y:symbols[back_patch_id].first)
            {
                if (!symbols[Y].is_terminal)
                {
                    for (int first_of_symbols : symbols[Y].first)
                    {
                        if (first_of_symbols != epsilon_id)
                        {
                            if (symbols[back_patch_id].first.find(first_of_symbols) ==
                                symbols[back_patch_id].first.end())
                            {
                                is_updated = true;
                                symbols[back_patch_id].first.insert(first_of_symbols);
                            }
                        }
                    }
                }
            }
        }
    } while (is_updated);

    for (int back_patch_id : back_patch)
    {
        vector<int> delete_symbols;
        for (int symbol_of_first : symbols[back_patch_id].first)
        {
            if (!symbols[symbol_of_first].is_terminal)
            {
                delete_symbols.push_back(symbol_of_first);
            }
        }
        for (int symbol_to_delete: delete_symbols)
            symbols[back_patch_id].first.erase(symbol_to_delete);
    }
}

/*
 * 功能：对一串字符alpha求first集
 * 算法：
 *（1） 置 FIRST(α) = FIRST (X1) - {ε};
 *（2） 若对 1<=j<= i -1,  ε∈FIRST (Xj), 则把
 *   	FIRST(Xi) -{ε}加到FIRST(α)中；
 *（3） 若对1<= j <=n, ε∈FIRST (Xj), 则把
 *      ε加到FIRST(α)中。
 */
set<int> Syntax::get_first(vector<int> alpha)
{
    set<int> first;
    if (alpha.size() == 1)
    {
        for (int symbol_of_first : symbols[alpha[0]].first)
            if (symbol_of_first != epsilon_id)
                first.insert(symbol_of_first);
        return first;
    }

    vector<int> back_patch;

    for (int symbol_of_first : symbols[alpha[0]].first)
    {
        if (symbols[symbol_of_first].id != epsilon_id)
            first.insert(symbols[symbol_of_first].id);
    }

    auto Y_iter = alpha.begin();
    while (!symbols[(*Y_iter)].is_terminal && Y_iter != alpha.end())
    {
        set<int> firsts_of_Y = symbols[(*Y_iter)].first;
        if (firsts_of_Y.find(epsilon_id) == firsts_of_Y.end())
            break;
        Y_iter++;
    }

    if (Y_iter == alpha.end())
    {
        first.insert(epsilon_id);
    }
    else if (Y_iter != alpha.begin())
    {
        for (int symbol_of_first : symbols[(*Y_iter)].first)
        {
            if (symbols[symbol_of_first].id != epsilon_id)
                first.insert(symbols[symbol_of_first].id);
        }
    }
    return first;
}

/*
 * 功能：对所有symbol求follow集
 * 算法：
 *（1）若A为文法开始符号，置#于FOLLOW(A）中；
 *（2）若有产生式B→αAβ,
 *	   则将FIRST(β) - {ε}加到FOLLOW (A)中;
 *（3）若有B→αA或B →αAβ, 且β -*-> ε
 *	   则将FOLLOW(B)加到FOLLOW(A)中；
 *（4）反复使用以上规则, 直至 FOLLOW(A)不再增大为止。
*/
void Syntax::init_follow()
{
    set<int> back_patch;

    //（1）若A为文法开始符号，置#于FOLLOW(A）中；
    symbols[start_symbol_id].follow.insert(sharp_id);
    for (Rule rule:rules)
    {
        //（2）若有产生式B→αAβ,
        //	则将FIRST(β) - {ε}加到FOLLOW (A)中;
        auto A_iter = rule.right_ids.begin();
        for (; A_iter != rule.right_ids.end() - 1; A_iter++)
        {
            if (!symbols[*A_iter].is_terminal)
            {
                vector<int> beta;
                for (auto beta_iter = A_iter + 1; beta_iter != rule.right_ids.end(); beta_iter++)
                    beta.push_back(*beta_iter);

                set<int> beta_first = get_first(beta);
                for (int symbol_of_first : beta_first)
                    symbols[*A_iter].follow.insert(symbol_of_first);
            }
        }
        //（3）若有B→αA或B →αAβ, 且β -*-> ε
        //	则将FOLLOW(B)加到FOLLOW(A)中；
        if (!symbols[*A_iter].is_terminal)
        {
            back_patch.insert(*A_iter);
            symbols[*A_iter].follow.insert(rule.left_id);
            if (symbols[*A_iter].first.find(epsilon_id) != symbols[*A_iter].first.end())
            {
                back_patch.insert(*(A_iter - 1));
                symbols[*(A_iter - 1)].follow.insert(rule.left_id);
            }
        }
    }

    //（4）反复使用以上规则, 直至 FOLLOW(A)不再增大为止。
    bool is_updated = false;
    do
    {
        is_updated = false;
        for (int back_patch_id : back_patch)
        {
            for (int Y:symbols[back_patch_id].follow)
            {
                if (!symbols[Y].is_terminal)
                {
                    for (int follow_of_symbols : symbols[Y].follow)
                    {
                        if (follow_of_symbols != epsilon_id)
                        {
                            if (symbols[back_patch_id].follow.find(follow_of_symbols) ==
                                symbols[back_patch_id].follow.end())
                            {
                                is_updated = true;
                                symbols[back_patch_id].follow.insert(follow_of_symbols);
                            }
                        }
                    }
                }
            }
        }
    } while (is_updated);

    for (int back_patch_id : back_patch)
    {
        vector<int> delete_symbols;
        for (int symbol_of_follow : symbols[back_patch_id].follow)
        {
            if (!symbols[symbol_of_follow].is_terminal)
            {
                delete_symbols.push_back(symbol_of_follow);
            }
        }
        for (int symbol_to_delete: delete_symbols)
            symbols[back_patch_id].follow.erase(symbol_to_delete);
    }
}

/*
  1.  I的任何项目都属于CLOSURE(I)；
  2.  若A→aplha·Bbeta属于CLOSURE(I)，那么，对任何关于B的产生式B→beta，项目B→·beta也属于CLOSURE(I)；
  3.  重复执行上述两步骤直至CLOSURE(I) 不再增大为止。
*/
set<Item> Syntax::get_closure(set<Item> I)
{
    set<Item> closure;
    bool is_updated = false;
    do
    {
        is_updated = false;
        for (Item item:I)
        {
            int B = item.rule.right_ids[item.dot];
            if (!symbols[B].is_terminal)
            {
                for (Rule rule:rules)
                {
                    if (rule.left_id == B)
                    {
                        Item temp;
                        temp.rule = rule;
                        temp.dot = 0;
                        closure.insert(temp);

                        is_updated = true;
                    }
                }
            }
        }
    }while(is_updated);

    return closure;
}

void Syntax::init_DFA()
{
    for (Rule rule:rules)
    {
        Item item;
        item.rule = rule;
        for (int dot_loc = 0; dot_loc < rule.right_ids.size() + 1; dot_loc++)
        {
            item.dot = dot_loc;
            items.insert(item);
        }
    }
    for (Item item:items)
    {
        cout << item.rule.left_id << " -> ";
        for (int j = 0; j < item.rule.right_ids.size(); j++)
        {
            if (j == item.dot)
                cout << '.';
            else
                cout << ' ';
            cout << item.rule.right_ids[j];
        }
        cout << endl;
    }


    for (Item item:items)
    {
        set<Item> itemset;
        itemset.insert(item);

        int B = item.rule.right_ids[item.dot];
        if (!symbols[B].is_terminal)
        {
            for (Rule rule:rules)
            {
                if (rule.left_id == B)
                {
                    Item temp;
                    temp.rule = rule;
                    temp.dot = 0;
                    itemset.insert(temp);
                }
            }
        }
    }
}

/*
 * 功能：单条规则构造函数
 * 说明：传入左右规则的string，将其转化为Rule结构体
 */
void Syntax::add_rule(const string &left_symbol, const vector<string> &right_symbols)
{

    Rule new_rule;

    /* ------------------ 规则左边的处理 ------------------ */

    new_rule.left_id = add_symbol(left_symbol);

    /* ------------------ 规则右边的处理 ------------------ */
    for (const string &right_symbol : right_symbols)
    {
        new_rule.right_ids.push_back(add_symbol(right_symbol));
    }
    rules.push_back(new_rule);
}

/*
 * 功能：添加一个新的符号
 * 说明：由于需要建立<符号,int>的索引，需要传入map用于判断此符号是否已经存在
 * 返回：此符号在索引表中的id
 */
symbol_id Syntax::add_symbol(const string &symbol_str)
{
    static int symbol_num = 0; // 符号-int索引，每个不同的符号对应一个新的int值


    bool is_terminal;
    string content;

    // 规定非终结符必须使用单引号注明
    if (symbol_str[0] == '\'')
    {
        int i = 1;
        while (symbol_str[i] != '\'' && i < symbol_str.length())
            i++;
        content = symbol_str.substr(1, i - 1);
        is_terminal = true;
    }
    else
    {
        content = symbol_str;
        is_terminal = false;
    }

    // 若map中不存在此项，则为第一次遇到此符号
    if (str_map_id.find(content) == str_map_id.end())
    {
        Symbol new_symbol;
        str_map_id[content] = symbol_num;

        new_symbol.content = content;
        new_symbol.is_terminal = is_terminal;
        new_symbol.id = symbol_num++;
        symbols.push_back(new_symbol);

        return new_symbol.id;
    }
    else
    {
        return str_map_id[content];
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
                string right_str = (*line_iter).substr(i + 1);
                vector<string> right_symbols = str_split(right_str, " ");

                add_rule(left_symbol, right_symbols);

                line_iter++;
            }
        }
        line_iter++;
    }
}
