#include "Analyze.h"
#include <iostream>
using namespace std;

Analyze::Analyze(const string filename)
{
    source_code = read_file(filename);

    // wait for cut source_code into pieces

    code_words = lexical.analyze(source_code);
    lexical.print_words();
//    write_result();
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