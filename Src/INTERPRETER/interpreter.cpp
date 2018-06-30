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
		Column_Cell column_cell;
		column_cell.columu_name = sen_str[i];
		Column_Type tmp = GetType(sen_str[i], mpair);
		char*pChar = nullptr;
		switch (tmp)
		{
		case Column_Type::I:
			column_cell.column_type = Column_Type::I;
			column_cell.column_value.IntValue = stoi(sen_str[i + 2]);
			break;

		case Column_Type::C:
			column_cell.column_type = Column_Type::C;
			pChar = (char*)malloc(sen_str[i + 2].size() + 1);
			strcpy(pChar, sen_str[i + 2].c_str());
			column_cell.column_value.StrValue = pChar;
			break;

		case Column_Type::D:
			column_cell.column_type = Column_Type::D;
			column_cell.column_value.IntValue = stod(sen_str[i + 2]);
			break;
		default:
			break;
		}
		CompareCell cmp_cell(GetOperatorType(sen_str[i+1]), column_cell);
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

	throw SQLError::CMD_FORMAT_ERROR();

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
		break;
	case CmdType::TABLE_DELETE:      // 删除表的记录
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
		std::cout << "创建成功" << std::endl;
	}
	else
	{
		std::cout << "创建失败" << std::endl;
	}
}



void PrintWindow::ShowAllTable(std::vector<std::string> sen_str, std::string path)
{
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
		std::cout << e << std::endl;
}

void PrintWindow::DropTable(bool is_dropped)
{
	if (is_dropped)
	{
		std::cout << "删除表成功" << std::endl;
	}
	else
	{
		std::cout << "删除表失败，表不存在或者没有使用数据库" << std::endl;
	}
}


void PrintWindow::SelectTable(SelectPrintInfo select_table_print_info)
{
	RecordHead record_head;
	auto fds = select_table_print_info.fds;
	auto table_name = select_table_print_info.table_name;
	auto pcolumn = record_head.GetFirstColumn();
	
	for (int i = 0; i < fds.size(); i++)
	{
		record_head = GetDbfRecord(table_name, fds[i], GetCp().GetCurrentPath());
		pcolumn = record_head.GetFirstColumn();
		while (pcolumn)
		{
			switch (pcolumn->column_type)
			{
			case Column_Type::I:
				std::cout << pcolumn->column_value.IntValue << "\t";
				break;
			case Column_Type::D:
				std::cout << pcolumn->column_value.DoubleValue << "\t";
				break;
			case Column_Type::C:
				std::cout << pcolumn->column_value.StrValue << "\t";
				break;
			default:
				break;
			}
			pcolumn = pcolumn->next;
		}
		std::cout << std::endl;
	}
}

void PrintWindow::InsertRecord(bool is_inserted)
{
	if (is_inserted)
	{
		std::cout << "插入成功" << std::endl;
	}
	else
	{
		std::cout << "插入失败" << std::endl;
	}
}

void PrintWindow::CreateDB(bool is_created)
{
	if (is_created)
	{
		std::cout << "创建成功" << std::endl;
	}
	else
	{
		std::cout << "创建失败" << std::endl;
	}
}

void PrintWindow::DropDB(bool is_dropped)
{
	if (is_dropped)
	{
		std::cout << "删除数据库成功" << std::endl;
	}
	else
	{
		std::cout << "删除数据库失败" << std::endl;
	}
}

void PrintWindow::ShowDB(std::vector<std::string> db_names)
{
	for (auto e : db_names)
		std::cout << e << std::endl;
}

void PrintWindow::UseDB(bool isUsed)
{
	if (isUsed)
	{
		std::cout << "选择数据库成功" << std::endl;
	}
	else
	{
		std::cout << "选择数据库失败" << std::endl;
	}
}
