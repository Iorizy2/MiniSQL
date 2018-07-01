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

std::string DropTableInfo(std::vector<std::string> sen_str)
{
	if ((sen_str.size() < 4)
		|| (StrToLower(sen_str[0]) != "drop")
		|| (StrToLower(sen_str[1]) != "table")
		|| (StrToLower(sen_str[3]) != ";")
		)
		throw SQLError::CMD_FORMAT_ERROR();
	return sen_str[2];
}

TB_Select_Info TableSelectInfo(std::vector<std::string> sen_str)
{
	TB_Select_Info tb_select_info;
	// 选择的字段名称
	if (StrToLower(sen_str[0]) != "select")
		throw SQLError::CMD_FORMAT_ERROR();
	int name_L_index = 1;
	int name_R_index = 0;
	for (int i = 0; i < sen_str.size(); i++)
	{
		if (StrToLower(sen_str[i]) == "from")
		{
			name_R_index = i-1;
			break;
		}
	}
	if(!name_R_index)
		throw SQLError::CMD_FORMAT_ERROR();
	for (int i = name_L_index; i <= name_R_index; i++)
	{
		tb_select_info.name_selected_column.push_back(sen_str[i]);
	}
	if(sen_str.size()-1 < (name_R_index+2))
		throw SQLError::CMD_FORMAT_ERROR();
	tb_select_info.table_name = sen_str[name_R_index + 2];

	int name_where_index = name_R_index + 3;
	if (sen_str.size() - 1 < name_where_index)
		return tb_select_info;

	auto mpair = GetColumnAndTypeFromTable(tb_select_info.table_name, GetCp().GetCurrentPath());
	// 打包查找条件
	for (int i = name_where_index + 1; i < sen_str.size();)
	{
		if (StrToLower(sen_str[i]) == ";")
			break;
	    CompareCell cmp_cell = CreateCmpCell(sen_str[i], GetType(sen_str[i], mpair), GetOperatorType(sen_str[i + 1]), sen_str[i + 2]);
		tb_select_info.vec_cmp_cell.push_back(cmp_cell);

		// 下一个查找条件
		if (StrToLower(sen_str[i + 3]) == "and")
		{
			i += 4;
		}
		else if (StrToLower(sen_str[i + 3]) == ";")
		{
			break;
		}
		else
		{
			throw SQLError::CMD_FORMAT_ERROR();
		}
	}
	return tb_select_info;
}

TB_Update_Info TableUpdateInfo(std::vector<std::string> sen_str)
{
	// TODO 语法检擦

	TB_Update_Info tb_update_info;
	int UPDATE = 0;
	int SET = 2;
	int WHERE = 0;
	for (int i = 0; i < sen_str.size(); i++)
	{
		if (StrToLower(sen_str[i]) == "where")
		{
			WHERE = i;
			break;
		}
	}

	// 新的字段值
	for (int j = SET + 1; j != WHERE;)
	{
		TB_Update_Info::NewValue new_value;
		new_value.field = sen_str[j];
		new_value.value = sen_str[j + 2];
		tb_update_info.field_value.push_back(new_value);
		if (sen_str[j + 3] == ",")
			j += 4;
		else
			j += 3;
	}

	// 需要更新的字段条件
	for (int j = WHERE + 1; j < sen_str.size();)
	{
		TB_Update_Info::Expr expr;
		expr.field = sen_str[j];
		expr.op = sen_str[j + 1];
		expr.value = sen_str[j + 2];
		tb_update_info.expr.push_back(expr);
		j += 4;
	}
	tb_update_info.table_name = sen_str[UPDATE + 1];
	return tb_update_info;
}

TB_Delete_Info TableDeleteInfo(std::vector<std::string> sen_str)
{
	TB_Delete_Info tb_delete_info;
	tb_delete_info.table_name = sen_str[2];

	for (int i = 4; i < sen_str.size(); )
	{
		if (sen_str[i] == ";")
			break;
		TB_Delete_Info::Expr expr;
		expr.field = sen_str[i];
		expr.op = sen_str[i + 1];
		expr.value = sen_str[i + 2];
		tb_delete_info.expr.push_back(expr);
		i += 4;
	}
	return tb_delete_info;
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
		else if (sen_str[i + 2] == ")")
		{
			i += 3;
			continue;
		}
	}

	return tb_create_info;
}

TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str)
{
	TB_Insert_Info tb_insert_info;


	if (StrToLower(sen_str[0]) != "insert" || StrToLower(sen_str[1]) != "into")
		throw SQLError::CMD_FORMAT_ERROR();

	// 读取表名
	tb_insert_info.table_name = sen_str[2];
	
	// 命令语法检查
	int col_name_left_bracket = 3;  // 字段名称列表左括号
	int col_name_right_bracket = 0;  // 字段名称列表右括号
	int col_value_left_bracket = 0;  // 字段值列表左括号
	int col_value_right_bracket = sen_str.size() - 2;  // 字段值列表右括号

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

	// 检查是否有字符串因包含空格字符而被误判为两个有意字串
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
	std::cout << "插入的记录键值对" << std::endl;
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

	if (sen_str[0] == "delete")
	{
		return CmdType::TABLE_SELECT;
	}

	if (sen_str[0] == "quit")
	{
		return CmdType::QUIT;
	}
	if (sen_str[0] == "help")
	{
		return CmdType::HELP;
	}

	throw SQLError::CMD_FORMAT_ERROR();

}


bool CompareCell::operator()(const Column_Cell &cc)
{
	switch (cmp_value.column_type)
	{
	case Column_Type::I:
		switch (OperType)
		{
		case B:
			return cc.column_value.IntValue > cmp_value.column_value.IntValue;
			break;
		case BE:
			return cc.column_value.IntValue >= cmp_value.column_value.IntValue;
			break;
		case L:
			return cc.column_value.IntValue < cmp_value.column_value.IntValue;
			break;
		case LE:
			return cc.column_value.IntValue <= cmp_value.column_value.IntValue;
			break;
		case E:
			return cc.column_value.IntValue == cmp_value.column_value.IntValue;
			break;
		case NE:
			return cc.column_value.IntValue != cmp_value.column_value.IntValue;
			break;
		default:
			return false;
			break;
		}
		break;
	case Column_Type::D:
		switch (OperType)
		{
		case B:
			return cc.column_value.DoubleValue > cmp_value.column_value.DoubleValue;
			break;
		case BE:
			return cc.column_value.DoubleValue >= cmp_value.column_value.DoubleValue;
			break;
		case L:
			return cc.column_value.DoubleValue < cmp_value.column_value.DoubleValue;
			break;
		case LE:
			return cc.column_value.DoubleValue <= cmp_value.column_value.DoubleValue;
			break;
		case E:
			return cc.column_value.DoubleValue == cmp_value.column_value.DoubleValue;
			break;
		case NE:
			return cc.column_value.DoubleValue != cmp_value.column_value.DoubleValue;
			break;
		default:
			return false;
			break;
		}
		break;
	case Column_Type::C:
		switch (OperType)
		{
		case B:
			return std::string(cc.column_value.StrValue) > std::string(cmp_value.column_value.StrValue);
			break;
		case BE:
			return std::string(cc.column_value.StrValue) >= std::string(cmp_value.column_value.StrValue);
			break;
		case L:
			return std::string(cc.column_value.StrValue) < std::string(cmp_value.column_value.StrValue);
			break;
		case LE:
			return std::string(cc.column_value.StrValue) <= std::string(cmp_value.column_value.StrValue);
			break;
		case E:
			return std::string(cc.column_value.StrValue) == std::string(cmp_value.column_value.StrValue);
			break;
		case NE:
			return std::string(cc.column_value.StrValue) != std::string(cmp_value.column_value.StrValue);
			break;
		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}
	return false;
}


void PrintWindow::CreateTable(bool is_created)
{
	if (is_created)
	{
		std::cout << "table create succeed!" << std::endl;
	}
	else
	{
		std::cout << "table create failed!" << std::endl;
	}
}



void PrintWindow::ShowAllTable(std::vector<std::string> sen_str, std::string path)
{
	std::cout << "There are all the tables !" << std::endl;
	std::cout << "+---------------------------------------------------------------+" << std::endl;
	Print(PRINTLENGTH, "table");
	std::cout << "+---------------------------------------------------------------+" << std::endl;
	
	



	if (!GetCp().GetIsInSpeDb() || sen_str.size() < 3 || sen_str[2] != ";")
	{
		throw SQLError::CMD_FORMAT_ERROR("Not use database or ");
	}

	std::vector<std::string> tables;

	_finddata_t FileInfo;
	path += "*.*";
	int k;
	long HANDLE;
	k = HANDLE = _findfirst(path.c_str(), &FileInfo);


	while (k != -1)
	{
		// 如果是普通文件夹则输出
		if (!(FileInfo.attrib&_A_SUBDIR) && strcmp(FileInfo.name, ".") != 0 && strcmp(FileInfo.name, "..") != 0)
		{
			// 只检查后缀.idx的文件
			std::string tmp_file(FileInfo.name);
			int index = tmp_file.size() - 1;


			if (tmp_file.size() < 4 || tmp_file[index] != 'x' || tmp_file[index - 1] != 'd' || tmp_file[index - 2] != 'i' || tmp_file[index - 3] != '.')
			{
				;
			}
			else
			{
				tables.push_back(std::string(tmp_file.begin(), tmp_file.begin() + tmp_file.size() - 4));
			}
		}

		k = _findnext(HANDLE, &FileInfo);
	}
	_findclose(HANDLE);

	// 排序
	std::sort(tables.begin(), tables.end());
	for (auto e : tables)
		Print(PRINTLENGTH, e);
	std::cout << "+---------------------------------------------------------------+" << std::endl;
}

void PrintWindow::DropTable(bool is_dropped)
{
	if (is_dropped)
	{
		std::cout << "Drop table succeed!" << std::endl;
	}
	else
	{
		std::cout << "Drop table succeed!" << std::endl;
	}
}


void PrintWindow::SelectTable(SelectPrintInfo select_table_print_info)
{
	std::string idx_file = GetCp().GetCurrentPath() + select_table_print_info.table_name + ".idx";
	std::string dbf_file = GetCp().GetCurrentPath() + select_table_print_info.table_name + ".dbf";
	BTree tree(idx_file);
	auto phead = tree.GetPtrIndexHeadNode();
	std::vector<std::string> col_name;
	std::vector<std::string> out_col_name;
	std::vector<int> col_len;
	int n_output_col = 0;  // 输出的字段个数
	int total_length = 0;  // 记录的总长度

	int sz_col = 0;// 字段个数
	for (int i = 0; phead->RecordTypeInfo[i] != '\0'; i++)
	{
		if (phead->RecordTypeInfo[i] == 'I')
		{
			col_len.push_back(sizeof(int)+10);
			sz_col++;
		}
		if (phead->RecordTypeInfo[i] == 'D')
		{
			col_len.push_back(sizeof(double)+16);
			sz_col++;
		}
		if (phead->RecordTypeInfo[i] == 'C')
		{
			int len = (phead->RecordTypeInfo[i + 1] - '0') * 100 + (phead->RecordTypeInfo[i + 2] - '0') * 10 + (phead->RecordTypeInfo[i + 3] - '0');
			col_len.push_back(len);
			sz_col++;
		}
	}
	char *pColumnName = phead->RecordColumnName;
	for (int j = 0; j < sz_col; j++)
	{
		col_name.push_back(pColumnName);
		pColumnName += ColumnNameLength;
	}



	

	RecordHead record_head;
	std::vector<FileAddr> fds;
	for (int i = 0; i < select_table_print_info.key_fd.size(); i++)
		fds.push_back(select_table_print_info.key_fd[i].second);
	
	auto table_name = select_table_print_info.table_name;
	auto pcolumn = record_head.GetFirstColumn();

	// 第一遍 只计算输出长度
	
	for (int i = 0; i < fds.size(); i++)
	{
		record_head = GetDbfRecord(table_name, fds[i], GetCp().GetCurrentPath());
		pcolumn = record_head.GetFirstColumn();
		if (i != 0)break;
		while (pcolumn)
		{
			int isPrint = false;
			for (int i = 0; i < col_name.size(); i++)
			{
				if (col_name[i] == pcolumn->columu_name)
				{
					isPrint = true;
					break;
				}
			}
			if (isPrint)
			{
				switch (pcolumn->column_type)
				{
				case Column_Type::I:
					if (i == 0) total_length += GetColumnLength(pcolumn->columu_name, col_name, col_len);
					if (i == 0)n_output_col++;
					if (i == 0)out_col_name.push_back(pcolumn->columu_name);
					//std::cout << "|" << std::left << std::setw(GetColumnLength(pcolumn->columu_name, col_name, col_len)) << pcolumn->column_value.IntValue;
					break;
				case Column_Type::D:
					if (i == 0) total_length += GetColumnLength(pcolumn->columu_name, col_name, col_len);
					if (i == 0)n_output_col++;
					if (i == 0)out_col_name.push_back(pcolumn->columu_name);
					//std::cout << "|" << std::left << std::setw(GetColumnLength(pcolumn->columu_name, col_name, col_len)) << pcolumn->column_value.DoubleValue;
					break;
				case Column_Type::C:
					if (i == 0) total_length += GetColumnLength(pcolumn->columu_name, col_name, col_len);
					if (i == 0)n_output_col++;
					if (i == 0)out_col_name.push_back(pcolumn->columu_name);
					//std::cout << "|" << std::left << std::setw(GetColumnLength(pcolumn->columu_name, col_name, col_len)) << pcolumn->column_value.StrValue;
					break;
				default:
					break;
				}
			}
			
			pcolumn = pcolumn->next;
		}
		//std::cout << "|";
		//std::cout << std::endl;
	}
	// 输出头部
	std::cout << "+";
	for (int i = 0; i < total_length + n_output_col - 1; i++)
		std::cout << "-";
	std::cout << "+" << std::endl;
	// 输出各个字段名称
	for (auto e : out_col_name)
	{
		std::cout << "|" << std::left << std::setw(GetColumnLength(e, col_name, col_len)) << e;
	}
	std::cout << "|" << std::endl;
	std::cout << "+";
	for (int i = 0; i < total_length + n_output_col - 1; i++)
		std::cout << "-";
	std::cout << "+" << std::endl;

	// 第二遍 真正输出
	for (int i = 0; i < fds.size(); i++)
	{
		record_head = GetDbfRecord(table_name, fds[i], GetCp().GetCurrentPath());
		pcolumn = record_head.GetFirstColumn();

		while (pcolumn)
		{
			int isPrint = false;
			for (int i = 0; i < col_name.size(); i++)
			{
				if (col_name[i] == pcolumn->columu_name)
				{
					isPrint = true;
					break;
				}
			}
			if (isPrint)
			{
				switch (pcolumn->column_type)
				{
				case Column_Type::I:
					//if (i == 0) total_length += GetColumnLength(pcolumn->columu_name, col_name, col_len);
					//if (i == 0)n_output_col++;
					std::cout << "|" << std::left << std::setw(GetColumnLength(pcolumn->columu_name, col_name, col_len)) << pcolumn->column_value.IntValue;
					break;
				case Column_Type::D:
					//if (i == 0) total_length += GetColumnLength(pcolumn->columu_name, col_name, col_len);
					//if (i == 0)n_output_col++;
					std::cout << "|" << std::left << std::setw(GetColumnLength(pcolumn->columu_name, col_name, col_len)) << pcolumn->column_value.DoubleValue;
					break;
				case Column_Type::C:
					//if (i == 0) total_length += GetColumnLength(pcolumn->columu_name, col_name, col_len);
					//if (i == 0)n_output_col++;
					std::cout << "|" << std::left << std::setw(GetColumnLength(pcolumn->columu_name, col_name, col_len)) << pcolumn->column_value.StrValue;
					break;
				default:
					break;
				}
			}

			pcolumn = pcolumn->next;
		}
		std::cout << "|";
		std::cout << std::endl;
	}
	// 输出最后一行
	std::cout << "+";
	for (int i = 0; i < total_length + n_output_col - 1; i++)
		std::cout << "-";
	std::cout << "+" << std::endl;
}

void PrintWindow::InsertRecord(bool is_inserted)
{
	if (is_inserted)
	{
		std::cout << "Insert succeed!" << std::endl;
	}
	else
	{
		std::cout << "Insert succeed!" << std::endl;
	}
}

void PrintWindow::CreateDB(bool is_created)
{
	if (is_created)
	{
		std::cout << "Create succeed!" << std::endl;
	}
	else
	{
		std::cout << "Create failed!" << std::endl;
	}
}

void PrintWindow::DropDB(bool is_dropped)
{
	if (is_dropped)
	{
		std::cout << "Drop database succeed!" << std::endl;
	}
	else
	{
		std::cout << "Drop database failed!" << std::endl;
	}
}

void PrintWindow::ShowDB(std::vector<std::string> db_names)
{
	std::cout << "There are all the databases !" << std::endl;
	std::cout << "+---------------------------------------------------------------+" << std::endl;
	Print(PRINTLENGTH, "Database");
	std::cout << "+---------------------------------------------------------------+" << std::endl;
	for (auto e : db_names)
	{
		Print(PRINTLENGTH, e);
	}
	std::cout << "+---------------------------------------------------------------+" << std::endl;
}

void PrintWindow::UseDB(bool isUsed)
{
	if (isUsed)
	{
		std::cout << "databae changed!" << std::endl;
	}
	else
	{
		std::cout << "database absent" << std::endl;
	}
}


void PrintWindow::UpdateTable(bool isUpdated)
{
	if (isUpdated)
	{
		std::cout << "Update succeed!" << std::endl;
	}
	else
	{
		std::cout << "Update failed" << std::endl;
	}
}

void PrintWindow::DeleteTable(bool isDeleted)
{
	if (isDeleted)
	{
		std::cout << "Delete succeed!" << std::endl;
	}
	else
	{
		std::cout << "Delete failed" << std::endl;
	}
}

void PrintWindow::Print(int len, std::string s)
{
	std::cout << "|";
	std::cout << s;
	for (int i = 0; i < (PRINTLENGTH - s.size()); i++)std::cout << " ";
	std::cout << "|";
	std::cout << std::endl;

}

int PrintWindow::GetColumnLength(std::string name, std::vector<std::string> col_name, std::vector<int> col_len)
{
	for (int j = 0; j < col_name.size(); j++)
	{
		if (name == col_name[j])
		{
			return col_len[j]>col_name[j].size()? col_len[j]:col_name[j].size();
		}
	}
	return 0;
}

void Interpreter(std::vector<std::string> sen_str, CmdType cmd_type, PrintWindow print_window)
{
	auto &cp = GetCp();
	TB_Select_Info tb_select_info;
	std::vector<FileAddr> fds;


	switch (cmd_type)
	{
	case CmdType::TABLE_CREATE:      // 创建表
		print_window.CreateTable(CreateTable(CreateTableInfo(sen_str), cp.GetCurrentPath()));
		break;

	case CmdType::TABLE_DROP:        // 删除表
		print_window.DropTable(DropTable(DropTableInfo(sen_str), cp.GetCurrentPath()));
		break;

	case CmdType::TABLE_SHOW:        // 列出当前数据库下所有表
		print_window.ShowAllTable(sen_str, cp.GetCurrentPath());
		break;

	case CmdType::TABLE_SELECT:      // 选择表的特定记录
		print_window.SelectTable(SelectTable(TableSelectInfo(sen_str), cp.GetCurrentPath()));
		break;

	case CmdType::TABLE_INSERT:      // 插入新的记录
		print_window.InsertRecord(InsertRecord(CreateInsertInfo(sen_str), cp.GetCurrentPath()));
		break;

	case CmdType::TABLE_UPDATE:      // 更新表的记录
		print_window.UpdateTable(UpdateTable(TableUpdateInfo(sen_str), cp.GetCurrentPath()));
		break;


	case CmdType::TABLE_DELETE:      // 删除表的记录
		print_window.DeleteTable(DeleteTable(TableDeleteInfo(sen_str), cp.GetCurrentPath()));
		break;

	case CmdType::DB_CREATE:         // 创建数据库
		print_window.CreateDB(CreateDatabase(CreateDbInfo(sen_str), cp));
		break;

	case CmdType::DB_DROP:           // 删除数据库
		print_window.DropDB(DropDatabase(DeleteDbInfo(sen_str), cp));
		break;

	case CmdType::DB_SHOW:           // 列出所有数据库
		print_window.ShowDB(ShowDatabase(cp));
		break;

	case CmdType::DB_USE:            // 使用数据库
		print_window.UseDB(UseDatabase(UseDbInfo(sen_str), cp));
		break;

	default:
		throw SQLError::CMD_FORMAT_ERROR();
		break;
	}
}
