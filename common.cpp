#include "common.h"
vector<string> read_file(const string filename)
{

    std::fstream fin(filename, std::ios::in);
    if(!fin.is_open())
    {
        cerr<<"cannot open file "<<filename <<endl;
        exit(EXIT_FAILURE);
    }

    vector<string> lines;
    string line;
    while(!fin.eof())
    {
        getline(fin, line);
        lines.push_back(line);
    }
    return lines;
}

Status write_file(const string filename, vector<string> lines)
{
    std::fstream fout(filename, std::ios::out);
    if(!fout.is_open())
    {
        cerr<<"cannot open file "<<filename <<endl;
        exit(EXIT_FAILURE);
    }

    for(const string &line : lines)
    {
        fout<<line<<endl;
    }
    return OK;
}