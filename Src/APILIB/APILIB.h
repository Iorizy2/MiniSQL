#ifndef __MiniSql_H__
#define __MiniSql_H__

#include "../GLOBAL/global.h"
#include "../BPLUSTREE/bptree.h"
#include "../RECORD/Record.h"
//#include "../INTERPRETER/interpreter.h"

// 条件查找类
enum Operator_Type { B, BE, L, LE, E, NE };
Operator_Type GetOperatorType(std::string s);
class CompareCell                     //一个字段比较单元
{
public:
	CompareCell(Operator_Type t, Column_Cell cc) :OperType(t), cmp_value(cc) {}
	bool operator()(const Column_Cell &cc);
	Operator_Type	OperType;	        //比较关系关系运算符
	Column_Cell		cmp_value;
};
struct SelectPrintInfo
{
	std::string table_name;
	std::vector<std::string> name_selected_column;
	std::vector<FileAddr> fds;
};

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
bool InsertRecord(TB_Insert_Info tb_insert_info, std::string path = std::string("./"));

// 选择记录
SelectPrintInfo SelectTable(TB_Select_Info tb_select_info, std::string path = std::string("./"));

// 打印整张表
std::vector<RecordHead> ShowTable(std::string table_name, std::string path = std::string("./"));

// 取出指定地址的数据
RecordHead GetDbfRecord(std::string table_name, FileAddr fd, std::string path = std::string("./"));


// 返回给定名表中各个字段名称以及对应类型
std::vector<std::pair<std::string,Column_Type>> GetColumnAndTypeFromTable(std::string table_name, std::string table_path);
Column_Type GetType(std::string name, std::vector<std::pair<std::string, Column_Type>> vec);

// 范围查找单个字段 返回查找的值在数据文件中的地址
std::vector<FileAddr> RangeSearch(CompareCell compare_cell, std::string table_name, std::string path = std::string("./"));

#endif //__MiniSql_H__
