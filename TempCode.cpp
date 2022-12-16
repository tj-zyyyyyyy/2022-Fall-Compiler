#include <fstream>
#include <iostream>
#include "TempCode.h"

list<int> mymerge(list<int> &l1, list<int> &l2)
{
	list<int> ret;
	ret.assign(l1.begin(), l1.end());
	ret.splice(ret.end(), l2);
	return ret;
}

int TempCode::nextQuad()
{
	return code.size();
}

void TempCode::push(string *sym)
{
	symbol.push_back(sym);
	return;
}

void TempCode::reduce(Production pro, int idx_pro)
{
	switch (idx_pro)
	{
	case 3: // declare ::= int ID M A function_declare
	{
		FunctionDeclare *function_declare = (FunctionDeclare *)symbol.back();
		M *m = (M *)symbol[symbol.size() - 3];
		Id *ID = (Id *)symbol[symbol.size() - 4];
		popStack(pro.right_symbol.size());
		FuncTable.push_back(Func{ID->name, D_INT, function_declare->plist, m->quad});
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	case 4: // declare ::= int ID ;
	{
		Id *ID = (Id *)symbol[symbol.size() - 2];
		popStack(pro.right_symbol.size());
		VarTable.push_back(Var{ID->name, D_INT, nowlevel});
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	case 5: // declare ::= void ID M A function_declare
	{
		FunctionDeclare *function_declare = (FunctionDeclare *)symbol.back();
		M *m = (M *)symbol[symbol.size() - 3];
		Id *ID = (Id *)symbol[symbol.size() - 4];
		popStack(pro.right_symbol.size());
		FuncTable.push_back(Func{ID->name, D_VOID, function_declare->plist, m->quad});
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	case 6: // function_declare ::= ( parameter ) sentence_block
	{
		SentenceBlock *sentence_block = (SentenceBlock *)symbol.back();
		Parameter *paramter = (Parameter *)symbol[symbol.size() - 3];
		FunctionDeclare *function_declare = new FunctionDeclare(pro.left_symbol);
		popStack(pro.right_symbol.size());
		function_declare->plist.assign(paramter->plist.begin(), paramter->plist.end());
		symbol.push_back((string *)function_declare);
		break;
	}
	case 7: // parameter :: = parameter_list
	{
		ParameterList *parameter_list = (ParameterList *)symbol.back();
		Parameter *parameter = new Parameter(pro.left_symbol);
		popStack(pro.right_symbol.size());
		parameter->plist.assign(parameter_list->plist.begin(), parameter_list->plist.end());
		symbol.push_back((string *)parameter);
		break;
	}
	case 8: // parameter ::= void
	{
		Parameter *parameter = new Parameter(pro.left_symbol);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)parameter);
		break;
	}
	case 9: // parameter_list ::= param
	{
		ParameterList *parameter_list = new ParameterList(pro.left_symbol);
		parameter_list->plist.push_back(D_INT);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)parameter_list);
		break;
	}
	case 10: // parameter_list1 ::= param , parameter_list2
	{
		ParameterList *parameter_list2 = (ParameterList *)symbol.back();
		ParameterList *parameter_list1 = new ParameterList(pro.left_symbol);
		popStack(pro.right_symbol.size());
		parameter_list2->plist.push_front(D_INT);
		parameter_list1->plist.assign(parameter_list2->plist.begin(), parameter_list2->plist.end());
		symbol.push_back((string *)parameter_list1);
		break;
	}
	case 11: // param ::= int ID
	{
		Id *ID = (Id *)symbol.back();
		popStack(pro.right_symbol.size());
		VarTable.push_back(Var{ID->name, D_INT, nowlevel});
		emit("get", "-", "-", ID->name);
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	case 12: // sentence_block ::= { inner_declare sentence_list }
	{
		SentenceList *sentence_list = (SentenceList *)symbol[symbol.size() - 2];
		SentenceBlock *sentence_block = new SentenceBlock(pro.left_symbol);
		sentence_block->nextList = sentence_list->nextList;
		//弹出变量表作用域到期的变量
		nowlevel--;
		int popNum = 0;
		for (vector<Var>::reverse_iterator riter = VarTable.rbegin(); riter != VarTable.rend(); riter++)
		{
			if (riter->level > nowlevel)
				popNum++;
			else
				break;
		}
		for (int i = 0; i < popNum; i++)
		{
			VarTable.pop_back();
		}
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)sentence_block);
		break;
	}
	case 15: // inner_var_declare ::= int ID
	{
		Id *ID = (Id *)symbol.back();
		popStack(pro.right_symbol.size());
		symbol.push_back(new string(pro.left_symbol));
		VarTable.push_back(Var{ID->name, D_INT, nowlevel});
		break;
	}
	case 16: // sentence_list ::= sentence M sentence_list
	{
		SentenceList *sentence_list2 = (SentenceList *)symbol.back();
		M *m = (M *)symbol[symbol.size() - 2];
		Sentence *sentence = (Sentence *)symbol[symbol.size() - 3];
		SentenceList *sentence_list1 = new SentenceList(pro.left_symbol);
		sentence_list1->nextList = sentence_list2->nextList;
		back_patch(sentence->nextList, m->quad);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)sentence_list1);
		break;
	}
	case 17: // sentence_list ::= sentence
	{
		Sentence *sentence = (Sentence *)symbol.back();
		SentenceList *sentence_list = new SentenceList(pro.left_symbol);
		sentence_list->nextList = sentence->nextList;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)sentence_list);
		break;
	}
	case 18: // sentence ::= if_sentence
	{
		IfSentence *if_sentence = (IfSentence *)symbol.back();
		Sentence *sentence = new Sentence(pro.left_symbol);
		sentence->nextList = if_sentence->nextList;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)sentence);
		break;
	}
	case 19: // sentence ::= while_sentence
	{
		WhileSentence *while_sentence = (WhileSentence *)symbol.back();
		Sentence *sentence = new Sentence(pro.left_symbol);
		sentence->nextList = while_sentence->nextList;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)sentence);
		break;
	}
	case 20: // sentence ::= return_sentence
	{
		Sentence *sentence = new Sentence(pro.left_symbol);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)sentence);
		break;
	}
	case 21: // sentence ::= assign_sentence
	{
		Sentence *sentence = new Sentence(pro.left_symbol);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)sentence);
		break;
	}
	case 22: //<assign_sentence> ::= id = <expression> ;
	{
		Expression *exp = (Expression *)symbol[symbol.size() - 2];
		Id *id = (Id *)symbol[symbol.size() - 4];
		if (!lookUpVar(id->name))
		{
			outputError("ERROR:Variable " + id->name + " not declared");
		}
		string *assign_sentence = new string(pro.left_symbol);
		emit(":=", exp->name, "-", id->name);
		popStack(pro.right_symbol.size());
		symbol.push_back(assign_sentence);
		break;
	}
	case 23: // return_sentence ::= return ;
	{
		emit("return", "-", "-", "-");
		popStack(pro.right_symbol.size());
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	case 24: // return_sentence ::= return expression ;
	{
		Expression *expression = (Expression *)symbol[symbol.size() - 2];
		emit("return", expression->name, "-", "-");
		popStack(pro.right_symbol.size());
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	case 25: // while_sentence ::= while M ( expression ) A sentence_block
	{
		SentenceBlock *sentence_block = (SentenceBlock *)symbol[symbol.size() - 1];
		Expression *expression = (Expression *)symbol[symbol.size() - 4];
		M *m = (M *)symbol[symbol.size() - 6];
		WhileSentence *while_sentence = new WhileSentence(pro.left_symbol);
		back_patch(sentence_block->nextList, m->quad);
		while_sentence->nextList = expression->falseList;
		emit("j", "-", "-", to_string(m->quad));
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)while_sentence);
		break;
	}
	case 26: // if_sentence ::= if ( expression ) A sentence_block
	{
		SentenceBlock *sentence_block = (SentenceBlock *)symbol[symbol.size() - 1];
		Expression *expression = (Expression *)symbol[symbol.size() - 4];
		IfSentence *if_sentence = new IfSentence(pro.left_symbol);
		expression->falseList.splice(expression->falseList.begin(), sentence_block->nextList);
		if_sentence->nextList = expression->falseList;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)if_sentence);
		break;
	}
	case 27: // if_sentence ::= if ( expression ) A1 sentence_block1 N else M  A2 sentence_block2
	{
		SentenceBlock *sentence_block2 = (SentenceBlock *)symbol[symbol.size() - 1];
		M *m = (M *)symbol[symbol.size() - 3];
		N *n = (N *)symbol[symbol.size() - 5];
		SentenceBlock *sentence_block1 = (SentenceBlock *)symbol[symbol.size() - 6];
		Expression *expression = (Expression *)symbol[symbol.size() - 9];
		IfSentence *if_sentence = new IfSentence(pro.left_symbol);
		back_patch(expression->falseList, m->quad);
		if_sentence->nextList = mymerge(sentence_block1->nextList, sentence_block2->nextList);
		if_sentence->nextList = mymerge(if_sentence->nextList, n->nextList);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)if_sentence);
		break;
	}
	case 28: // N ::=
	{
		N *n = new N(pro.left_symbol);
		n->nextList.push_back(nextQuad());
		emit("j", "-", "-", "-");
		symbol.push_back((string *)n);
		break;
	}
	case 29: // M ::=
	{
		M *m = new M(pro.left_symbol);
		m->quad = nextQuad();
		symbol.push_back((string *)m);
		break;
	}
	case 30: //<expression> ::= <add_expression>
	{
		AddExpression *addexp = (AddExpression *)symbol.back();
		Expression *expression = new Expression(pro.left_symbol);
		expression->name = addexp->name;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)expression);
		break;
	}
	case 31: // expression ::= add_expression1 > add_expression2
	{
		AddExpression *add_expression2 = (AddExpression *)symbol[symbol.size() - 1];
		AddExpression *add_expression1 = (AddExpression *)symbol[symbol.size() - 3];
		Expression *expression = new Expression(pro.left_symbol);
		expression->falseList.push_back(nextQuad());
		emit("j<=", add_expression1->name, add_expression2->name, "-1");
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)expression);
		break;
	}
	case 32: // expression ::= add_expression1 < add_expression2
	{
		AddExpression *add_expression2 = (AddExpression *)symbol[symbol.size() - 1];
		AddExpression *add_expression1 = (AddExpression *)symbol[symbol.size() - 3];
		Expression *expression = new Expression(pro.left_symbol);
		expression->falseList.push_back(nextQuad());
		emit("j>=", add_expression1->name, add_expression2->name, "-1");
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)expression);
		break;
	}
	case 33: // expression ::= add_expression1 == add_expression2
	{
		AddExpression *add_expression2 = (AddExpression *)symbol[symbol.size() - 1];
		AddExpression *add_expression1 = (AddExpression *)symbol[symbol.size() - 3];
		Expression *expression = new Expression(pro.left_symbol);
		expression->falseList.push_back(nextQuad());
		emit("j!=", add_expression1->name, add_expression2->name, "-1");
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)expression);
		break;
	}
	case 34: // expression ::= add_expression1 >= add_expression2
	{
		AddExpression *add_expression2 = (AddExpression *)symbol[symbol.size() - 1];
		AddExpression *add_expression1 = (AddExpression *)symbol[symbol.size() - 3];
		Expression *expression = new Expression(pro.left_symbol);
		expression->falseList.push_back(nextQuad());
		emit("j<", add_expression1->name, add_expression2->name, "-1");
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)expression);
		break;
	}
	case 35: // expression ::= add_expression1 <= add_expression2
	{
		AddExpression *add_expression2 = (AddExpression *)symbol[symbol.size() - 1];
		AddExpression *add_expression1 = (AddExpression *)symbol[symbol.size() - 3];
		Expression *expression = new Expression(pro.left_symbol);
		expression->falseList.push_back(nextQuad());
		emit("j>", add_expression1->name, add_expression2->name, "-1");
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)expression);
		break;
	}
	case 36: // expression ::= add_expression1 != add_expression2
	{
		AddExpression *add_expression2 = (AddExpression *)symbol[symbol.size() - 1];
		AddExpression *add_expression1 = (AddExpression *)symbol[symbol.size() - 3];
		Expression *expression = new Expression(pro.left_symbol);
		expression->falseList.push_back(nextQuad());
		emit("j==", add_expression1->name, add_expression2->name, "-1");
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)expression);
		break;
	}
	case 37: // add_expression ::= item
	{
		Nomial *item = (Nomial *)symbol.back();
		AddExpression *add_expression = new AddExpression(pro.left_symbol);
		add_expression->name = item->name;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)add_expression);
		break;
	}
	case 38: // add_expression1 ::= item + add_expression2
	{
		AddExpression *add_expression2 = (AddExpression *)symbol.back();
		Nomial *item = (Nomial *)symbol[symbol.size() - 3];
		AddExpression *add_expression1 = new AddExpression(pro.left_symbol);
		add_expression1->name = nt.newTemp();
		emit("+", item->name, add_expression2->name, add_expression1->name);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)add_expression1);
		break;
	}
	case 39: // add_expression ::= item - add_expression
	{
		AddExpression *add_expression2 = (AddExpression *)symbol.back();
		Nomial *item = (Nomial *)symbol[symbol.size() - 3];
		AddExpression *add_expression1 = new AddExpression(pro.left_symbol);
		add_expression1->name = nt.newTemp();
		emit("-", item->name, add_expression2->name, add_expression1->name);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)add_expression1);
		break;
	}
	case 40: // item ::= factor
	{
		Factor *factor = (Factor *)symbol.back();
		Nomial *item = new Nomial(pro.left_symbol);
		item->name = factor->name;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)item);
		break;
	}
	case 41: // item1 ::= factor * item2
	{
		Nomial *item2 = (Nomial *)symbol.back();
		Factor *factor = (Factor *)symbol[symbol.size() - 3];
		Nomial *item1 = new Nomial(pro.left_symbol);
		item1->name = nt.newTemp();
		emit("*", factor->name, item2->name, item1->name);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)item1);
		break;
	}
	case 42: // item1 ::= factor / item2
	{
		Nomial *item2 = (Nomial *)symbol.back();
		Factor *factor = (Factor *)symbol[symbol.size() - 3];
		Nomial *item1 = new Nomial(pro.left_symbol);
		item1->name = nt.newTemp();
		emit("/", factor->name, item2->name, item1->name);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)item1);
		break;
	}
	case 43: // factor ::= NUM
	{
		Num *num = (Num *)symbol.back();
		Factor *factor = new Factor(pro.left_symbol);
		factor->name = num->number;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)factor);
		break;
	}
	case 44: // factor ::= ( expression )
	{
		Expression *expression = (Expression *)symbol[symbol.size() - 2];
		Factor *factor = new Factor(pro.left_symbol);
		factor->name = expression->name;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)factor);
		break;
	}
	case 45: // factor ::= ID ( argument_list )
	{
		ArgumentList *argument_list = (ArgumentList *)symbol[symbol.size() - 2];
		Id *ID = (Id *)symbol[symbol.size() - 4];
		Factor *factor = new Factor(pro.left_symbol);
		Func *f = lookUpFunc(ID->name);
		if (!f)
		{
			outputError("ERROR:Function " + ID->name + " not declared");
		}
		else if (!march(argument_list->alist, f->paramTypes))
		{
			outputError("ERROR:Function " + ID->name + " argument list mismatch");
		}
		for (list<string>::iterator iter = argument_list->alist.begin(); iter != argument_list->alist.end(); iter++)
		{
			emit("param", *iter, "-", "-");
		}
		factor->name = nt.newTemp();
		emit("call", ID->name, "-", "-");
		emit(":=", "@RETURN_PLACE", "-", factor->name);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)factor);
		break;
	}
	case 46: // factor ::= ID
	{
		Id *ID = (Id *)symbol.back();
		if (!lookUpVar(ID->name))
		{
			outputError("ERROR:Variable " + ID->name + " not declared");
		}
		Factor *factor = new Factor(pro.left_symbol);
		factor->name = ID->name;
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)factor);
		break;
	}
	case 47: // argument_list ::=
	{
		ArgumentList *argument_list = new ArgumentList(pro.left_symbol);
		symbol.push_back((string *)argument_list);
		break;
	}
	case 48: // argument_list ::= expression
	{
		Expression *expression = (Expression *)symbol[symbol.size() - 1];
		ArgumentList *argument_list = new ArgumentList(pro.left_symbol);
		argument_list->alist.push_back(expression->name);
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)argument_list);
		break;
	}
	case 49: // argument_list1 ::= expression , argument_list2
	{
		ArgumentList *argument_list2 = (ArgumentList *)symbol[symbol.size() - 1];
		Expression *expression = (Expression *)symbol[symbol.size() - 3];
		ArgumentList *argument_list1 = new ArgumentList(pro.left_symbol);
		argument_list2->alist.push_front(expression->name);
		argument_list1->alist.assign(argument_list2->alist.begin(), argument_list2->alist.end());
		popStack(pro.right_symbol.size());
		symbol.push_back((string *)argument_list1);
		break;
	}
	case 50: //<A> ::= Epsilon	标记变量作用域
	{
		nowlevel++;
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	default:
	{
		if (!(pro.right_symbol.size() == 1 && pro.right_symbol[0] == "Epsilon"))
			popStack(pro.right_symbol.size());
		symbol.push_back(new string(pro.left_symbol));
		break;
	}
	}
	return;
}

void TempCode::outputError(string str)
{
	flag = false;
	auto oldbuf = cout.rdbuf();
	ofstream out("out_error.txt", ios::app);
	cout.rdbuf(out.rdbuf()); // 重定向cout
	cout << str << endl;
	cout.rdbuf(oldbuf);
}

Func *TempCode::lookUpFunc(string ID)
{
	for (vector<Func>::iterator iter = FuncTable.begin(); iter != FuncTable.end(); iter++)
	{
		if (iter->name == ID)
		{
			return &(*iter);
		}
	}
	return NULL;
}

Var *TempCode::lookUpVar(string ID)
{
	for (vector<Var>::reverse_iterator iter = VarTable.rbegin(); iter != VarTable.rend(); iter++)
	{
		if (iter->name == ID)
		{
			return &(*iter);
		}
	}
	return NULL;
}

bool TempCode::march(list<string> &argument_list, list<DType> &parameter_list)
{
	if (argument_list.size() != parameter_list.size())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void TempCode::emit(string op, string src1, string src2, string des)
{
	code.push_back(Quaternary{op, src1, src2, des});
	return;
}

void TempCode::back_patch(list<int> nextList, int quad)
{
	for (list<int>::iterator iter = nextList.begin(); iter != nextList.end(); iter++)
	{
		code[*iter].des = to_string(quad);
	}
}

void TempCode::printTempCode(const char *Filename)
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

NewTemper::NewTemper()
{
	now = 0;
}

string NewTemper::newTemp()
{
	return string("T") + to_string(now++);
}

Id::Id(const string &sym, const string &name)
{
	this->name = name;
}

Num::Num(const string &sym, const string &number)
{
	this->number = number;
}

FunctionDeclare::FunctionDeclare(const string &sym)
{
	this->name = sym;
}

Parameter::Parameter(const string &sym)
{
	this->name = sym;
}

ParameterList::ParameterList(const string &sym)
{
	this->name = sym;
}

SentenceBlock::SentenceBlock(const string &sym)
{
	this->name = sym;
}

SentenceList::SentenceList(const string &sym)
{
	this->name = sym;
}

Sentence::Sentence(const string &sym)
{
	this->name = sym;
}

WhileSentence::WhileSentence(const string &sym)
{
	this->name = sym;
}

IfSentence::IfSentence(const string &sym)
{
	this->name = sym;
}

Expression::Expression(const string &sym)
{
	this->name = sym;
}

AddExpression::AddExpression(const string &sym)
{
	this->name = sym;
}

Nomial::Nomial(const string &sym)
{
	this->name = sym;
}

Factor::Factor(const string &sym)
{
	this->name = sym;
}

N::N(const string &sym)
{
	this->name = sym;
}

M::M(const string &sym)
{
	this->name = sym;
}

ArgumentList::ArgumentList(const string &sym)
{
	this->name = sym;
}