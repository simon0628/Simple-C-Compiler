#include "common.h"
#include "Lexical.h"

vector<Word> Lexical::analyze(vector<string> lines)
{
    prepare(lines);
    return words;
}


/*
 * 处理文本类字符：' '、'\r'、'\t'、'\n'
 * 将源码按上述字符分割成word
 * 并忽略上述字符
 */
void Lexical::prepare(vector<string> lines)
{
//    auto word_iter = words.begin();
    int i = 0 ,j = 0;
    for(string line : lines)
    {
        for(j=0; j<line.length(); j++)
        {
            if(line[j] != '\t' && line[j] != ' ' && line[j] != '\r' && line[j] !=  '\n')
            {
                Word word;
                word.location.line = i;
                word.location.colum = j;

                int j0 = j;
                while(line[j] != '\t' && line[j] != ' ' && line[j] != '\r' && line[j] !=  '\n')
                    j++;
                word.content = line.substr(j0, j);

                words.push_back(word);
            }
        }
        i++;
    }
}
