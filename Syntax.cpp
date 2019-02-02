//
// Created by simon on 2019/2/2.
//

#include "Syntax.h"

Syntax::Syntax()
{
    rules = string_to_rules(read_file("rules.dat"));

}

Syntax::~Syntax()
{}

vector<Rule> Syntax::string_to_rules(vector<string> lines)
{
    for(const string &line : lines)
    {

    }
}

void Syntax::analyze(list<Word> words)
{

}

void Syntax::save_result()
{

}