#include <fstream>
#include <iostream>
#include "TempCode.h"

void TempCode::push(string* sym)
{
    symbol.push_back(sym);
    return;
}

void TempCode::reduce(Production pro)
{
	if (pro.left_symbol == "<Assign>")  //<Assign> ::= id = <Exp>
	{
		Expression* exp = (Expression*)symbol.back();
		Id* id = (Id*)symbol[symbol.size() - 3];
		string* assign_sentence = new string(pro.left_symbol);
		emit(":=", exp->name, "-", id->name);
		popStack(pro.right_symbol.size());
		symbol.push_back(new string(pro.left_symbol));
	}
	else if (pro.left_symbol == "<Exp>")  //<Exp> ::= <AddSubExp> <Tmp3>
	{
		string* tmp3 = symbol.back();
		AddExpression* addsubexp = (AddExpression*)symbol[symbol.size() - 2];
		Expression* expression = new Expression(pro.left_symbol);
		expression->name = addsubexp->name;
		popStack(pro.right_symbol.size());
		symbol.push_back((string*)expression);
	}
    return;
}

void TempCode::emit(string op, string src1, string src2, string des)
{
    code.push_back(Quaternary{ op,src1,src2,des });
	return;
}

void TempCode::printTempCode(const char* Filename)
{
    auto oldbuf = cout.rdbuf();
    ofstream out(Filename);
    cout.rdbuf(out.rdbuf());
    for (int i = 0; i < code.size(); i++)
    {
        cout << "(" << code[i].op << "," << code[i].src1 << "," << code[i].src2 << "," << code[i].des << ")\n";
    }
    cout.rdbuf(oldbuf);
    out.close();
	return;
}

void TempCode::popStack(int count)
{
	while (count--)
		symbol.pop_back();
	return;
}

list<int>merge(list<int>& l1, list<int>& l2) {
	list<int>ret;
	ret.assign(l1.begin(), l1.end());
	ret.splice(ret.end(), l2);
	return ret;
}

NewTemper::NewTemper() {
	now = 0;
}

string NewTemper::newTemp() {
	return string("T") + to_string(now++);
}

Id::Id(const string& sym, const string& name) {
	this->name = name;
}

Num::Num(const string& sym, const string& number) {
	this->number = number;
}

Expression::Expression(const string& sym) {
	this->name = sym;
}

AddExpression::AddExpression(const string& sym) {
	this->name = sym;
}
