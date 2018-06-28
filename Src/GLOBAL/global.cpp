#include "global.h"

SensefulStr::SensefulStr(std::string srcstr /*= ""*/)
	:src_str(srcstr)
{
	Parse();
}

void SensefulStr::SetSrcStr(std::string senstr)
{
	src_str = senstr;
	Parse();
}

std::vector<std::string> SensefulStr::GetSensefulStr() const
{
	return sen_str;
}

// 解析命令为有意字串
void SensefulStr::Parse()
{
	int i = 0;
	std::string token;
	while (i < src_str.size())
	{
		// 先判断标识符
		if (src_str[i] == ' ')
		{
			if (!token.empty())
				sen_str.push_back(token);
			token.clear();
			i++;
			continue;
		}
		if (src_str[i] == '\n')
		{
			i++;
			continue;
		}

		else if (src_str[i] == ',' || src_str[i] == '(' || src_str[i] == ')' || src_str[i] == '=')
		{
			if (!token.empty())
				sen_str.push_back(token);
			token.clear();

			sen_str.push_back(std::string() + src_str[i]);
			i++;
			continue;
		}
		else if (src_str[i] == ';')
		{
			if (!token.empty())
				sen_str.push_back(token);
			token.clear();

			sen_str.push_back(";");
			break;
		}

		token += src_str[i++];
	}
}

Column_Type StrConvertToEnumType(std::string str_type)
{
	for (auto &c : str_type)
		tolower(c);

	if (str_type == "int")
	{
		return Column_Type::I;
	}
	if (str_type == "char")
	{
		return Column_Type::C;
	}
	if (str_type == "double")
	{
		return Column_Type::D;
	}

	return Column_Type::I;
}

std::string IdxToDbf(std::string idx_name)
{
	std::string dbf_name(idx_name);

	int i = idx_name.size()-1;
	while (idx_name[i] != '.')
		i--;
	if (i < 0) throw SQLError::FILENAME_CONVERT_ERROR();
	idx_name[i + 1] = 'd';
	idx_name[i + 2] = 'b';
	idx_name[i + 3] = 'f';
	idx_name[i + 4] = '\0';

	return dbf_name;
}

std::string DbfToIdx(std::string dbf_name)
{
	std::string idx_name(dbf_name);

	int i = idx_name.size() - 1;
	while (idx_name[i] != '.')
		i--;
	if (i < 0) throw SQLError::FILENAME_CONVERT_ERROR();
	idx_name[i + 1] = 'i';
	idx_name[i + 2] = 'd';
	idx_name[i + 3] = 'x';
	idx_name[i + 4] = '\0';

	return idx_name;
}

int StrToInt(std::string str)
{
	int x = 0;
	for (int i = 0; i < str.size(); i++)
	{
		x = x * 10 + str[i] - '0';
	}
	return x;
}

std::string StrToLower(std::string str)
{
	for (auto &c : str)
		tolower(c);
	return str;
}
