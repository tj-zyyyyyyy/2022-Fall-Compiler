#pragma once
#include "Lexer.h"
#include "Lexer.cpp"
#include "GETanalyzer.h"
#include "GETanalyzer.cpp"
#include "TempCode.h"
#include "TempCode.cpp"
#include <string>
#include <queue>
#include <unordered_map>
#include <stack>
#include <vector>
using namespace std;

#define COLWIDTH 40

//changed start
//声明类型（变量声明/函数声明）
enum DecType {
	DEC_VAR,DEC_FUN
};

//数据类型（int/void）
enum DType { D_VOID, D_INT };


struct Var {
	string name;
	DType type;
	int level;
};

struct Func {
	string name;
	DType returnType;
	list<DType> paramTypes;
	int enterPoint;
};

class Id
{
public:
	string name;
	Id(const string& sym, const string& name);
};

class Num
{
public:
	string number;
	Num(const string& sym,const string& number);
};

class FunctionDeclare
{
public:
	list<DType>plist;
	FunctionDeclare(const string& sym);
};

class Parameter
{
public:
	list<DType>plist;
	Parameter(const string& sym);
};

class ParameterList
{
public:
	list<DType>plist;
	ParameterList(const string& sym);
};

class SentenceBlock
{
public:
	list<int>nextList;
	SentenceBlock(const string& sym);
};

class SentenceList
{
public:
	list<int>nextList;
	SentenceList(const string& sym);
};

class Sentence
{
public:
	list<int>nextList;
	Sentence(const string& sym);
};

class WhileSentence
{
public:
	list<int>nextList;
	WhileSentence(const string& sym);
};

class IfSentence
{
public:
	list<int>nextList;
	IfSentence(const string& sym);
};

class Expression
{
public:
	string name;
	list<int>falseList;
	Expression(const string& sym);
};

class M
{
public:
	int quad;
	M(const string& sym);
};

class N
{
public:
	list<int> nextList;
	N(const string& sym);
};

class AddExpression
{
public:
	string name;
	AddExpression(const string& sym);
};

class Nomial
{
public:
	string name;
	Nomial(const string& sym);
};

class Factor
{
public:
	string name;
	Factor(const string& sym);
};

class ArgumentList
{
public:
	list<string> alist;
	ArgumentList(const string& sym);
};

class NewTemper 
{
private:
	int now;
public:
	NewTemper();
	string newTemp();
};
//changed end

class LRanalyzer
{
public:
	LRanalyzer(GETanalyzer& Ga);
    void popStack(stack<int> &idxStack, vector<int> &idx_sub, bool drawtree);
    int LRanalyze(string line, queue<pair<string, string>> lex_res, bool drawtree = false);
	void pushStack(string* content,ofstream &out, Production pro, stack<int> &idxStack, vector<int> &idx_sub, bool drawtree, int &idx_node);

private:
	int num_status;
	elem** LRtable;
	vector<pair<int, pair<string, string*>>> StatusSymbolStack;
	unordered_map<string, int> symbolmapping;
	Production* productions;
	TempCode code;
	NewTemper nt;

	void printStack(string line);
};
