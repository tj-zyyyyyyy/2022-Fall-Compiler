#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
using namespace std;

class Pretreatment
{
public:
	/* 预处理子程序 */
	string pretreat(const char *line)
	{
		char *lines = (char *)line;
		string ret = "";

		while (*lines != '\0' && lines != NULL)
		{
			char *loc, *noloc = strstr(lines, "//"), *nloc, *qloc1 = strchr(lines, '\"'), *qloc2 = NULL; // 注释和引号的位置
			if (multinote)
			{
				nrloc = strstr(lines, "*/");
				if (nrloc != NULL)
					multinote = false;
				while (lines != nrloc + 2 && *lines != '\0')
					lines++;
			}
			/*
			不能改成else
			*/
			if (!multinote)
			{
				nlloc = strstr(lines, "/*");
				if (noloc != NULL && nlloc != NULL)
					nloc = noloc < nlloc ? noloc : nlloc;
				else if (noloc != NULL)
					nloc = noloc;
				else if (nlloc != NULL)
					nloc = nlloc;
				else
					nloc = NULL;
				if (qloc1 != NULL) // 处理\"
				{
					if (*(qloc1 - 1) == '\\')
						qloc1 = NULL;
					else
						qloc2 = strchr(qloc1 + 1, '\"');
				}
				else
					qloc2 = NULL;
				if (qloc1 != NULL && nloc != NULL)
					loc = qloc1 < nloc ? qloc1 : nloc;
				else
					loc = qloc1 > nloc ? qloc1 : nloc; // 取非Epsilon

				// 去除cout了
				if (loc == qloc1)
				{
					while (lines != qloc2 + 1 && *lines != '\0')
					{
						ret += *lines;
						lines++;
					}
				}
				else if (loc == nloc)
				{
					while (lines != loc && *lines != '\0')
					{
						ret += *lines;
						lines++;
					}
					if (nloc == noloc)
					{
						ret += "//";
						while (*lines != '\0')
							lines++;
					}
					else if (nloc == nlloc)
					{
						nrloc = strstr(lines, "*/");
						ret += "/**/";
						if (nrloc == NULL)
						{
							multinote = true;
							while (*lines != '\0')
								lines++;
						}
						else
						{
							while (lines != nrloc + 2)
								lines++;
						}
					}
				}
			}
		}
		cout << ret << endl;
		return ret;
	}

	void reset()
	{
		multinote = false;
	}

private:
	bool multinote = false;
	char *nlloc, *nrloc;
};
