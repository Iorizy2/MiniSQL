#ifndef __MiniSql_H__
#define __MiniSql_H__

#include "../GLOBAL/global.h"
#include "../BPLUSTREE/bptree.h"
#include "../RECORD/Record.h"
//#include "../INTERPRETER/interpreter.h"

// 创建数据库
bool CreateDatabase(std::string database_name, CatalogPosition &cp);

// Drop数据库
bool DropDatabase(std::string database_name, CatalogPosition &cp);
void DelFilesInFolder(std::string folderPath);  // 删除目录下的所有文件及文件夹

												// 返回数据库名称向量
std::vector<std::string> ShowDatabase(CatalogPosition &cp);

// 选择数据库
bool UseDatabase(std::string db_name, CatalogPosition &cp);


// 创建表 eg. create table test1(id int,score double,Name char(20) primary);
bool CreateTable(TB_Create_Info tb_create_info, std::string path = std::string("./"));

// 删除表
bool DropTable(std::string table_name, std::string path = std::string("./"));
// 显示数据库下的表项
std::vector<std::string> ShowAllTable(bool b, std::string path = std::string("./"));

// 插入记录 eg. insert into test1(id, score, Name)values(10, 1.5, bcd);
void InsertRecord(TB_Insert_Info tb_insert_info, std::string path = std::string("./"));

// 打印整张表
std::vector<RecordHead> ShowTable(std::string table_name, std::string path = std::string("./"));

// 取出指定地址的数据
RecordHead GetDbfRecord(std::string table_name, FileAddr fd, std::string path = std::string("./"));

#endif //__MiniSql_H__
