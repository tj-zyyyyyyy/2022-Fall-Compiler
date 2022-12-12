#pragma once
#include <string>
#include <vector>
#include "GETanalyzer.h"
using namespace std;

struct Quaternary {
	string op;
	string src1;
	string src2;
	string des;
};

class TempCode
{
public:
	void push(string* sym);
	void reduce(Production pro);
	void emit(string op, string src1, string src2, string des);
	void printTempCode(const char* Filename);

private:
	vector<Quaternary> code;
	vector<string*>symbol;

	void popStack(int count);
};

enum DecType {
	DEC_VAR, DEC_FUN
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
	Num(const string& sym, const string& number);
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
