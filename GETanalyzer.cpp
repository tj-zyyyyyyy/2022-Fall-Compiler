#include "GETanalyzer.h"

static vector<string> split(const string str, const string separator)
{
	vector<string> strs;
	int start = 0;
	int end = str.find(separator, start);
	if (end == string::npos)
	{
		strs.push_back(str);
		return strs;
	}

	while (end != string::npos)
	{
		string sub = str.substr(start, end - start);
		// 删除首尾Epsilon字符
		sub.erase(0, sub.find_first_not_of("\f\v\r\t\n "));
		sub.erase(sub.find_last_not_of("\f\v\r\t\n ") + 1);
		if (!sub.empty())
			strs.push_back(sub);
		start = end + separator.length();
		end = str.find(separator, start);
	}
	// 加入最后剩余的子串部分
	string sub = str.substr(start);
	// 删除首尾Epsilon字符
	sub.erase(0, sub.find_first_not_of("\f\v\r\t\n "));
	sub.erase(sub.find_last_not_of("\f\v\r\t\n ") + 1);
	if (!sub.empty())
		strs.push_back(sub);

	return strs;
}

// 读入产生式
int GETanalyzer::readProductions(Production *productions, string Filename)
{
	ifstream in;
	in.open(Filename, ios::in);
	if (!in.is_open())
	{
		cerr << "无法打开文法文件" << endl;
		return -1;
	}

	int num_line = 0, num_production = -1; // 没有第0条产生式
	string line;
	while (getline(in, line, '\n'))
	{
		num_line++;
		if (line.empty())
			continue;

		size_t sep = line.find(PROTOKEN);
		string left_production = line.substr(0, sep), right_production = line.substr(sep + strlen(PROTOKEN));
		if (right_production.find(PROTOKEN) != string::npos)
		{
			cout << "第" << num_line << "行的产生式格式有误！每行应有且只有一个\"::=\"符号" << endl;
			return -1;
		}

		// 右边部分以" | "为界分解
		vector<string> production_right_parts = split(right_production, " | ");
		if (production_right_parts.size() == 0)
		{
			cout << "第" << num_line << "行的产生式格式有误！产生式右端没有文法符号" << endl;
			return -1;
		}
		for (auto production_right_it = production_right_parts.begin(); production_right_it != production_right_parts.end(); production_right_it++)
		{
			vector<string> right_symbol_str = split(*production_right_it, " ");
			num_production++;
			productions[num_production].left_symbol = left_production;
			for (auto &right_symbol_it : right_symbol_str)
				productions[num_production].right_symbol.push_back(right_symbol_it);

			// cout << productions[num_production].left_symbol << "->";
			Vn.insert(productions[num_production].left_symbol);
			for (int i = 0; i < productions[num_production].right_symbol.size(); i++)
			{
				if (!(productions[num_production].right_symbol[i][0] == '<' && productions[num_production].right_symbol[i][productions[num_production].right_symbol[i].size() - 1] == '>'))
				{
					Vt.insert(productions[num_production].right_symbol[i]);
				}
				// cout << productions[num_production].right_symbol[i];
			}
			// cout << endl;
		}
	}
	for (auto it = Vt.begin(); it != Vt.end(); it++)
	{
		all_symbol.push_back(*it);
	}
	for (auto it = Vn.begin(); it != Vn.end(); it++)
	{
		all_symbol.push_back(*it);
	}
	in.close();
	return 0;
}

// 根据产生式构造
GETanalyzer::GETanalyzer(string Filename)
{
	readProductions(G, Filename);
	LRtable = new elem *[maxBufferSize];
	for (int i = 0; i < maxBufferSize; i++)
	{
		LRtable[i] = new elem[maxBufferSize];
	}
}

// 找非终结符l的产生式，若存在，返回下标的数组
vector<int> GETanalyzer::find_production(string l)
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
vector<string> GETanalyzer::get_first_single_symbol(string str)
{
	vector<string> ret;
	if (str == "<relop>")
		ret = ret;
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
			while (j < G[index[i]].right_symbol.size()) // 对于第j个位置的符号
			{
				if (Vt.count(G[index[i]].right_symbol[j])) // 是非终结符（包括epsilon），加入到FIRST（X）中，这个产生式就找完了；
				{
					ret.push_back(G[index[i]].right_symbol[j]);
					break;
				}
				else if (Vn.count(G[index[i]].right_symbol[j])) // 是终结符，把 FIRST (Y) - {ε}加入到FIRST（X）中，如果 FIRST (Y)包括{ε}，就继续去找下一个位置；
				{
					vector<string> first_Y = get_first_single_symbol(G[index[i]].right_symbol[j]);
					bool continue_flag = false;
					for (int k = 0; k < first_Y.size(); k++)
					{
						if (first_Y[k] != "Epsilon")
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
			if (j == G[index[i]].right_symbol.size())
				ret.push_back("Epsilon");
		}
	}
	// 去重
	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end()), ret.end());

	first_table.insert(make_pair(str, ret));
	return ret;
}

// 对于符号串α= X1X2… Xn，构造 FIRST (α)
vector<string> GETanalyzer::get_first(vector<string> beta_a)
{
	vector<string> ret;
	int i = 0;
	while (i < beta_a.size()) // 对于第i个文法符号
	{
		bool continue_flag = false;
		vector<string> first_Y = first_table.count(beta_a[i]) ? first_table[beta_a[i]] : get_first_single_symbol(beta_a[i]);
		for (int j = 0; j < first_Y.size(); j++) // 遍历第i个文法符号的first集
		{
			if (first_Y[j] != "Epsilon")
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
		ret.push_back("Epsilon");
	// 去重
	sort(ret.begin(), ret.end());
	ret.erase(unique(ret.begin(), ret.end()), ret.end());
	return ret;
}

// 比较项目是否已在项目集中
bool GETanalyzer::compare_Item(vector<Item> I_set, Item I)
{
	for (int i = 0; i < I_set.size(); i++)
	{
		Item curr_Item = I_set[i];
		if (curr_Item == I)
			return true;
	}
	return false;
}

// 获得项目集的闭包
void GETanalyzer::get_closure(vector<Item> &Item_set)
{
	for (int i = 0; i < Item_set.size(); i++) // 对第i个项目
	{
		if (Item_set[i].curr_pos < Item_set[i].pd.right_symbol.size() && Vn.count(Item_set[i].pd.right_symbol[Item_set[i].curr_pos])) // 当前读到非终结符
		{
			vector<int> index = find_production(Item_set[i].pd.right_symbol[Item_set[i].curr_pos]); // 找到对应的产生式

			for (int j = 0; j < index.size(); j++) // 对该非终结符的第j个产生式
			{
				vector<string> beta_a; // 需要求first的部分
				for (int j = Item_set[i].curr_pos + 1; j < Item_set[i].pd.right_symbol.size(); j++)
				{
					beta_a.push_back(Item_set[i].pd.right_symbol[j]);
				}
				beta_a.push_back(Item_set[i].forward);

				vector<string> all_forward = get_first(beta_a); // 这个时候求出的是整个first集，我们接下来把里面的单个元素分别添加到一个Item中

				for (int k = 0; k < all_forward.size(); k++)
				{
					Item tmp;
					// 这些其实都一样
					tmp.pd.left_symbol = G[index[j]].left_symbol;
					tmp.pd.right_symbol = G[index[j]].right_symbol;
					if (tmp.pd.right_symbol[0] != "Epsilon")
						tmp.curr_pos = 0;
					else
						tmp.curr_pos = 1;
					// first集里面的每一个元素作为一个forward加入不同的Item
					tmp.forward = all_forward[k];
					if (!compare_Item(Item_set, tmp)) // 不重复就加入
						Item_set.push_back(tmp);
				}
			}
		}
	}
}

// 打印所有的项目集
void GETanalyzer::print_all_Item_set()
{
	auto oldbuf = cout.rdbuf();
	ofstream out("out_all_Item_set.txt");
	cout.rdbuf(out.rdbuf()); // 重定向cout 到 out_all_Item_set.txt
	for (int i = 0; i < all_Item_set.size(); i++)
	{
		vector<Item> curr_Item_set = all_Item_set[i];
		cout << "项目集I" << i << ":" << endl;
		for (int j = 0; j < curr_Item_set.size(); j++)
		{
			Item curr_Item = curr_Item_set[j];
			cout << curr_Item.pd.left_symbol << "-->";
			for (int k = 0; k < curr_Item.pd.right_symbol.size(); k++)
			{
				cout << curr_Item.pd.right_symbol[k];
			}
			cout << "," << curr_Item.forward << ",curr_pos=" << curr_Item.curr_pos << ",next_state=" << curr_Item.next_state << endl;
		}
	}
	cout.rdbuf(oldbuf);
}

// 比较项目集是否已在项目集族中,若存在返回下标
int GETanalyzer::compare_Item_set(vector<Item> I)
{
	for (int i = 0; i < all_Item_set.size(); i++)
	{
		vector<Item> curr_Item_set = all_Item_set[i];
		if (!(curr_Item_set.size() == I.size()))
			continue;
		bool flag = false;
		for (int j = 0; j < I.size(); j++)
		{
			if (!(curr_Item_set[j] == I[j]))
			{
				flag = true;
				break;
			}
		}
		if (flag)
			continue;
		return i;
	}
	return -1;
}

// 获取所有的项目集
void GETanalyzer::get_all_Item_set()
{
	// 初始化
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
				if (curr_Item.curr_pos < curr_Item.pd.right_symbol.size() && curr_Item.pd.right_symbol[curr_Item.curr_pos] == all_symbol[j]) // 匹配文法符号X
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
				if (!compare_Item(new_Item_set, new_Item)) // 不存在就加入
					new_Item_set.push_back(new_Item);
			}

			get_closure(new_Item_set); // GO（I,X）
			if (new_Item_set.size())
			{
				int find = compare_Item_set(new_Item_set); // 找到下标
				if (find != -1)							   // 若存在
				{
					for (int k = 0; k < index_j.size(); k++)
					{
						all_Item_set[i][index_j[k]].next_state = find; // 建立next_state=GO（I,X）
					}
				}
				else // 若不存在
				{
					all_Item_set.push_back(new_Item_set);
					for (int k = 0; k < index_j.size(); k++)
					{
						all_Item_set[i][index_j[k]].next_state = all_Item_set.size() - 1;
					}
				}
			}
		}
	}
}

// 建立哈希
void GETanalyzer::get_hash()
{
	int i = 0;
	for (auto it = Vt.begin(); it != Vt.end(); it++)
	{
		symbol_to_colNO.insert(make_pair(*it, i));
		i++;
	}
	for (auto it = Vn.begin(); it != Vn.end(); it++)
	{
		symbol_to_colNO.insert(make_pair(*it, i));
		i++;
	}
}

// 写LR分析表
void GETanalyzer::write_LRtable()
{
	get_hash();
	for (int i = 0; i < all_Item_set.size(); i++)
	{
		vector<Item> curr_Item_set = all_Item_set[i];
		for (int j = 0; j < curr_Item_set.size(); j++)
		{
			Item curr_Item = curr_Item_set[j];
			string curr_symbol = (curr_Item.curr_pos < curr_Item.pd.right_symbol.size()) ? curr_Item.pd.right_symbol[curr_Item.curr_pos] : "";
			string curr_foward = curr_Item.forward;
			if (Vt.count(curr_symbol)) // 若项目[A→·a, b]属于Ik且GO(Ik, a)＝Ij， a为终结符，则置ACTION[k, a]为“ “sj”
			{
				LRtable[i][symbol_to_colNO[curr_symbol]].status = elem::shiftin;
				LRtable[i][symbol_to_colNO[curr_symbol]].next_state = curr_Item.next_state;
			}
			else if (curr_Item.pd.left_symbol == START_SYMBOL && (curr_Item.curr_pos == curr_Item.pd.right_symbol.size())) // 若项目[S→S·, #]属于Ik，则置ACTION[k, #]为 “acc”。
			{
				LRtable[i][symbol_to_colNO["#"]].status = elem::acc;
			}
			else if (curr_Item.curr_pos == curr_Item.pd.right_symbol.size()) //. 若项目[A→·，a]属于Ik，则置ACTION[k, a]为 “rj”；其中假定A→为文法G的第j个产生式。
			{
				LRtable[i][symbol_to_colNO[curr_foward]].status = elem::reduce;
				for (int k = 0; G[k].left_symbol != ""; k++) // 查找第几条产生式
				{
					if (G[k] == curr_Item.pd)
					{
						LRtable[i][symbol_to_colNO[curr_foward]].next_state = k;
						break;
					}
				}
			}

			if (Vn.count(curr_symbol)) // 若GO(Ik，A)＝Ij，则置GOTO[k, A]=j。
			{
				int a = symbol_to_colNO[curr_symbol];
				LRtable[i][symbol_to_colNO[curr_symbol]].status = elem::go;
				LRtable[i][symbol_to_colNO[curr_symbol]].next_state = curr_Item.next_state;
			}
		}
	}
}

// 打印LR分析表
void GETanalyzer::print_LRtable()
{
	auto oldbuf = cout.rdbuf();
	ofstream out("out_LRtable.txt");
	cout.rdbuf(out.rdbuf()); // 重定向cout 到 out_LRtable.txt
	cout << setiosflags(ios::left) << setw(col_width) << "	";
	for (int j = 0; j < all_symbol.size(); j++)
	{
		cout << setw(col_width) << all_symbol[j] << "	";
	}
	cout << endl;
	for (int i = 0; i < all_Item_set.size(); i++)
	{
		string str = "I" + to_string(i);
		cout << setw(col_width) << str << "	";
		for (int j = 0; j < all_symbol.size(); j++)
		{
			if (LRtable[i][j].status == elem::error)
			{
				cout << setw(col_width) << ""
					 << "	";
			}
			else if (LRtable[i][j].status == elem::shiftin)
			{
				string str = "s" + to_string(LRtable[i][j].next_state);
				cout << setw(col_width) << str << "	";
			}
			else if (LRtable[i][j].status == elem::reduce)
			{
				string str = "r" + to_string(LRtable[i][j].next_state);
				cout << setw(col_width) << str << "	";
			}
			else if (LRtable[i][j].status == elem::acc)
			{
				cout << setw(col_width) << "acc"
					 << "	";
			}
			else if (LRtable[i][j].status == elem::go)
			{
				cout << setw(col_width) << LRtable[i][j].next_state << "	";
			}
		}
		cout << endl;
	}
	cout.rdbuf(oldbuf);
}

// 调用
void GETanalyzer::use()
{
	get_all_Item_set();
	print_all_Item_set();
	write_LRtable();
	print_LRtable();
}