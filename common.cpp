#include "common.h"

bool is_letter(char ch)
{
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}
vector<string> read_file(const string filename)
{

    std::fstream fin(filename, std::ios::in);
    if (!fin.is_open())
    {
        cerr << "cannot open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> lines;
    string line;
    while (!fin.eof())
    {
        getline(fin, line);
        lines.push_back(line);
    }
    return lines;
}

Status write_file(const string filename, vector<string> lines)
{
    std::fstream fout(filename, std::ios::out);
    if (!fout.is_open())
    {
        cerr << "cannot open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    for (const string &line : lines)
    {
        fout << line << endl;
    }
    return OK;
}

vector<string> str_split(const string &s, const string &seperator)
{
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size())
    {
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while (i != s.size() && flag == 0)
        {
            flag = 1;
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[i] == seperator[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0)
        {
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[j] == seperator[x])
                {
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ++j;
        }
        if (i != j)
        {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}