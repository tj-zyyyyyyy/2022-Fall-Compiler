#include "LRanalyzer.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <io.h>
#include <stack>
#define PROTOKEN " ::= "

LRanalyzer::LRanalyzer(GETanalyzer& Ga)
{
	LRtable = Ga.LRtable;
	symbolmapping = Ga.symbol_to_colNO;
	productions = Ga.G;
}

int LRanalyzer::LRanalyze(string line, queue<pair<string, string>> lex_res, bool drawtree)
{
	auto oldbuf = cout.rdbuf();
	ofstream outt("out_LRanalyze.txt");
	cout.rdbuf(outt.rdbuf());//重定向cout 到 out_LRtable.txt
	cout << setiosflags(ios::left) << setw(5) << "序号" << setw(40) << "状态" << setw(40) << "符号" << setw(40) << "输入串" << endl;
	ofstream out(".\\Graphviz\\SyntaxTree.dot", ios::out);
	if (drawtree)
	{
		if (_access(".\\Graphviz\\dot.exe", 0) != 0)
		{
			cerr << "文件缺失！无法生成语法树文件" << endl;
			drawtree = false;
			out.close();
		}
		else if (!out.is_open())
		{
			cerr << "无法生成语法树文件！" << endl;
			drawtree = false;
		}
		else
			out << "digraph SyntaxTree {\n";
	}

	StatusSymbolStack.clear();
	StatusSymbolStack.push_back(make_pair(0, make_pair("#", "#")));

	int idx_node = 1;
	stack<int> idxStack;
	while (!lex_res.empty())
	{
		printStack(line);
		string symbol = lex_res.front().first, content;
		if (symbol == "id")
			content = ((info*)strtoull(lex_res.front().second.c_str(), NULL, 0))->name;
		else if (symbol == "num")
			content = lex_res.front().second;
		else
			content = symbol;

		int status = StatusSymbolStack.back().first;
		int idx_sym = symbolmapping[symbol];    //是拿id(目前)还是name作为终结符
		if (idx_sym == -1)
		{
			cerr << "语法分析错误！\'" << symbol << "\'字符在文法中不存在" << endl;
			return -1;
		}
		elem cur = LRtable[status][idx_sym];
		if (cur.status == elem::error)
		{
			cerr << "语法分析错误！该句不符合语法规则" << endl;
			return -1;
		}
		else if (cur.status == elem::shiftin)
		{
			StatusSymbolStack.push_back({ cur.next_state,{symbol,content} });
			idxStack.push(idx_node);
			if (drawtree)
				out << "n" << idx_node++ << "[label=\"" << content << "\",color=red];" << endl;
			/*if (line.find(content) != 0)
			{
				cerr << "语法分析错误！内部错误1" << endl;
				return -1;
			}*/
			line.erase(0, content.size());
			line.erase(0, line.find_first_not_of("\f\v\r\t\n "));
			lex_res.pop();
		}
		else if (cur.status == elem::reduce)
		{
			int idx_pro = cur.next_state;
			Production pro = productions[idx_pro];

			//右边
			vector<int> idx_sub;
			if (pro.right_symbol.front() != "Epsilon")
			{
				int count = pro.right_symbol.size();
				while (count--)
				{
					StatusSymbolStack.pop_back();
					if (drawtree)
					{
						idx_sub.push_back(idxStack.top());
						idxStack.pop();
					}
				}
			}

			//左边
			int idx_nt = symbolmapping[pro.left_symbol];
			if (idx_nt == 0)
			{
				cerr << "语法分析错误！内部错误2" << endl;
				return -1;
			}

			int next_state = LRtable[StatusSymbolStack.back().first][idx_nt].next_state;
			StatusSymbolStack.push_back({ next_state,{pro.left_symbol,pro.left_symbol} });
			if (drawtree)
			{
				idxStack.push(idx_node);
				out << "n" << idx_node++ << "[label=\"" << pro.left_symbol << "\"];\n";
				if (idx_sub.size() != 0)
				{
					for (auto t = idx_sub.begin(); t != idx_sub.end(); t++)
						out << "n" << idx_node - 1 << " -> " << "n" << *t << ";\n";
				}
				else	//Epsilon串
				{
					out << "e" << idx_node << "[label=\"Epsilon\"];\n";
					out << "n" << idx_node - 1 << " -> " << "e" << idx_node << ";\n";
				}
			}
		}
		else if (cur.status == elem::acc)
		{
			cout << "语法分析成功！" << endl;
			if (drawtree)
			{
				out << "}\n";
				out.close();
				system(".\\Graphviz\\dot.exe -Tpng .\\Graphviz\\SyntaxTree.dot -o SyntaxTree.png");
				cout << "生成语法树图片成功！请见当前目录下SyntaxTree.png" << endl;
			}
			return 0;
		}

		/*if (lex_res.size() == 1 && lex_res.front().first != "#")
		{
			cerr << "语法分析错误！输入字符串格式不匹配！" << endl;
			return -1;
		}*/
	}
	cout << "语法分析完毕！未见#" << endl;
	cout.rdbuf(oldbuf);
	out.close();
	return 0;
}

void LRanalyzer::printStack(string line)
{
	static int idx_print = 0;
	string str_status, str_symbol;
	for (size_t i = 0; i < StatusSymbolStack.size(); i++)
	{
		str_status += to_string(StatusSymbolStack[i].first) + " ";
		str_symbol += StatusSymbolStack[i].second.second + " ";
	}
	idx_print++;
	cout << setiosflags(ios::left) << setw(5) << idx_print << setw(COLWIDTH) << str_status << setw(COLWIDTH) << str_symbol << setw(COLWIDTH) << line << endl;
	return;
}
