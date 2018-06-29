#include "interpreter.h"



std::string CreateDbInfo(std::vector<std::string> sen_str)
{
	if ( (sen_str.size()<4) 
		|| (StrToLower(sen_str[0]) != "create") 
		|| (StrToLower(sen_str[1]) != "database") 
		|| (StrToLower(sen_str[3]) != ";")
		)
		throw SQLError::CMD_FORMAT_ERROR();
	return sen_str[2];
}

std::string DeleteDbInfo(std::vector<std::string> sen_str)
{
	if ((sen_str.size() < 4)
		|| (StrToLower(sen_str[0]) != "drop")
		|| (StrToLower(sen_str[1]) != "database")
		|| (StrToLower(sen_str[3]) != ";")
		)
		throw SQLError::CMD_FORMAT_ERROR();
	return sen_str[2];
}

std::string UseDbInfo(std::vector<std::string> sen_str)
{
	if ((sen_str.size() < 4)
		|| (StrToLower(sen_str[0]) != "use")
		|| (StrToLower(sen_str[1]) != "database")
		|| (StrToLower(sen_str[3]) != ";")
		)
		throw SQLError::CMD_FORMAT_ERROR();
	return sen_str[2];
}

std::string ShowDbInfo(std::vector<std::string> sen_str)
{
	if ((sen_str.size() < 3)
		|| (StrToLower(sen_str[0]) != "show")
		|| (StrToLower(sen_str[1]) != "databases")
		|| (StrToLower(sen_str[2]) != ";")
		)
		throw SQLError::CMD_FORMAT_ERROR();
	return std::string();
}


bool CreateShowTableInfo(std::vector<std::string> sen_str)
{
	
	if (!GetCp().GetIsInSpeDb() || sen_str.size() < 3 || sen_str[2] != ";")
	{
		return false;
	}	
	else
	{
		return true;
	}
}

TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str)
{
	TB_Create_Info tb_create_info;

	assert(sen_str.size()>=3);
	tb_create_info.table_name = sen_str[2];

	if (sen_str[3] != "(")
		throw SQLError::CMD_FORMAT_ERROR();

	int i = 4;
	while (i < sen_str.size())
	{
		if (sen_str[i] == ";")
			break;
		if(i+1>=sen_str.size())
			throw SQLError::CMD_FORMAT_ERROR();

		TB_Create_Info::ColumnInfo colmu_info;
		colmu_info.name = sen_str[i];
		colmu_info.type = StrConvertToEnumType(sen_str[i + 1]);
		colmu_info.isPrimary = false;

		if (sen_str[i + 2] == ",")
		{
			tb_create_info.columns_info.push_back(colmu_info);
			i += 3;
			continue;
		}
		else if (sen_str[i + 2] == "primary")
		{
			colmu_info.isPrimary = true;
			tb_create_info.columns_info.push_back(colmu_info);
			i += 4;
			continue;
		}
		else if (sen_str[i + 2] == "(")
		{
			colmu_info.length = StrToInt(sen_str[i + 3]);
			if (sen_str[i + 4] != ")")
				throw SQLError::CMD_FORMAT_ERROR();
			if (sen_str[i + 5] == "primary")
			{
				colmu_info.isPrimary = true;
				tb_create_info.columns_info.push_back(colmu_info);
				i += 7;
				continue;
			}
			else
			{
				tb_create_info.columns_info.push_back(colmu_info);
				i += 6;
				continue;
			}
		}
	}

	return tb_create_info;
}

TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str)
{
	TB_Insert_Info tb_insert_info;


	if (StrToLower(sen_str[0]) != "insert" || StrToLower(sen_str[1]) != "into")
		throw SQLError::CMD_FORMAT_ERROR();

	// ��ȡ����
	tb_insert_info.table_name = sen_str[2];
	
	// �����﷨���
	int col_name_left_bracket = 3;  // �ֶ������б�������
	int col_name_right_bracket = 0;  // �ֶ������б�������
	int col_value_left_bracket = 0;  // �ֶ�ֵ�б�������
	int col_value_right_bracket = sen_str.size() - 2;  // �ֶ�ֵ�б�������

	if (sen_str[sen_str.size() - 1] != ";")
		throw SQLError::CMD_FORMAT_ERROR("Lack ';' at the end of command");

	for (int j = 3; j <sen_str.size()-1; j++)
	{
		if (sen_str[j] == ")")
		{
			col_name_right_bracket = j;
			col_value_left_bracket = j + 2;
			break;
		}
	}
	if(sen_str[col_value_left_bracket]!="("|| StrToLower(sen_str[col_name_right_bracket+1])!="values")
		throw SQLError::CMD_FORMAT_ERROR("key's count is not match value's count or values that not value");

	// ����Ƿ����ַ���������ո��ַ���������Ϊ���������ִ�
	for (auto iter = sen_str.begin() + col_name_left_bracket+1; *iter != ")";)
	{
		//std::cout << *iter << std::endl;
		//std::cout << *(iter+1) << std::endl;
		if (*(iter + 1) == ",")
		{
			iter+=2;
		}
		else if (*(iter + 1) == ")")
		{
			iter++;
		}
		else
		{
			*iter += (" "+*(iter + 1));
			sen_str.erase(iter + 1);
		}
	}
	col_value_left_bracket = 0;
	auto iter = sen_str.begin() + 4;
	while (*iter != "(")iter++;
	if(iter==sen_str.end())
		throw SQLError::CMD_FORMAT_ERROR("");
	for (iter++; *iter != ")";)
	{
		//std::cout << *iter << std::endl;
		//std::cout << *(iter + 1) << std::endl;
		if (*(iter + 1) == ",")
		{
			iter += 2;
		}
		else if (*(iter + 1) == ")")
		{
			iter++;
		}
		else
		{
			*iter += (" " + *(iter + 1));
			sen_str.erase(iter + 1);
		}
	}

	for (int j = 3; j < sen_str.size() - 1; j++)
	{
		if (sen_str[j] == ")")
		{
			col_name_right_bracket = j;
			col_value_left_bracket = j + 2;
			break;
		}
	}
	col_value_right_bracket = sen_str.size() - 2;
	for (int p = col_name_left_bracket + 1, q = col_value_left_bracket + 1; q <= col_value_right_bracket - 1; p+=2, q+=2)
	{
		tb_insert_info.insert_info.push_back({ sen_str[p], sen_str[q] });
	}
#ifndef NDEBUG
	std::cout << "����ļ�¼��ֵ��" << std::endl;
	for (auto e : tb_insert_info.insert_info)
		std::cout << e.column_name << " " << e.column_value << std::endl;
#endif
	return tb_insert_info;
}


CmdType GetOpType(std::vector<std::string> sen_str)
{
	for (auto&e : sen_str)
		StrToLower(e);

	if (sen_str[0] == "create"&&sen_str[1] == "table")
	{
		return CmdType::TABLE_CREATE;
	}
		

	if (sen_str[0] == "create"&&sen_str[1] == "database")
	{
		return CmdType::DB_CREATE;
	}
		

	if (sen_str[0] == "drop"&&sen_str[1] == "table")
	{
		return CmdType::TABLE_DROP;
	}
		

	if (sen_str[0] == "drop"&&sen_str[1] == "database")
	{
		return CmdType::DB_DROP;
	}

	if (sen_str[0] == "show"&&sen_str[1] == "tables")
	{
		return CmdType::TABLE_SHOW;
	}

	if (sen_str[0] == "show"&&sen_str[1] == "databases")
	{
		return CmdType::DB_SHOW;
	}

	if (sen_str[0] == "use")
	{
		return CmdType::DB_USE;
	}

	if (sen_str[0] == "select")
	{
		return CmdType::TABLE_SELECT;
	}

	if (sen_str[0] == "insert")
	{
		return CmdType::TABLE_INSERT;
	}

	if (sen_str[0] == "update")
	{
		return CmdType::TABLE_UPDATE;
	}

	if (sen_str[0] == "delete")
	{
		return CmdType::TABLE_DELETE;
	}

	throw SQLError::CMD_FORMAT_ERROR();

}

void Interpreter(std::vector<std::string> sen_str, CmdType cmd_type, PrintWindow print_window)
{
	auto &cp = GetCp();
	
	switch (cmd_type)
	{
	case CmdType::TABLE_CREATE:      // ������
		print_window.CreateTable(CreateTable(CreateTableInfo(sen_str), cp.GetCurrentPath()));
		break;
	case CmdType::TABLE_DROP:        // ɾ����
		break;
	case CmdType::TABLE_SHOW:        // �г���ǰ���ݿ������б�
		print_window.ShowAllTable(sen_str, cp.GetCurrentPath());

		break;
	case CmdType::TABLE_SELECT:      // ѡ�����ض���¼
		break;
	case CmdType::TABLE_INSERT:      // �����µļ�¼
		break;
	case CmdType::TABLE_UPDATE:      // ���±�ļ�¼
		break;
	case CmdType::TABLE_DELETE:      // ɾ����ļ�¼
		break;

	case CmdType::DB_CREATE:         // �������ݿ�
		print_window.CreateDB(CreateDatabase(CreateDbInfo(sen_str), cp));
		break;

	case CmdType::DB_DROP:           // ɾ�����ݿ�
		print_window.DropDB(DropDatabase(DeleteDbInfo(sen_str), cp));
		break;

	case CmdType::DB_SHOW:           // �г��������ݿ�
		print_window.ShowDB(ShowDatabase(cp));
		break;

	case CmdType::DB_USE:            // ʹ�����ݿ�
		print_window.UseDB(UseDatabase(UseDbInfo(sen_str), cp));
		break;

	default:
		break;
	}
}

