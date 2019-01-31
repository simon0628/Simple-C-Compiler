#pragma once
#include <string>
#include <vector>
#include <iostream>

using std::vector;
using std::string;
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
    int types;
    string content;
};

//class Lexical;
//class Grammar;
//class Semantic;
//class Optimize;
//class Analyze;