//
// Created by simon on 2019/2/2.
//

#include "Syntax.h"
#include <algorithm>

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

    init_itemsets();

//    for (Symbol symbol:all_symbols)
//    {
//        if (symbol.is_terminal)
//            cout << symbol.id << " : \'" << symbol.content << "\'" << endl;
//        else
//            cout << symbol.id << " : " << symbol.content << endl;
//    }

//
//    for (Rule rule:all_rules)
//    {
//        cout << rule.left_id << " -> ";
//        for (int right_symbol:rule.right_ids)
//            cout << right_symbol << ' ';
//        cout << endl;
//    }


//    cout << endl << "First: " << endl;
//
//    for (Symbol symbol:all_symbols)
//    {
//        cout << symbol.content << ": ";
//        for (int symbol_firsts: symbol.first)
//            cout << all_symbols[symbol_firsts].content << ' ';
//        cout << endl;
//    }
//
//    cout << endl << "Follow: " << endl;
//
//    for (Symbol symbol:all_symbols)
//    {
//        cout << symbol.content << ": ";
//        for (int symbol_firsts: symbol.follow)
//            cout << all_symbols[symbol_firsts].content << ' ';
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
    for (Symbol &X:all_symbols)
    {
        //（1）若X∈VT,则FIRST(X）={X}；
        if (X.is_terminal)
        {
            X.first.insert(X.id);
        }
        else
        {
            for (Rule &rule:all_rules)
            {
                if (rule.left_id == X.id)
                {
                    if (all_symbols[rule.right_ids[0]].is_terminal)
                    {
                        X.first.insert(rule.right_ids[0]);
                    }
                    else
                    {
                        //（3）若X∈VN,且X→Y… ,Y∈VN, 则把
                        X.first.insert(rule.right_ids[0]);
                        back_patch.insert(X.id);

                        auto Y_iter = rule.right_ids.begin();
                        while (!all_symbols[(*Y_iter)].is_terminal && Y_iter != rule.right_ids.end())
                        {
                            set<int> firsts_of_Y = all_symbols[(*Y_iter)].first;
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
            for (int Y:all_symbols[back_patch_id].first)
            {
                if (!all_symbols[Y].is_terminal)
                {
                    for (int first_of_symbols : all_symbols[Y].first)
                    {
                        if (first_of_symbols != epsilon_id)
                        {
                            if (all_symbols[back_patch_id].first.find(first_of_symbols) ==
                                all_symbols[back_patch_id].first.end())
                            {
                                is_updated = true;
                                all_symbols[back_patch_id].first.insert(first_of_symbols);
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
        for (int symbol_of_first : all_symbols[back_patch_id].first)
        {
            if (!all_symbols[symbol_of_first].is_terminal)
            {
                delete_symbols.push_back(symbol_of_first);
            }
        }
        for (int symbol_to_delete: delete_symbols)
            all_symbols[back_patch_id].first.erase(symbol_to_delete);
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
        for (int symbol_of_first : all_symbols[alpha[0]].first)
            if (symbol_of_first != epsilon_id)
                first.insert(symbol_of_first);
        return first;
    }

    vector<int> back_patch;

    for (int symbol_of_first : all_symbols[alpha[0]].first)
    {
        if (all_symbols[symbol_of_first].id != epsilon_id)
            first.insert(all_symbols[symbol_of_first].id);
    }

    auto Y_iter = alpha.begin();
    while (!all_symbols[(*Y_iter)].is_terminal && Y_iter != alpha.end())
    {
        set<int> firsts_of_Y = all_symbols[(*Y_iter)].first;
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
        for (int symbol_of_first : all_symbols[(*Y_iter)].first)
        {
            if (all_symbols[symbol_of_first].id != epsilon_id)
                first.insert(all_symbols[symbol_of_first].id);
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
    all_symbols[start_symbol_id].follow.insert(sharp_id);
    for (Rule rule:all_rules)
    {
        //（2）若有产生式B→αAβ,
        //	则将FIRST(β) - {ε}加到FOLLOW (A)中;
        auto A_iter = rule.right_ids.begin();
        for (; A_iter != rule.right_ids.end() - 1; A_iter++)
        {
            if (!all_symbols[*A_iter].is_terminal)
            {
                vector<int> beta;
                for (auto beta_iter = A_iter + 1; beta_iter != rule.right_ids.end(); beta_iter++)
                    beta.push_back(*beta_iter);

                set<int> beta_first = get_first(beta);
                for (int symbol_of_first : beta_first)
                    all_symbols[*A_iter].follow.insert(symbol_of_first);
            }
        }
        //（3）若有B→αA或B →αAβ, 且β -*-> ε
        //	则将FOLLOW(B)加到FOLLOW(A)中；
        if (!all_symbols[*A_iter].is_terminal)
        {
            back_patch.insert(*A_iter);
            all_symbols[*A_iter].follow.insert(rule.left_id);
            if (all_symbols[*A_iter].first.find(epsilon_id) != all_symbols[*A_iter].first.end())
            {
                back_patch.insert(*(A_iter - 1));
                all_symbols[*(A_iter - 1)].follow.insert(rule.left_id);
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
            for (int Y:all_symbols[back_patch_id].follow)
            {
                if (!all_symbols[Y].is_terminal)
                {
                    for (int follow_of_symbols : all_symbols[Y].follow)
                    {
                        if (follow_of_symbols != epsilon_id)
                        {
                            if (all_symbols[back_patch_id].follow.find(follow_of_symbols) ==
                                all_symbols[back_patch_id].follow.end())
                            {
                                is_updated = true;
                                all_symbols[back_patch_id].follow.insert(follow_of_symbols);
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
        for (int symbol_of_follow : all_symbols[back_patch_id].follow)
        {
            if (!all_symbols[symbol_of_follow].is_terminal)
            {
                delete_symbols.push_back(symbol_of_follow);
            }
        }
        for (int symbol_to_delete: delete_symbols)
            all_symbols[back_patch_id].follow.erase(symbol_to_delete);
    }
}

/*
 * 1.  I的任何项目都属于CLOSURE(I)；
 * 2.  若A→aplha·Bbeta属于CLOSURE(I)，那么，对任何关于B的产生式B→beta，项目B→·beta也属于CLOSURE(I)；
 * 3.  重复执行上述两步骤直至CLOSURE(I) 不再增大为止。
 */
ItemSet Syntax::get_closure(ItemSet I)
{
    ItemSet closure = I;

    bool is_updated;
    do
    {
        is_updated = false;
        for (int item_id:I)
        {
            Item item = all_items[item_id];
            if(item.dot < item.rule.right_ids.size())
            {
                int B = item.rule.right_ids[item.dot];
                if (!all_symbols[B].is_terminal)
                {
                    for (Rule rule:all_rules)
                    {
                        if (rule.left_id == B)
                        {
                            int item_loc = find_item(0, rule);
                            if (item_loc != -1)
                            {
                                if (closure.insert(item_loc).second == true)
                                    is_updated = true;
                            }
                        }
                    }
                }
            }
        }
    }while(is_updated);

    return closure;
}

/*
 * GO(I，X)＝CLOSURE(J), 其中
 * J＝{任何形如A→alphaX·beta的项目| A→alpha·Xbeta属于I}
 */
ItemSet Syntax::get_go(ItemSet I, Symbol X)
{
    ItemSet J;
    for(int i : I)
    {
        vector<int> right_ids = all_items[i].rule.right_ids;
        vector<int>::iterator find_iter = std::find(right_ids.begin(), right_ids.end(), X.id);
        if(find_iter != right_ids.end())
        {
            int loc = find_iter-right_ids.begin();
            if(loc == all_items[i].dot && loc < right_ids.size())
            {
                int item_id = find_item(loc + 1, all_items[i].rule);
                if(item_id != -1)
                    J.insert(item_id);
            }
        }
    }

    ItemSet p = get_closure(J);
    return p;
}

int Syntax::find_item(int dot, Rule rule)
{
    bool match = false;
    for(int i = 0; i < all_items.size(); i++)
    {
        Item item = all_items[i];
        if(item.rule.left_id == rule.left_id && item.dot == dot)
        {
            match = true;
            for(int j = 0; j < rule.right_ids.size() && j < item.rule.right_ids.size(); j++)
            {
                if(rule.right_ids[j] != item.rule.right_ids[j])
                {
                    match = false;
                    break;
                }
            }
            if(match)
                return i;
        }
    }
    return -1;
}


/*
 * PROCEDURE  ITEMSETS(G')；
 * BEGIN
 * C:={CLOSURE({S->·S})}；
 *   REPEAT
 *     FOR  C中每个项目集I和G'的每个符号X  DO
 *         IF  GO(I，X)非空且不属于C   THEN
 *         把GO(I，X)放入C族中;
 *   UNTIL C	不再增大
 * END
 */
void Syntax::init_itemsets()
{
    for (Rule rule:all_rules)
    {
        Item item;
        item.rule = rule;
        for (int dot_loc = 0; dot_loc < rule.right_ids.size() + 1; dot_loc++)
        {
            item.dot = dot_loc;
            all_items.push_back(item);
        }
    }

    set<int> start_item;
    start_item.insert(0);

    itemsets.push_back(get_closure(start_item));

    bool is_updated = false;
    do
    {
        is_updated = false;
        for(ItemSet I : itemsets)
        {
            vector<Symbol> itemset_symbols;
            for(int item_id : I)
            {
                Item item = all_items[item_id];
                if(item.dot < item.rule.right_ids.size())
                itemset_symbols.push_back(all_symbols[item.rule.right_ids[item.dot]]);
            }
            for(Symbol X : itemset_symbols)
            {
                ItemSet temp_go = get_go(I, X);
                if(temp_go.size() > 0)
                {
                    vector<ItemSet>::iterator find_iter = std::find(itemsets.begin(), itemsets.end(), temp_go);
                    if(find_iter == itemsets.end())
                    {
                        itemsets.push_back(temp_go);
                        is_updated = true;
                    }
                }
            }
        }
    }while(is_updated);

    for(ItemSet itemset:itemsets)
    {
        for( int i : itemset)
        {
            Item item = all_items[i];
            cout << all_symbols[item.rule.left_id].content << " -> ";
            for (int j = 0; j < item.rule.right_ids.size(); j++)
            {
                if (j == item.dot)
                    cout << '.';
                else
                    cout << ' ';
                cout << all_symbols[item.rule.right_ids[j]].content;
            }
            cout << endl;
        }
        cout<<endl;
    }

//    set<int> r1 = get_closure(t);
//
//    for( int i : r1)
//    {
//        Item item = all_items[i];
//        cout << all_symbols[item.rule.left_id].content << " -> ";
//        for (int j = 0; j < item.rule.right_ids.size(); j++)
//        {
//            if (j == item.dot)
//                cout << '.';
//            else
//                cout << ' ';
//            cout << all_symbols[item.rule.right_ids[j]].content;
//        }
//        cout << endl;
//    }
//    cout<<endl;
//
//    set<int> r = get_go(r1, all_symbols[str_map_id["b"]]);
//
//    for( int i : r)
//    {
//        Item item = all_items[i];
//        cout << all_symbols[item.rule.left_id].content << " -> ";
//        for (int j = 0; j < item.rule.right_ids.size(); j++)
//        {
//            if (j == item.dot)
//                cout << '.';
//            else
//                cout << ' ';
//            cout << all_symbols[item.rule.right_ids[j]].content;
//        }
//        cout << endl;
//    }


//    for (Item item:all_items)
//    {
//        set<Item> itemset;
//        itemset.insert(item);
//
//        int B = item.rule.right_ids[item.dot];
//        if (!all_symbols[B].is_terminal)
//        {
//            for (Rule rule:all_rules)
//            {
//                if (rule.left_id == B)
//                {
//                    Item temp;
//                    temp.rule = rule;
//                    temp.dot = 0;
//                    itemset.insert(temp);
//                }
//            }
//        }
//    }
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
    all_rules.push_back(new_rule);
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
        all_symbols.push_back(new_symbol);

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
