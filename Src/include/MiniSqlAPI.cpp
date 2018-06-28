#include "MiniSqlAPI.h"

void CreateTable(TB_Create_Info tb_create_info, std::string path)
{
	// 获取表名
	std::string table_name = tb_create_info.table_name;

	// 获取关键字位置
	int KeyTypeIndex = 0;
	for (int j = 0; j < tb_create_info.columns_info.size(); j++)
	{
		if (tb_create_info.columns_info[j].isPrimary)
		{
			KeyTypeIndex = j;
			break;
		}		
	}

	//获取字段信息
	char RecordTypeInfo[RecordColumnCount];          // 记录字段类型信息
	char *ptype = RecordTypeInfo;
	char RecordColumnName[RecordColumnCount / 4 * ColumnNameLength];
	char*pname = RecordColumnName;

	auto &column_info_ref = tb_create_info.columns_info;
	for (int i = 0; i < column_info_ref.size(); i++)
	{
		// 类型信息
		switch (column_info_ref[i].type)
		{
		case Column_Type::I:
			*ptype++ = 'I';
			break;

		case  Column_Type::D:
			*ptype++ = 'D';
			break;

		case Column_Type::C:
			*ptype++ = 'C';
			*ptype++ = column_info_ref[i].length / 100 + '0';
			*ptype++ = (column_info_ref[i].length % 100) / 10 + '0';
			*ptype++ = column_info_ref[i].length % 10 + '0';
		default:
			break;
		}
		// 名称信息
		strcpy(pname, column_info_ref[i].name.c_str());
		pname += ColumnNameLength;
	}
	*ptype = '\0';

	// 创建索引文件
	std::string idx_file = path + table_name + ".idx";
	BTree tree(idx_file, KeyTypeIndex, RecordTypeInfo, RecordColumnName);
	// 创建数据文件
	std::string dbf_file = path + table_name + ".dbf";
	GetGlobalFileBuffer().CreateFile(dbf_file.c_str());
}

void InsertRecord(TB_Insert_Info tb_insert_info, std::string path /*= std::string("./")*/)
{
	std::string idx_file = path + tb_insert_info.table_name+".idx";
	std::string dbf_file = path + tb_insert_info.table_name + ".dbf";
	BTree tree(idx_file);
	auto phead = tree.GetPtrIndexHeadNode();

	KeyAttr key;

#ifndef NDEBUG
	std::cout << "打印索引头部信息" << std::endl;
	std::cout << "记录字段类型：" << phead->RecordTypeInfo << std::endl;
#endif

	int sz_col = 0;// 字段个数
	for (int i = 0; phead->RecordTypeInfo[i] != '\0'; i++)
	{
		if (phead->RecordTypeInfo[i] == 'I' || phead->RecordTypeInfo[i] == 'C' || phead->RecordTypeInfo[i] == 'D')sz_col++;
	}

#ifndef NDEBUG
	std::cout << "各个字段名称：" << std::endl;
	char *pColumnName = phead->RecordColumnName;
	for (int j = 0; j < sz_col; j++)
	{
		std::cout << pColumnName << std::endl;
		pColumnName += ColumnNameLength;
	}
#endif

	// 将记录信息封装成记录数据对象
	RecordHead record_head;
	int column_id = 0;
	for (int i = 0; phead->RecordTypeInfo[i] != '\0'; i++)
	{
		Column_Cell cc;
		if (phead->RecordTypeInfo[i] == 'I')
		{
			//找到对应的字段名称
			char *pColumnName = phead->RecordColumnName+column_id*ColumnNameLength;
			//在插入记录里寻找该字段的值
			int k = -1;
			for (int j = 0; j < tb_insert_info.insert_info.size(); j++)
			{
				if (pColumnName == tb_insert_info.insert_info[j].column_name)
				{
					k = j;
					break;
				}
			}

			if (k != -1)
			{
				
				cc.column_type = Column_Type::I;
				cc.column_value.IntValue = stoi(tb_insert_info.insert_info[k].column_value);
			}
			else
			{
				// 默认值填充
				cc.column_type = Column_Type::I;
			}
			column_id++;
			record_head.AddColumnCell(cc);
		}

		if (phead->RecordTypeInfo[i] == 'D')
		{
			//找到对应的字段名称
			char *pColumnName = phead->RecordColumnName + column_id*ColumnNameLength;
			//在插入记录里寻找该字段的值
			int k = -1;
			for (int j = 0; j < tb_insert_info.insert_info.size(); j++)
			{
				if (pColumnName == tb_insert_info.insert_info[j].column_name)
				{
					k = j;
					break;
				}
			}

			if (k != -1)
			{

				cc.column_type = Column_Type::D;
				cc.column_value.DoubleValue = stod(tb_insert_info.insert_info[k].column_value);
			}
			else
			{
				// 默认值填充
				cc.column_type = Column_Type::D;
			}

			column_id++;
			record_head.AddColumnCell(cc);
		}

		if (phead->RecordTypeInfo[i] == 'C')
		{
			//找到对应的字段名称
			char *pColumnName = phead->RecordColumnName + column_id*ColumnNameLength;
			//在插入记录里寻找该字段的值
			int k = -1;
			for (int j = 0; j < tb_insert_info.insert_info.size(); j++)
			{
				if (pColumnName == tb_insert_info.insert_info[j].column_name)
				{
					k = j;
					break;
				}
			}

			if (k != -1)
			{
				cc.column_type = Column_Type::C;
				int sz = (phead->RecordTypeInfo[i + 1] - '0') * 100 + (phead->RecordTypeInfo[i + 2] - '0') * 10 + (phead->RecordTypeInfo[i + 3] - '0');
				char*pChar = (char*)malloc(sz+3); // 多申请三个字节用来保存用户定义的字符串长度值
				memcpy(pChar, &(phead->RecordTypeInfo[i+1]),3);
				pChar += 3;
				strcpy(pChar, tb_insert_info.insert_info[k].column_value.c_str());
				cc.column_value.StrValue = pChar-3;
			}
			else
			{
				// 默认值填充
				cc.column_type = Column_Type::C;
				cc.column_value.StrValue = nullptr;
			}
			column_id++;
			record_head.AddColumnCell(cc);
		}

	}
    
	// 插入数据文件
	Record record;
	auto fd = record.InsertRecord(dbf_file,record_head);
	// 插入索引
	int key_index = 0;
	auto p = record_head.GetFirstColumn();
	while (key_index != phead->KeyTypeIndex)
	{
		p = p->next;
		key_index++;
	}
	key = *p;
	tree.Insert(key, fd);
}

std::vector<RecordHead> ShowTable(std::string table_name, std::string path /*= std::string("./")*/)
{
	std::string idx_file = path + table_name + ".idx";
	std::string dbf_file = path + table_name + ".dbf";
	BTree tree(idx_file);
	std::vector<RecordHead> vec_record_head;

	auto data_fd = tree.GetPtrIndexHeadNode()->MostLeftNode;
	while (data_fd.offSet != 0)
	{
		auto pNode = tree.FileAddrToMemPtr(data_fd);

		for (int i = 0; i < pNode->count_valid_key; i++)
		{
			auto tmp = GetDbfRecord(table_name, pNode->children[i], path);
			vec_record_head.push_back(tmp);
		}
		
		
		data_fd = pNode->next;
	}

	return vec_record_head;
}

RecordHead GetDbfRecord(std::string table_name, FileAddr fd, std::string path /*= std::string("./")*/)
{
	std::string idx_file = path + table_name + ".idx";
	std::string dbf_file = path + table_name + ".dbf";
	BTree tree(idx_file);
	
	RecordHead record_head;
	// 获取结点内存地址
	char* pRecTypeInfo = tree.GetPtrIndexHeadNode()->RecordTypeInfo;
	//std::cout << pRecTypeInfo << std::endl;
	auto pdata = (char*)GetGlobalFileBuffer()[dbf_file.c_str()]->ReadRecord(&fd);
	pdata += sizeof(FileAddr);  // 每条记录头部默认添加该记录的地址值

	while (*pRecTypeInfo != '\0')
	{
		Column_Cell cc;
		switch (*pRecTypeInfo)
		{
		case 'I':
			cc.column_type = Column_Type::I;
			cc.column_value.IntValue = *(int*)pdata;
			pdata += sizeof(int);
			record_head.AddColumnCell(cc);
			break;

		case 'D':
			cc.column_type = Column_Type::D;
			cc.column_value.DoubleValue = *(double*)pdata;
			pdata += sizeof(double);
			record_head.AddColumnCell(cc);
			break;

		case 'C':
			cc.column_type = Column_Type::C;
			// 读取字符串长度
			int sz = 0;
			sz = (*(pRecTypeInfo + 1) - '0') * 100 + (*(pRecTypeInfo + 2) - '0') * 10 + (*(pRecTypeInfo + 3) - '0');
			auto pchar = (char*)malloc(sz);
			memcpy(pchar, pdata, sz);
			cc.column_value.StrValue = pchar;
			pdata += sz;
			record_head.AddColumnCell(cc);
			break;
		}
		pRecTypeInfo++;
	}
	
	return record_head;
}
