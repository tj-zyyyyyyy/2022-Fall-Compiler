#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iomanip>
using namespace std;

#define PROTOKEN " ::= "
#define col_width 15
#define START_SYMBOL "<Program>"
const int maxBufferSize = 500;

// 定义分析表元素的结构体
struct elem
{
	enum attribute
	{
		error,
		shiftin,
		reduce,
		acc,
		go
	};
	attribute status = error;
	int next_state = -1;
};

// 定义产生式的结构体
struct Production
{
	string left_symbol;
	vector<string> right_symbol;

	bool operator==(const Production &a) const
	{
		return (left_symbol == a.left_symbol && right_symbol == a.right_symbol);
	}
};

// 定义项目的结构体
struct Item
{
	Production pd;	  // 产生式
	int curr_pos = 0; // 产生式当前读到的位置（就是那个·) 因此Item_set[i].pd.right_symbol[Item_set[i].curr_pos]就是·后面的字符
	string forward;	  // 向前搜索串
	int next_state = -1;

	bool operator==(const Item &a) const
	{
		return (pd == a.pd && curr_pos == a.curr_pos && forward == a.forward);
	}
};

class GETanalyzer
{
public:
	GETanalyzer(string Filename);								   // 根据产生式构造
	int readProductions(Production *productions, string Filename); // 读入产生式
	vector<int> find_production(string l);						   // 找非终结符l的产生式，若存在，返回下标的数组
	vector<string> get_first_single_symbol(string str);			   // 对于文法G的每个文法符号X∈VT∪VN，构造FIRST（X）
	vector<string> get_first(vector<string> beta_a);			   // 对于符号串α= X1X2… Xn，构造 FIRST (α)
	bool compare_Item(vector<Item> I_set, Item I);				   // 比较项目是否已在项目集中
	void get_closure(vector<Item> &Item_set);					   // 获得项目集的闭包
	void print_all_Item_set();									   // 打印所有的项目集
	int compare_Item_set(vector<Item> I);						   // 比较项目集是否已在项目集族中
	void get_all_Item_set();									   // 获取所有的项目集
	void get_hash();											   // 建立哈希
	void write_LRtable();										   // 写LR分析表
	void print_LRtable();										   // 打印LR分析表
	void use();													   // 调用
private:
	Production G[100];								   // 存储产生式
	unordered_set<string> Vn;						   // 存储非终结符
	unordered_set<string> Vt = {"#"};				   // 存储终结符（包括'#'和'Epsilon'）
	vector<string> all_symbol;						   // 存储所有文法符号
	unordered_map<string, int> symbol_to_colNO;		   // 文法符号到列号的哈希表
	vector<vector<Item>> all_Item_set;				   // LR1的项目集族
	elem **LRtable;									   // LR分析表
	unordered_map<string, vector<string>> first_table; // 单个文法符号的first表
	friend class LRanalyzer;
};