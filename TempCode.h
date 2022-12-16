#pragma once
#include <string>
#include <vector>
#include "GETanalyzer.h"
#include "GETanalyzer.cpp"
using namespace std;

struct Quaternary
{
	string op;
	string src1;
	string src2;
	string des;
};

enum DecType
{
	DEC_VAR,
	DEC_FUN
};

// 数据类型（int/void）
enum DType
{
	D_VOID,
	D_INT
};

struct Var
{
	string name;
	DType type;
	int level; // 作用域
};

struct Func
{
	string name;
	DType returnType;
	list<DType> paramTypes;
	int enterPoint;
};

class NewTemper
{
private:
	int now;

public:
	NewTemper();
	string newTemp();
};

class TempCode
{
public:
	void push(string *sym);
	void reduce(Production pro, int idx_pro);
	void outputError(string str);
	Func *lookUpFunc(string ID);
	Var *lookUpVar(string ID);
	bool march(list<string> &argument_list, list<DType> &parameter_list); // 比较参数列表
	void emit(string op, string src1, string src2, string des);			  // 生成三地址代码
	void back_patch(list<int> nextList, int quad);
	int nextQuad();
	void printTempCode(const char *Filename);
	bool flag = true;

private:
	int nowlevel = 0; // 作用域层级
	vector<Quaternary> code;
	vector<string *> symbol;
	vector<Var> VarTable;
	vector<Func> FuncTable;
	NewTemper nt;

	void popStack(int count);
};

class Id
{
public:
	string name;
	Id(const string &sym, const string &name);
};

class Num
{
public:
	string number;
	Num(const string &sym, const string &number);
};

class FunctionDeclare
{
public:
	string name;
	list<DType> plist;
	FunctionDeclare(const string &sym);
};

class Parameter
{
public:
	string name;
	list<DType> plist;
	Parameter(const string &sym);
};

class ParameterList
{
public:
	string name;
	list<DType> plist;
	ParameterList(const string &sym);
};

class SentenceBlock
{
public:
	string name;
	list<int> nextList;
	SentenceBlock(const string &sym);
};

class SentenceList
{
public:
	string name;
	list<int> nextList;
	SentenceList(const string &sym);
};

class Sentence
{
public:
	string name;
	list<int> nextList;
	Sentence(const string &sym);
};

class WhileSentence
{
public:
	string name;
	list<int> nextList;
	WhileSentence(const string &sym);
};

class IfSentence
{
public:
	string name;
	list<int> nextList;
	IfSentence(const string &sym);
};

class Expression
{
public:
	string name;
	list<int> falseList;
	Expression(const string &sym);
};

class M
{
public:
	string name;
	int quad;
	M(const string &sym);
};

class N
{
public:
	string name;
	list<int> nextList;
	N(const string &sym);
};

class AddExpression
{
public:
	string name;
	AddExpression(const string &sym);
};

class Nomial
{
public:
	string name;
	Nomial(const string &sym);
};

class Factor
{
public:
	string name;
	Factor(const string &sym);
};

class ArgumentList
{
public:
	string name;
	list<string> alist;
	ArgumentList(const string &sym);
};
