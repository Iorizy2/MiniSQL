/***********************************************************************************

** 文件名: interpreter.h

** Copyright (c)

** 创建人: ReFantasy/TDL

** 日  期: 2018-06-27

** 描  述: 解释器模块，连接用户界面和下层API模块

** 功  能：将用户的输入命令进行语法分析和语义解析并得到需要的命令参数,
           最后将该命令参数封装成对应命令的参数类对象,传回到 API 模块

** 版  本: 1.00

***********************************************************************************/
#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "../RECORD/Record.h"
#include "../APILIB/APILIB.h"
// 打印命令行窗口，使底层实现和GUI分离，便于扩展
#define PRINTLENGTH 63
class PrintWindow
{
public:
	void CreateTable(bool is_created);
	void ShowAllTable(std::vector<std::string> sen_str, std::string path);
	void DropTable(bool is_dropped);
	void SelectTable(SelectPrintInfo select_table_print_info);
	void InsertRecord(bool is_inserted);
	void CreateDB(bool is_created);
	void DropDB(bool is_dropped);
	void ShowDB(std::vector<std::string> db_names);
	void UseDB(bool isUsed);
	void UpdateTable(bool isUpdated);
	void DeleteTable(bool isDeleted);
private:
	void Print(int len, std::string s); // 打印 |xxxx        | 其中竖线内长度为 len
	int GetColumnLength(std::string name,std::vector<std::string> col_name, std::vector<int> col_len);
	
};



// 创建数据库,返回要创建的名称
std::string CreateDbInfo(std::vector<std::string> sen_str);

// 删除数据库,返回要删除的名称
std::string DeleteDbInfo(std::vector<std::string> sen_str);

// 使用数据库
std::string UseDbInfo(std::vector<std::string> sen_str);

// 显示数据库
std::string ShowDbInfo(std::vector<std::string> sen_str);
// 生成表相关信息
bool CreateShowTableInfo(std::vector<std::string> sen_str);
TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str);
TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str);
std::string DropTableInfo(std::vector<std::string> sen_str);
TB_Select_Info TableSelectInfo(std::vector<std::string> sen_str);  //生成select操作所需的信息
TB_Update_Info TableUpdateInfo(std::vector<std::string> sen_str);
TB_Delete_Info TableDeleteInfo(std::vector<std::string> sen_str);
//TB_Update_Info
// 返回有意字串的操作类型,同时做类型检查
CmdType GetOpType(std::vector<std::string> sen_str);

void Interpreter(std::vector<std::string> sen_str, CmdType cmd_type, PrintWindow print_window);
#endif //__INTERPRETER_H__
