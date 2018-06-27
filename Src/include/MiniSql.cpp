#include "MiniSql.h"

void CreateTable(TB_Create_Info tb_create_info, std::string path)
{
	// ��ȡ����
	std::string table_name = tb_create_info.table_name;

	// ��ȡ�ؼ���λ��
	int KeyTypeIndex = 0;
	for (int j = 0; j < tb_create_info.columns_info.size(); j++)
	{
		if (tb_create_info.columns_info[j].isPrimary)
		{
			KeyTypeIndex = j;
			break;
		}		
	}

	//��ȡ�ֶ���Ϣ
	char RecordTypeInfo[RecordColumnCount];          // ��¼�ֶ�������Ϣ
	char *ptype = RecordTypeInfo;
	char RecordColumnName[RecordColumnCount / 4 * ColumnNameLength];
	char*pname = RecordColumnName;

	auto &column_info_ref = tb_create_info.columns_info;
	for (int i = 0; i < column_info_ref.size(); i++)
	{
		// ������Ϣ
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
		// ������Ϣ
		strcpy(pname, column_info_ref[i].name.c_str());
		pname += ColumnNameLength;
	}
	*ptype = '\0';

	// ���������ļ�
	std::string idx_file = path + table_name + ".idx";
	BTree tree(idx_file, KeyTypeIndex, RecordTypeInfo, RecordColumnName);
	// ���������ļ�
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
	std::cout << "��ӡ����ͷ����Ϣ" << std::endl;
	std::cout << "��¼�ֶ����ͣ�" << phead->RecordTypeInfo << std::endl;
#endif

	int sz_col = 0;// �ֶθ���
	for (int i = 0; phead->RecordTypeInfo[i] != '\0'; i++)
	{
		if (phead->RecordTypeInfo[i] == 'I' || phead->RecordTypeInfo[i] == 'C' || phead->RecordTypeInfo[i] == 'D')sz_col++;
	}

#ifndef NDEBUG
	std::cout << "�����ֶ����ƣ�" << std::endl;
	char *pColumnName = phead->RecordColumnName;
	for (int j = 0; j < sz_col; j++)
	{
		std::cout << pColumnName << std::endl;
		pColumnName += ColumnNameLength;
	}
#endif

	// ����¼��Ϣ��װ�ɼ�¼���ݶ���
	RecordHead record_head;
	int column_id = 0;
	for (int i = 0; phead->RecordTypeInfo[i] != '\0'; i++)
	{
		Column_Cell cc;
		if (phead->RecordTypeInfo[i] == 'I')
		{
			//�ҵ���Ӧ���ֶ�����
			char *pColumnName = phead->RecordColumnName+column_id*ColumnNameLength;
			//�ڲ����¼��Ѱ�Ҹ��ֶε�ֵ
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
				// Ĭ��ֵ���
				cc.column_type = Column_Type::I;
			}
			column_id++;
			record_head.AddColumnCell(cc);
		}

		if (phead->RecordTypeInfo[i] == 'D')
		{
			//�ҵ���Ӧ���ֶ�����
			char *pColumnName = phead->RecordColumnName + column_id*ColumnNameLength;
			//�ڲ����¼��Ѱ�Ҹ��ֶε�ֵ
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
				cc.column_value.IntValue = stod(tb_insert_info.insert_info[k].column_value);
			}
			else
			{
				// Ĭ��ֵ���
				cc.column_type = Column_Type::D;
			}

			column_id++;
			record_head.AddColumnCell(cc);
		}

		if (phead->RecordTypeInfo[i] == 'C')
		{
			//�ҵ���Ӧ���ֶ�����
			char *pColumnName = phead->RecordColumnName + column_id*ColumnNameLength;
			//�ڲ����¼��Ѱ�Ҹ��ֶε�ֵ
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
				char*pChar = (char*)malloc(sz+3); // �����������ֽ����������û�������ַ�������ֵ
				memcpy(pChar, &(phead->RecordTypeInfo[i+1]),3);
				pChar += 3;
				strcpy(pChar, tb_insert_info.insert_info[k].column_value.c_str());
				cc.column_value.StrValue = pChar-3;
			}
			else
			{
				// Ĭ��ֵ���
				cc.column_type = Column_Type::C;
				cc.column_value.StrValue = nullptr;
			}
			column_id++;
			record_head.AddColumnCell(cc);
		}

	}
    
	// ���������ļ�
	Record record;
	auto fd = record.InsertRecord(dbf_file,record_head);
	// ��������
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
