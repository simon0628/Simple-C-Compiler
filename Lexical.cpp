#include "common.h"
#include "Lexical.h"


/*
 * 功能：词法分析主程序，public
 * 说明：
 *      根据DFA扫描
 *      将源程序分割成词
 *      并按内容赋其type
 * 返回：words链表用于语法分析
 */
list<Word> Lexical::analyze(vector<string> lines)
{
    prepare(lines);

    // prepare将结果按空格分割，接下来还需按符号分割
    int j;
    unsigned long word_len;
    string word_str;
    for (auto word_iter = words.begin(); word_iter != words.end(); word_iter++)
    {
        if((*word_iter).type == t_string) // 在prepare内已经将字符串常量分割完毕
            continue;

        word_str = (*word_iter).content;
        word_len = word_str.length();

        j = 0;
        char ch = word_str[j];

        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
        { // 标识符或保留字
            ch = word_str[++j];
            while ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
            {
                ch = word_str[++j];
            }

            word_str = (*word_iter).content.substr(0, j);
            bool is_keyword = false;
            for (const string &keyword : keywords)
            {
                if (word_str == keyword)
                {
                    (*word_iter).type = t_keyword;
                    is_keyword = true;
                    break;
                }
            }
            if (!is_keyword)
            {
                if ((*word_iter).content[j] == '(')
                    (*word_iter).type = t_function;
                else
                    (*word_iter).type = t_variable;
            }
        }
        else if (ch >= '0' && ch <= '9')
        { // 常量值
            int sum = 0;
            while (ch >= '0' && ch <= '9')
            {
                sum = sum * 10 + ch - '0';
                ch = word_str[++j];
            }
            (*word_iter).value = sum;
            (*word_iter).type = t_const;
        }
        else if (ch == ';' || ch == ',')
        {
            (*word_iter).type = t_delimiter;
            j++;
        }
        else if (ch == '(' || ch == ')' || ch == '{' || ch == '}')
        {
            (*word_iter).type = t_bracket;
            j++;
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
        {
            (*word_iter).type = t_operator;
            j++;
        }
        else if (ch == '<' || ch == '>' || ch == '=' || ch == '!')
        {
            (*word_iter).type = t_operator;
            j++;

            ch = word_str[j];
            if (ch == '=')
                j++;
        }
        else
        {
            (*word_iter).type = t_unknown;
            j++;
        }

        if (j < word_len)
        {
            split_word(word_iter, j);
        }
    }
    return words;
}


/*
 * 功能：格式化代码
 * 说明：
 *      处理文本类字符：' '、'\r'、'\t'、'\n'
 *      将源码按上述字符分割成words
 *      去除注释和上述字符
 *      记录每个词的位置
 * 返回：无，结果存在words成员变量内
 */
void Lexical::prepare(vector<string> lines)
{
    int i = 0, j = 0, colum = 0, j0; // j为字符索引，colum为位置索引，只有遇到\t时不一样
    bool in_comment = false;         // 在多行注释内
    bool in_quotation = false;       // 在引号内
    bool recording = false;          // 正在记录长短语
    Word *new_word;

    for (string line : lines)
    {
        j = 0;
        colum = 0;

        while (j < line.length())
        {
            // C语言忽略引号内的注释符，优先判断引号
            if(in_quotation && line[j] == '"' ) // 在引号内且遇到第二个引号，结束本段
            {
                in_quotation = false;

                // 将本段作为字符串常量分割出来
                if (recording)
                {
                    recording = false;
                    new_word->content = line.substr(j0, j - j0 + 1);
                    new_word->type = t_string;
                    words.push_back(*new_word);
                }
                j++;
                colum++;
                continue;
            }

            if (in_comment && line[j] == '*' && line[j + 1] == '/') // 在多行注释内遇到结尾，结束本段
            {
                in_comment = false;
                j += 2;
                colum += 2;
                continue;
            }

            if (in_comment || in_quotation) // 在多行注释或引号内，继续向后索引
            {
                j++;
                colum++;
                continue;
            }



            if (line[j] == '\t')
            {
                j++;
                colum += 4;
            }
            else if(line[j] == '"' && line[j+1] != '\'') // 第一次遇到双引号，且此双引号不在单引号内
            {
                if(recording)
                {
                    recording = false;
                    new_word->content = line.substr(j0, j - j0);
                    words.push_back(*new_word);
                }
                in_quotation = true;

                //开始记录本段字符串常量
                if (!recording)
                {
                    recording = true;
                    new_word = new Word;
                    new_word->location.line = i;
                    new_word->location.colum = colum;

                    j0 = j;
                }

                j++;
                colum++;
            }
            else if (line[j] == '/' && line[j + 1] == '/') // 双杠注释直接跳过本行
            {
                break;
            }
            else if (line[j] == '/' && line[j + 1] == '*') // 第一次遇到多行注释
            {
                j += 2;
                colum += 2;
                in_comment = true;
            }
            else if (line[j] == ' ' || line[j] == '\r' || line[j] == '\n' || line[j] == '\0')
            {
                // 遇到文本类字符，若正在记录长短语，结束此段并分割
                if (recording)
                {
                    recording = false;
                    new_word->content = line.substr(j0, j - j0);
                    words.push_back(*new_word);
                }
                j++;
                colum++;
            }
            else
            {
                // 为可接受短语，可以开始记录
                if (!recording)
                {
                    recording = true;
                    new_word = new Word;
                    new_word->location.line = i;
                    new_word->location.colum = colum;

                    j0 = j;
                }
                j++;
                colum++;
            }
        }

        // 若到达一行的末尾仍在记录，则结束记录并分割
        if (recording)
        {
            recording = false;
            new_word->content = line.substr(j0, j - j0);
            words.push_back(*new_word);
        }
        i++;
    }
}

/*
 * 功能：在指定位置分割此短语，后一半作为链表的新结点插入
 */
void Lexical::split_word(const list<Word>::iterator word_iter, int j)
{
    string word_str = (*word_iter).content;
    Word new_word;

    new_word.location.line = (*word_iter).location.line;
    new_word.location.colum = (*word_iter).location.colum + j;
    new_word.content = word_str.substr(j, word_str.length() - j);
    words.insert(std::next(word_iter), new_word);
    (*word_iter).content = word_str.substr(0, j);
}

/*
 * 功能：cmd测试使用，为输出程序赋颜色
 */
namespace Color
{
    enum Code
    {
        FG_RED = 31,
        FG_GREEN = 32,
        FG_BLUE = 34,
        FG_MAGENTA = 35,
        FG_CYAN = 36,
        FG_GREY = 37,
        FG_DEFAULT = 39,
        BG_RED = 41,
        BG_GREEN = 42,
        BG_BLUE = 44,
        BG_DEFAULT = 49
    };

    class Modifier
    {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode)
        {}

        friend std::ostream &
        operator<<(std::ostream &os, const Modifier &mod)
        {
            return os << "\033[" << mod.code << "m";
        }
    };
}

Color::Modifier grey(Color::FG_GREY);
Color::Modifier blue(Color::FG_BLUE);
Color::Modifier green(Color::FG_GREEN);
Color::Modifier magenta(Color::FG_MAGENTA);
Color::Modifier def(Color::FG_DEFAULT);

/*
 * 功能：cmd测试使用，为输出程序赋颜色
 */
void Lexical::print_words()
{
    auto word_iter = words.begin();
    int len;
    while (word_iter != words.end())
    {
        switch ((*word_iter).type)
        {
            case t_keyword:
                cout << blue << (*word_iter).content;
                break;
            case t_const:
                cout << grey << (*word_iter).content;
                break;
            case t_string:
                cout << green << (*word_iter).content;
                break;
            case t_function:
                cout << magenta << (*word_iter).content;
                break;
            default:
                cout << def << (*word_iter).content;
        }

        auto next_word = std::next(word_iter);
        if ((*word_iter).location.line == (*next_word).location.line)
        {
            len = (*next_word).location.colum - (*word_iter).location.colum - (*word_iter).content.length();
            word_iter++;
        }
        else
        {
            word_iter++;
            len = (*word_iter).location.colum;
            cout << endl;
        }
        for (int i = 0; i < len; i++)
            cout << ' ';
    }
    cout << endl;


    vector<string> lines;
    string line;
    for (word_iter = words.begin(); word_iter != words.end(); word_iter++)
    {
        line += "<\'" + (*word_iter).content + "\':" + type_str[(*word_iter).type] + ">   ";
        if(std::next(word_iter)->location.line != word_iter->location.line)
        {
            lines.push_back(line);
//            cout<<line<<endl;
            line = "";
        }
    }
    write_file("lexical_result.dat", lines);

}