#ifndef __MiniSql_H__
#define __MiniSql_H__

#include "../INTERPRETER/interpreter.h"
#include "../GLOBAL/global.h"
#include "../BPLUSTREE/bptree.h"
#include "../BUFFER/Buffer.h"
#include "../RECORD/Record.h"
// ������ eg. create table test1(id int,score double,Name char(20) primary);
void CreateTable(TB_Create_Info tb_create_info, std::string path = std::string("./"));

// �����¼ eg. insert into test1(id, score, Name)values(10, 1.5, bcd);
void InsertRecord(TB_Insert_Info tb_insert_info, std::string path = std::string("./"));

// ��ӡ���ű�
std::vector<RecordHead> ShowTable(std::string table_name, std::string path = std::string("./"));

// ȡ��ָ����ַ������
RecordHead GetDbfRecord(std::string table_name, FileAddr fd, std::string path = std::string("./"));
#endif //__MiniSql_H__
