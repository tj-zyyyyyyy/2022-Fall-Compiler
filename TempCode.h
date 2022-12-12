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

struct Quaternary {
	string op;
	string src1;
	string src2;
	string des;
};

class TempCode
{
private:
	vector<Quaternary> code;
public:
    void emit(string op,string src1,string src2,string des);
    void printTempCode(const char* Filename);
};