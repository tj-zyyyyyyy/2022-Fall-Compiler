#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <queue>
#include <fstream>
using namespace std;

// 符号表项的结构体
struct info
{
	int No;
	string name;
	string type;
	unsigned long long addr;
	int size;
};

class Lexer
{
private:
	string::iterator start_p;  // 起点指示器
	string::iterator search_p; // 搜索指示器
	int BufferSize;			   // 输入缓冲区大小
	char ch;				   // 当前扫描的字符 全局
	string token;			   // 当前构成单词符号的字符串
	int syn;				   // 当前扫描字符的种别码

	const int maxSymbolSize = 256;			// 符号表的大小
	info *Symbol = new info[maxSymbolSize]; // 符号表
	queue<pair<string, string>> res;		// 结果队列
public:
	void init(string &buffer);
	void GetChar();							// 取下一个字符，搜索指示器后移一位
	void GetBC();							// 取下一个非Epsilon字符
	void Concat();							// 将字符连接成字符串
	bool IsLetter();						// 判断是不是字母
	bool IsDigit();							// 判断是不是数字
	int Reserve();							// 判断是否为关键词，若是返回关键词表下标，否则返回0
	void Retract();							// 搜索指示器回退一格，字符变Epsilon
	info *InsertId();						// 插入符号表，返回符号表指针
	double InsertConst();					// 插入数值
	void StateTrans();						// 状态转移图，一次识别一个单词，输出syn和token
	void scan();							// 扫描整个缓冲区
	void printSymbol();						// 输出符号表
	void printQueue();						// 打印队列（调试用）
	queue<pair<string, string>> getQueue(); // 获取队列
	void end_input();						// 文件全部读入后再把#入队
};