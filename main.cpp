#include "LRanalyzer.h"
#include "LRanalyzer.cpp"
#include "Pretreatment.cpp"

int main()
{
	Lexer lex;
	Pretreatment p;
	string name1 = "test.txt", name2 = "productions1.txt";
	string input, line;
	ifstream file;
	file.open(name1, ios::in);
	cout << "当前读入测试数据的文件名为：" << name1 << endl;
	cout << "当前读入文法产生式的文件名为：" << name2 << endl
		 << endl;
	if (!file.is_open())
	{
		cout << "文件打开失败" << endl;
		return -1;
	}
	// 初始化清空error log
	fstream error_log;
	error_log.open("out_error.txt", std::fstream::out | std::ios_base::trunc);
	if (!error_log.is_open())
	{
		cout << "文件打开失败" << endl;
		return -1;
	}

	cout << "词法分析进行中..." << endl;
	auto oldbuf = cout.rdbuf();
	ofstream out1("out_lexer.txt"), out2("out_LRanalyze.txt");
	cout.rdbuf(out1.rdbuf()); // 重定向cout
	int line_no = 1;
	while (getline(file, line, '\n'))
	{
		cout << "第" << line_no << "行的分析结果为：" << endl;
		line_no++;
		line = p.pretreat(line.c_str());
		string line_end = line + "#";
		lex.init(line_end);
		lex.scan();
		input += line;
	}
	cout.rdbuf(oldbuf);
	lex.end_input();
	input += "#";
	cout << "词法分析完毕!" << endl;
	cout << "输出词法分析结果的文件名为：out_lexer.txt" << endl
		 << endl;
	cout << "语法分析进行中..." << endl;
	GETanalyzer Ga(name2);
	Ga.use();
	LRanalyzer lr(Ga);
	// 不能printQueue，会使Queue清Epsilon
	// lex.printQueue();
	cout.rdbuf(out2.rdbuf()); // 重定向cout
	lr.LRanalyze(input, lex.getQueue(), true);
	cout.rdbuf(oldbuf);
	cout << "语法分析完毕!" << endl;
	cout << "输出LR1项目族的文件名为：out_all_Item_set.txt" << endl;
	cout << "输出LR1分析表的文件名为：out_LR_table.txt" << endl;
	cout << "输出LR1分析结果的文件名为：out_LRanalyze.txt" << endl;
	cout << "输出语法树的文件名为：SyntaxTree.png" << endl
		 << endl;
	cout << "语义分析进行中..." << endl;
	if (!lr.flag)
	{
		cout << "语义分析错误！请查看out_error.txt了解错误信息" << endl;
	}
	else
	{
		lr.printTempCode("Out_Code.txt");
		cout << "语义分析完毕！" << endl;
		cout << "输出中间代码的文件名为：out_Code.txt" << endl;
	}
	return 0;
}
