#include "Analyze.h"
#include <fstream>
#include <iostream>
using namespace std;

Analyze::Analyze(char *filename)
{
    source_code = read_file(filename);

    // wait for cut source_code into pieces

    code_words = lexical.analyze(source_code);
    write_result();
}

vector<string> Analyze::read_file(char *filename)
{

    fstream fin(filename, ios::in);
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

// void Analyze::lexical_analyze();
 void Analyze::write_result()
{
    auto it = source_code.begin();
    while(it != source_code.end())
    {
        cout<<*it<<endl;
        it++;
    }
}