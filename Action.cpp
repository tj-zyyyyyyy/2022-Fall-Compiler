#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

struct elem
{
	enum Action
	{
		error,
		shiftin,
		reduce,
		acc
	};
	int status;
	int next_state;
};

struct Production
{
	string left_symbol;
	vector<string> right_symbol;
};

Production G[4] = {{"S'", {"S"}},
				   {"S", {"B", "B"}},
				   {"B", {"a", "B"}},
				   {"B", {"b"}}};
unordered_map<string, int> Vn = {{(string) "S'", 0}, {(string) "S", 1}, {(string) "B", 2}};
unordered_map<string, int> Vt = {{(string) "a", 0}, {(string) "b", 1}, {(string) "epsilon", 2}};
vector<string> all_symbol = {"S'", "S", "B", "a", "b"};

struct Item
{
	Production pd;	  // 产生式
	int curr_pos = 0; // 产生式当前读到的位置（就是那个·) 因此Item_set[i].pd.right_symbol[Item_set[i].curr_pos]就是·后面的字符
	string forward;	  // 向前搜索串
};

vector<vector<Item>> all_Item_set; // LR1的项目集族

vector<int> find_production(string l)
{
	vector<int> ret;
	for (int i = 0; G[i].left_symbol != ""; i++)
	{
		if (G[i].left_symbol == l)
			ret.push_back(i);
	}
	return ret;
}

// 对于文法G的每个文法符号X∈VT∪VN，构造FIRST（X）
vector<string> get_first_single_symbol(string str)
{
	vector<string> ret;
	if (Vt.count(str))
	{
		ret.push_back(str);
		return ret;
	}
	else if (Vn.count(str))
	{
		vector<int> index = find_production(str);
		for (int i = 0; i < index.size(); i++) // 对于第i个产生式
		{
			int j = 0;
			while (j < G[i].right_symbol.size()) // 对于第j个位置的符号
			{
				if (Vt.count(G[i].right_symbol[j])) // 是非终结符（包括epsilon），加入到FIRST（X）中，这个产生式就找完了；
				{
					ret.push_back(G[i].right_symbol[j]);
					break;
				}
				else if (Vn.count(G[i].right_symbol[j])) // 是终结符，把 FIRST (Y) - {ε}加入到FIRST（X）中，如果 FIRST (Y)包括{ε}，就继续去找下一个位置；
				{
					vector<string> first_Y = get_first_single_symbol(G[i].right_symbol[j]);
					bool continue_flag = false;
					for (int k = 0; k < first_Y.size(); k++)
					{
						if (first_Y[k] != "epsilon")
							ret.push_back(first_Y[k]);
						else
							continue_flag = true;
					}
					if (!continue_flag)
						break;
					else
						j++;
				}
			}
			// 如果都找完了，都包括{ε}，则把epsilon加入
			if (j == G[i].right_symbol.size())
				ret.push_back("epsilon");
		}
	}
	return ret;
}

// 对于符号串α= X1X2… Xn，构造 FIRST (α)
vector<string> get_first(vector<string> beta_a)
{
	vector<string> ret;
	int i = 0;
	while (i < beta_a.size()) // 对于第i个文法符号
	{
		bool continue_flag = false;
		vector<string> first_Y = get_first_single_symbol(beta_a[i]);
		for (int j = 0; j < first_Y.size(); j++) // 遍历第i个文法符号的first集
		{
			if (first_Y[j] != "epsilon")
				ret.push_back(first_Y[j]);
			else
				continue_flag = true;
		}
		if (!continue_flag)
			break;
		else
			i++;
	}
	// 如果都找完了，都包括{ε}，则把epsilon加入
	if (i == beta_a.size())
		ret.push_back("epsilon");
	return ret;
}

void get_closure(vector<Item> &Item_set)
{
	for (int i = 0; i < Item_set.size(); i++) // 对第i个项目
	{
		if (Vn.count(Item_set[i].pd.right_symbol[Item_set[i].curr_pos])) // 当前读到非终结符
		{
			vector<int> index = find_production(Item_set[i].pd.right_symbol[Item_set[i].curr_pos]); // 找到对应的产生式

			for (int j = 0; j < index.size(); j++) // 对该非终结符的第j个产生式
			{
				vector<string> beta_a; // 需要求first的部分
				for (int j = Item_set[i].curr_pos + 1; j < Item_set.size(); j++)
				{
					beta_a.push_back(Item_set[i].pd.right_symbol[j]);
				}
				beta_a.push_back(Item_set[i].forward);

				vector<string> all_forward = get_first(beta_a); // 这个时候求出的是整个first集，我们接下来把里面的单个元素分别添加到一个Item中

				for (int k = 0; k < all_forward.size(); k++)
				{
					Item tmp;
					// 这些其实都一样
					tmp.pd.left_symbol = G[j].left_symbol;
					tmp.pd.right_symbol = G[j].right_symbol;
					tmp.curr_pos = 0;
					// first集里面的每一个元素作为一个forward加入不同的Item
					tmp.forward = all_forward[j];
					Item_set.push_back(tmp);
				}
			}
		}
	}
}

void get_all_Item_set()
{
	// C:={CLOSURE({[S→·S，#]})};
	Item start;
	start.pd.left_symbol = G[0].left_symbol;
	start.pd.right_symbol = G[0].right_symbol;
	start.curr_pos = 0;
	start.forward = "#";
	vector<Item> st;
	st.push_back(start);
	get_closure(st);
	all_Item_set.push_back(st);

	for (int i = 0; i < all_Item_set.size(); i++)
	{
		vector<Item> curr_Item_set = all_Item_set[i];
		for (int j = 0; j < all_symbol.size(); j++)
		{
			vector<int> index_j; // 第一遍循环记录所有匹配文法符号all_symbol[j]的下标
			for (int k = 0; k < curr_Item_set.size(); k++)
			{
				Item curr_Item = curr_Item_set[k];
				if (curr_Item.pd.right_symbol[curr_Item.curr_pos] == all_symbol[j]) // 匹配文法符号X
				{
					index_j.push_back(k);
				}
			}

			vector<Item> new_Item_set; // 对应项目集J=(I,X)
			for (int k = 0; k < index_j.size(); k++)
			{
				Item curr_Item = curr_Item_set[index_j[k]];
				Item new_Item;
				new_Item.pd.left_symbol = curr_Item.pd.left_symbol;
				new_Item.pd.right_symbol = curr_Item.pd.right_symbol;
				new_Item.curr_pos = curr_Item.curr_pos + 1;
				new_Item.forward = curr_Item.forward;
				new_Item_set.push_back(new_Item);
			}

			get_closure(new_Item_set); // GO（I,X）
			all_Item_set.push_back(new_Item_set);
		}
	}
}