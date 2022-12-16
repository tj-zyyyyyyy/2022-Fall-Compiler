#include "Lexer.h"

const int Keywords_num = 63; // 关键词数量
string Keywords[Keywords_num + 1] = {
	"",
	"int",
	"void",
	"if",
	"else",
	"while",
	"return",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"char",
	"float",
	"double",
	"short",
	"long",
	"signed",
	"unsigned",
	"struct",
	"union",
	"enum",
	"typedef",
	"sizeof",
	"auto",
	"static",
	"register",
	"extern",
	"const",
	"volatile",
	"continue",
	"break",
	"goto",
	"switch",
	"case",
	"default",
	"for",
	"do",
	"include",
	"public",
	"private",
	"using",
	"namespace",
	"std",
	"class",
	"string",
	"cout",
	"bool",
}; // 关键词表，种别码从28后开始逐个增加

void Lexer::init(string &buffer)
{
	start_p = buffer.begin();
	search_p = start_p;
	BufferSize = (int)buffer.length();
	ch = ' ';
	token = "";
	syn = 0;
}

// 取下一个字符，搜索指示器后移一位
void Lexer::GetChar()
{
	if (search_p - start_p < BufferSize)
	{
		ch = *search_p;
		search_p++;
	}
}

// 取下一个非Epsilon字符
void Lexer::GetBC()
{
	while (ch == ' ' || ch == '	')
	{
		GetChar();
	}
}

// 将字符连接成字符串
void Lexer::Concat()
{
	token += ch;
}

// 判断是不是字母
bool Lexer::IsLetter()
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;
	else
		return false;
}

// 判断是不是数字
bool Lexer::IsDigit()
{
	if (ch >= '0' && ch <= '9')
		return true;
	else
		return false;
}

// 判断是否为关键词，若是返回关键词表下标，否则返回0
int Lexer::Reserve()
{
	for (int i = 1; i <= Keywords_num; i++)
	{
		if (token == Keywords[i])
			return i;
	}
	return 0;
}

// 搜索指示器回退一格，字符变Epsilon
void Lexer::Retract()
{
	if (search_p >= start_p)
		search_p--;
	ch = ' ';
}

// 插入符号表，返回符号表指针
info *Lexer::InsertId()
{
	int i;
	for (i = 0; Symbol[i].name != "" && i < maxSymbolSize; i++)
	{
		if (token == Symbol[i].name)
			return Symbol + i;
	}

	if (i >= maxSymbolSize)
		return NULL;

	Symbol[i].No = i;
	Symbol[i].name = token;
	Symbol[i].addr = (unsigned long long)&Symbol[i];
	return Symbol + i;
}

// 插入数值
double Lexer::InsertConst()
{
	return stod(token);
}

// 状态转移图，一次识别一个单词，输出syn和token
void Lexer::StateTrans()
{
	token = "";
	GetChar();
	GetBC();
	if (IsLetter())
	{
		while (IsLetter() || IsDigit())
		{
			Concat();
			GetChar();
		}
		Retract();
		int is_keywords = Reserve();
		if (is_keywords == 0)
		{
			info *p = InsertId();
			syn = 7;
			cout << "(" << syn << "," << p->addr << ")" << endl;
			char tmp[64];
			sprintf_s(tmp, 64, "%llu", p->addr);
			res.push(make_pair((string) "id", tmp));
			return;
		}
		else
		{
			syn = is_keywords;
			cout << "(" << syn << ","
				 << "-"
				 << ")" << endl;
			res.push(make_pair((string)token, "-"));
			return;
		}
	}
	else if (IsDigit())
	{
		bool flag = 0;
		while (IsDigit() || (ch == '.' && flag == 0)) //////////////////////////////////////////////////////////实常数
		{
			if (ch == '.')
				flag = 1;
			Concat();
			GetChar();
		}
		Retract();
		double value = InsertConst();
		syn = 8;
		cout << "(" << syn << "," << value << ")" << endl;
		res.push(make_pair((string) "num", to_string(value)));
		return;
	}
	else if (ch == '=')
	{
		Concat();
		GetChar();
		if (ch == '=')
		{
			Concat();
			syn = 14;
			cout << "(" << syn << ","
				 << "-"
				 << ")" << endl;
			res.push(make_pair((string)token, "-"));
			return;
		}
		Retract();
		syn = 9;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '+')
	{
		Concat();
		GetChar();
		if (ch == '=') //////////////////////////////////////////////////////////符号的种别码统一100往上
		{
			Concat();
			syn = 100;
			cout << "(" << syn << ","
				 << "-"
				 << ")" << endl;
			res.push(make_pair((string)token, "-"));
			return;
		}
		Retract();
		syn = 10;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '-')
	{
		Concat();
		syn = 11;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '*')
	{
		Concat();
		syn = 12;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '/')
	{
		Concat();
		GetChar();
		if (ch == '*')
		{
			Concat();
			GetChar();
			if (ch == '*')
			{
				Concat();
				GetChar();
				if (ch == '/')
				{
					Concat();
					syn = 22;
					cout << "(" << syn << ","
						 << "-"
						 << ")" << endl;
					// res.push(make_pair((string)"note", "-"));		note不入队！！！！！
					return;
				}
				Retract();
			}
			Retract();
		}
		Retract();

		GetChar();
		if (ch == '/')
		{
			Concat();
			syn = 23;
			cout << "(" << syn << ","
				 << "-"
				 << ")" << endl;
			// res.push(make_pair((string)"note", "-"));
			return;
		}
		Retract();

		syn = 13;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '>')
	{
		Concat();
		GetChar();
		if (ch == '=')
		{
			Concat();
			syn = 16;
			cout << "(" << syn << ","
				 << "-"
				 << ")" << endl;
			res.push(make_pair((string)token, "-"));
			return;
		}
		Retract();
		syn = 15;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '<')
	{
		Concat();
		GetChar();
		if (ch == '=')
		{
			Concat();
			syn = 18;
			cout << "(" << syn << ","
				 << "-"
				 << ")" << endl;
			res.push(make_pair((string)token, "-"));
			return;
		}
		Retract();
		syn = 17;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '!')
	{
		Concat();
		GetChar();
		if (ch == '=')
		{
			Concat();
			syn = 19;
			cout << "(" << syn << ","
				 << "-"
				 << ")" << endl;
			res.push(make_pair((string)token, "-"));
			return;
		}
		Retract();
	}
	else if (ch == ';')
	{
		Concat();
		syn = 20;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == ',')
	{
		Concat();
		syn = 21;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '(')
	{
		Concat();
		syn = 24;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == ')')
	{
		Concat();
		syn = 25;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '{')
	{
		Concat();
		syn = 26;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '}')
	{
		Concat();
		syn = 27;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '#')
	{
		syn = 0;
		cout << "分析完成！" << endl;
		return;
	}
	else if (ch == ':') //////////////////////////////////////////////////////////不能合并的多的加在后面这里了
	{
		Concat();
		syn = 101;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	else if (ch == '.')
	{
		Concat();
		syn = 102;
		cout << "(" << syn << ","
			 << "-"
			 << ")" << endl;
		res.push(make_pair((string)token, "-"));
		return;
	}
	syn = -1;
	return;
}

// 扫描整个缓冲区
void Lexer::scan()
{
	do
	{
		StateTrans();
		if (syn == -1)
		{
			cout << "包含非法字符" << endl;
			break;
		}
	} while (syn != 0);
}

// 输出符号表
void Lexer::printSymbol()
{
	int i;
	for (i = 0; Symbol[i].name != "" && i < maxSymbolSize; i++)
	{
		cout << setiosflags(ios::left) << setw(8) << dec << Symbol[i].No << setw(16) << Symbol[i].name << setw(16) << hex << Symbol[i].addr << endl;
	}
}

// 打印队列（调试用）
void Lexer::printQueue()
{
	cout << "The Queue is" << endl;
	while (!res.empty())
	{
		cout << "(" << res.front().first << "," << res.front().second << ")" << endl;
		// 测试符号表指针
		if (res.front().first == "id")
		{
			info *p = (info *)strtoull(res.front().second.c_str(), NULL, 0);
			// cout << p->name<<endl;
		}
		res.pop();
	}
}

// 获取队列
queue<pair<string, string>> Lexer::getQueue()
{
	return res;
}

void Lexer::end_input()
{
	res.push(make_pair("#", "-"));
}