#include "TempCode.h"

void TempCode::emit(string op,string src1,string src2,string des)
{
    code.push_back(Quaternary{ op,src1,src2,des });
}

void TempCode::printTempCode(const char* Filename)
{
    auto oldbuf = cout.rdbuf();
	ofstream out(Filename);
	cout.rdbuf(out.rdbuf());
    for(int i=0;i<code.size();i++)
    {
        cout<<"("<<code[i].op<<","<<code[i].src1<<","<<code[i].src2<<","<<code[i].des<<")\n";
    }
    cout.rdbuf(oldbuf);
	out.close();
}
