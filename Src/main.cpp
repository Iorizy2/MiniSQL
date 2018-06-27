#include <iostream>
#include <string>
#include "ERROR/Error.h"
#include "BUFFER/Buffer.h"
#include "BPLUSTREE/bptree.h"
#include "RECORD/Record.h"
#include "INTERPRETER/interpreter.h"
#include "include/MiniSql.h"
//#define NDEBUG 
using namespace std;

// 判断POD数据
void IsPod();
#ifndef NDEBUG
void TestModule();
#endif

int main()
{
	IsPod();
#ifndef NDEBUG
	try
	{
		//TestModule();
		//throw SQLError::LSEEK_ERROR();

		// 解释有意字串
		/*std::string cmd = "create table test1(id int,ISBN char(16),Name char(20))";
		SensefulStr SenStr(cmd);
		auto sen_str = SenStr.GetSensefulStr();
		for (auto str : sen_str)
			cout << str << endl;

		TB_Create_Info tb_info = CreateTableInfo(sen_str);
		CreateTable(tb_info);*/

		// 插入记录
		/*std::string cmd_insert = "insert into test1(id, ISBN, Name)values(89,hello1,test1 ver1!!);";
		SensefulStr SenStr(cmd_insert);
		auto sen_str = SenStr.GetSensefulStr();
		auto tb_insert_info = CreateInsertInfo(sen_str);
		InsertRecord(tb_insert_info);*/

		auto p = ShowTable("test1", "./");
	}
	catch (SQLError::BaseError &e)
	{
		SQLError::DispatchError(e);
		cout << endl;
	}
#endif

	system("pause");
}

void IsPod()
{
	cout << std::is_pod<PAGEHEAD>::value << endl;
	cout << std::is_pod<FileAddr>::value << endl;
	cout << std::is_pod<FILECOND>::value << endl;
	cout << std::is_pod<BTNode>::value << endl;
	cout << std::is_pod<Column_Value>::value << endl;
}
#ifndef NDEBUG
void TestModule()
{
	using std::string;
	using std::vector;
	using std::cin;
	using std::cout;
	using std::endl;
	auto &buffer = GetGlobalFileBuffer();
	string idx_name = "test.idx";
	string dbf_name = "test.dbf";
	string record_info = "idc040";

	// 删除已有的文件
	remove(idx_name.c_str());
	remove(dbf_name.c_str());

	// 创建数据文件
	buffer.CreateFile(dbf_name.c_str());
	// 创建索引文件
	char _RecordTypeInfo[RecordColumnCount];
	char _RecordColumnName[RecordColumnCount / 4 * ColumnNameLength];
	BTree tree(idx_name, 'i', _RecordTypeInfo, _RecordColumnName);

	Record record;
	unsigned int record_sz = 0;  // 记录大小
	unsigned int str_sz_tmp = 0; // 字符串字段的长度

	// 计算记录大小
	auto pMemPage = buffer.GetMemFile(idx_name.c_str());
	IndexHeadNode * pIndexHeadNode = (IndexHeadNode*)(pMemPage->GetFileFirstPage()->GetFileCond()->reserve);
	cout << pIndexHeadNode->RecordTypeInfo << endl;
	char *rdinfo = (char*)malloc(strlen(pIndexHeadNode->RecordTypeInfo)+1);
	memcpy(rdinfo, pIndexHeadNode->RecordTypeInfo, strlen(pIndexHeadNode->RecordTypeInfo)+1);
	
	for (int i = 0; rdinfo[i] != '0'; i++)
	{
		if (rdinfo[i] == 'i')
		{
			record_sz += sizeof(int);
		}
		else if (rdinfo[i] == 'd')
		{
			record_sz += sizeof(double);
		}
		else if (rdinfo[i] == 'c')
		{
			for (int j = 1; j <= 3; j++)
			{
				str_sz_tmp = str_sz_tmp * 10 + rdinfo[i + j] - '0';
			}
			record_sz += str_sz_tmp;
			i += 3;
		}
	}

	cout << "记录的大小为" << record_sz << endl;

	char c = 'c';
	while (1)
	{
		fflush(stdin);
		std::cout << "operator: insert(i), delete(d), search(q), print(p), quit(q)" << std::endl;
		c = (char)getchar();
		if (c == 'd')
		{
			/*std::cout << "delete key" << std::endl;
			KeyAttr key;
			int x;
			std::cin >> x;
			key.x = x;
			auto fd = tree.Delete(key);
			cout << "被删除的记录地址" << fd.filePageID << " " << fd.offSet << endl;

			record.DeleteRecord(dbf_name, fd, record_sz);*/


		}
		if (c == 'i')
		{
			/*std::cout << "insert record" << std::endl;
			KeyAttr key;

			RecordHead record_head;
			Column_Cell cc;

			cc.column_type = Column_Type::I;
			cin >> cc.column_value.IntValue;
			record_head.AddColumnCell(cc);

			key.x = cc.column_value.IntValue;

			cc.column_type = Column_Type::D;
			cin >> cc.column_value.DoubleValue;
			record_head.AddColumnCell(cc);

			cc.column_type = Column_Type::C;
			std::string s;
			cin >> s;
			char *pstr = (char*)malloc(3 + str_sz_tmp);
			pstr[0] = '0';
			pstr[1] = '4';
			pstr[2] = '3';
			
			memcpy(pstr + 3, s.data(), s.size());
			*(pstr + 3 + s.size()) = '\0';
			cc.column_value.StrValue = pstr;
			record_head.AddColumnCell(cc);

			
			auto data_fd = record.InsertRecord(dbf_name, record_head);
			tree.Insert(key, data_fd);*/
		}
		if (c == 'u')
		{
			/*std::cout << "update record" << std::endl;
			KeyAttr key;
			KeyAttr key_new;
			std::cout << "input key to update :" << std::endl;
			cin >> key.x;
			std::cout << "input new key  :" << std::endl;
			cin >> key_new.x;
			auto update_fd = tree.UpdateKey(key, key_new);

			std::cout << "input new record :" << std::endl;
			RecordHead record_head;
			Column_Cell cc;

			cc.column_type = Column_Type::I;
			cin >> cc.column_value.IntValue;
			record_head.AddColumnCell(cc);

			cc.column_type = Column_Type::D;
			cin >> cc.column_value.DoubleValue;
			record_head.AddColumnCell(cc);

			cc.column_type = Column_Type::C;
			std::string s;
			cin >> s;
			char *pstr = (char*)malloc(3 + str_sz_tmp);
			pstr[0] = '0';
			pstr[1] = '4';
			pstr[2] = '3';

			memcpy(pstr + 3, s.data(), s.size());
			*(pstr + 3 + s.size()) = '\0';
			cc.column_value.StrValue = pstr;
			record_head.AddColumnCell(cc);


			auto data_fd = record.UpdateRecord(dbf_name, record_head, update_fd);*/
			
		}
		if (c == 's')
		{
			/*std::cout << "input search key" << std::endl;
			KeyAttr key;
			cin >> key.x;
			auto fd = tree.Search(key);
			if (fd.offSet == 0)
				cout << "关键字不存在" << endl;
			else
			{
				auto pdata = (char*)buffer[dbf_name.c_str()]->ReadRecord(&fd);
				cout << "记录的地址为:" << ((FileAddr*)pdata)->filePageID << " " << ((FileAddr*)pdata)->offSet << endl;
				pdata += sizeof(FileAddr);

				cout << "记录内容：" << *((int*)pdata)<<" ";
				pdata += sizeof(int);
				cout << *((double*)pdata) << " ";
				pdata += sizeof(double);
				cout << pdata << endl;
				
			}*/
		}
		if (c == 'p')
		{
			std::cout << "索引列表：" << std::endl;
			tree.PrintBTreeStruct();
			std::cout << std::endl;
		}


		if (c == 'q')
		{
			break;
		}


		fflush(stdin);
		fflush(stdin);
		fflush(stdin);
		getchar();

	}
	delete rdinfo;
	tree.PrintAllLeafNode();
}
#endif