#pragma once
#include <string>
#include <vector>
#include <list>
#include <iostream>

using std::vector;
using std::string;
using std::list;
using std::cin;
using std::cout;
using std::endl;

struct Location
{
    int line;
    int colum;
};


struct Word
{
    Location location;
    int type = 0;
    string content;
    int value = 0;
};

//class Lexical;
//class Grammar;
//class Semantic;
//class Optimize;
//class Analyze;