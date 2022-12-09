#pragma once
#include "Lexer.h"
#include "Lexer.cpp"
#include "GETanalyzer.h"
#include "GETanalyzer.cpp"
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

#define COLWIDTH 40

class LRanalyzer
{
public:
	LRanalyzer(GETanalyzer& Ga);
	int LRanalyze(string line, queue<pair<string, string>> lex_res, bool drawtree = false);

private:
	int num_status;
	elem** LRtable;
	vector<pair<int, pair<string, string>>> StatusSymbolStack;
	unordered_map<string, int> symbolmapping;
	Production* productions;

	void printStack(string line);
};
