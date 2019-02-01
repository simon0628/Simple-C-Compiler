#include "common.h"
#include "Lexical.h"

list<Word> Lexical::analyze(vector<string> lines)
{
    prepare(lines);

    int j;
    unsigned long word_len;
    string word_str;
    for (auto word_iter = words.begin(); word_iter != words.end(); word_iter++)
    {
        word_str = (*word_iter).content;
        word_len = word_str.length();

        j = 0;
        char ch = word_str[j];

        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        { // 标识符或保留字
            ch = word_str[++j];
            while ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
            {
                ch = word_str[++j];
            }

            word_str = (*word_iter).content.substr(0,j);
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
                if((*word_iter).content[j] == '(')
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
        else if(ch == ';' || ch == ',')
        {
            (*word_iter).type = t_delimiter;
            j++;
        }
        else if(ch == '(' || ch == ')' || ch == '{' || ch == '}')
        {
            (*word_iter).type = t_bracket;
            j++;
        }
        else if(ch == '+' || ch == '-' || ch == '*' || ch == '/' )
        {
            (*word_iter).type = t_operator;
            j++;
        }
        else if(ch == '<' || ch == '>' || ch == '=')
        {
            (*word_iter).type = t_operator;
            j++;

            ch = word_str[j];
            if(ch == '=')
                j++;
        }
        else if(ch == '!')
        {
            (*word_iter).type = t_operator;
            if(word_str[j+1] == '=')
                j+=2;
        }
        else
        {
            (*word_iter).type = t_unknown;
            j++;
        }

        if ((*word_iter).type != t_unknown && j < word_len)
        {
            split_word(word_iter, j);
        }
    }
    return words;
}


/*
 * 处理文本类字符：' '、'\r'、'\t'、'\n'
 * 将源码按上述字符分割成words
 * 去除注释和上述字符
 */
void Lexical::prepare(vector<string> lines)
{
    int i = 0, j = 0, colum = 0; // j为字符索引，colum为位置索引，遇到\t时不一样
    bool disable = false;

    for (string line : lines)
    {
        j = 0;
        colum = 0;
        while(j < line.length())
        {
            if(line[j] == '*' && line[j+1] == '/')
            {
                disable = false;
                j+=2;
                colum +=2;
                continue;
            }
            if(disable)
            {
                j++;
                colum++;
                continue;
            }

            if (line[j] == '\t')
            {
                j++;
                colum+=4;
                continue;
            }
            else if(line[j] == ' ' || line[j] == '\r' || line[j] == '\n')
            {
                j++;
                colum++;
                continue;
            }
            else if(line[j] == '/' && line[j+1] == '/')
            {
                break;
            }
            else if(line[j] == '/' && line[j+1] == '*')
            {
                j+=2;
                colum +=2;
                disable = true;
                continue;
            }
            else
            {
                Word word;
                word.location.line = i;
                word.location.colum = colum;

                int j0 = j;
                j++;
                colum++;
                while (line[j] != '\t' && line[j] != ' ' && line[j] != '\r' && line[j] != '\n')
                {
                    j++;
                    colum++;
                }
                word.content = line.substr(j0, j-j0);

                words.push_back(word);
            }
        }
        i++;
    }
}

void Lexical::split_word(const list<Word>::iterator word_iter, int j)
{
    string word_str = (*word_iter).content;
    Word new_word;

    new_word.location.line = (*word_iter).location.line;
    new_word.location.colum = (*word_iter).location.colum + j;
    new_word.content = word_str.substr(j, word_str.length()-j);
    words.insert(std::next(word_iter), new_word);
    (*word_iter).content = word_str.substr(0, j);
}

namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 37,
        FG_BLUE     = 34,
        FG_MAGENTA  = 35,
        FG_CYAN     = 36,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}

Color::Modifier green(Color::FG_GREEN);
Color::Modifier blue(Color::FG_BLUE);
Color::Modifier def(Color::FG_DEFAULT);
Color::Modifier magenta(Color::FG_MAGENTA);

void Lexical::print_words()
{
    auto word_iter = words.begin();
    int len;
    while(word_iter != words.end())
    {
        switch((*word_iter).type)
        {
            case t_keyword: cout<<blue<<(*word_iter).content; break;
            case t_const: cout<<green<<(*word_iter).content; break;
            case t_function: cout<<magenta<<(*word_iter).content; break;
            default: cout<<def<<(*word_iter).content;
        }

        auto next_word = std::next(word_iter);
        if((*word_iter).location.line == (*next_word).location.line)
        {
            len = (*next_word).location.colum - (*word_iter).location.colum - (*word_iter).content.length();
            word_iter++;
        }
        else
        {
            word_iter++;
            len = (*word_iter).location.colum;
            cout<<endl;
        }
        for(int i=0; i<len; i++)
            cout<<' ';
    }

    cout<<endl;
    vector<string> lines;
    for(word_iter = words.begin(); word_iter!=words.end(); word_iter++)
    {
        string line = "<\'"+(*word_iter).content+"\',"+type_str[(*word_iter).type-1]+">";
        lines.push_back(line);
    }
    write_file("lexical_result.dat", lines);

}