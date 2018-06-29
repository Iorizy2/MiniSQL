#ifndef __MiniSql_H__
#define __MiniSql_H__

#include "../GLOBAL/global.h"
#include "../BPLUSTREE/bptree.h"
#include "../RECORD/Record.h"
//#include "../INTERPRETER/interpreter.h"

// �������ݿ�
bool CreateDatabase(std::string database_name, CatalogPosition &cp);

// Drop���ݿ�
bool DropDatabase(std::string database_name, CatalogPosition &cp);
void DelFilesInFolder(std::string folderPath);  // ɾ��Ŀ¼�µ������ļ����ļ���

												// �������ݿ���������
std::vector<std::string> ShowDatabase(CatalogPosition &cp);

// ѡ�����ݿ�
bool UseDatabase(std::string db_name, CatalogPosition &cp);


// ������ eg. create table test1(id int,score double,Name char(20) primary);
bool CreateTable(TB_Create_Info tb_create_info, std::string path = std::string("./"));

// ɾ����
bool DropTable(std::string table_name, std::string path = std::string("./"));
// ��ʾ���ݿ��µı���
std::vector<std::string> ShowAllTable(bool b, std::string path = std::string("./"));

// �����¼ eg. insert into test1(id, score, Name)values(10, 1.5, bcd);
void InsertRecord(TB_Insert_Info tb_insert_info, std::string path = std::string("./"));

// ��ӡ���ű�
std::vector<RecordHead> ShowTable(std::string table_name, std::string path = std::string("./"));

// ȡ��ָ����ַ������
RecordHead GetDbfRecord(std::string table_name, FileAddr fd, std::string path = std::string("./"));

#endif //__MiniSql_H__
