/***********************************************************************************

** 文件名: interpreter.h

** Copyright (c)

** 创建人: ReFantasy/TDL

** 日  期: 2018-06-27

** 描  述: 解释器模块

** 功  能：将用户的输入命令进行语法分析和语义解析并得到需要的命令参数,
          最后将该命令参数封装成对应命令的参数类对象,传回到 APILIB 模块

** 版  本: 1.00

***********************************************************************************/
#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <iostream>
#include <string>
#include <vector>
#include "../RECORD/Record.h"

/************************************************************************
*    类名：有意字串类                                                   
*    功能：将命令字符串解析为有意字串                                     
*	 有意字串定义：有意字串即指一个以回车或空格或标志符来分割的有独立含义的字符
*                 串,标识符主要有逗号、括号、比较运算符、分号等。标识符也算作
*                 有意字串
************************************************************************/
class SensefulStr
{
public:
	SensefulStr(std::string srcstr = "");
	void SetSrcStr(std::string senstr);
	std::vector<std::string> GetSensefulStr()const;
private:
	void Parse();
	std::string src_str;  // 原始命令字符串
	std::vector<std::string> sen_str; // 解析后的又一字串
};


void GenerateParameterObj(std::vector<std::string> sen_str);

/************************************************************************
*    表创建信息
************************************************************************/
struct TB_Create_Info
{
	using ColumnInfo = struct ColumnInfo
	{
		std::string name;
		Column_Type type;
		bool isPrimary;       // 是否主键
		int length;           // 数据大小，只用于字符串字段
	};

	std::string table_name;
	std::vector<ColumnInfo> columns_info;  //字段信息向量

};
TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str);


/************************************************************************
*    表插入信息
************************************************************************/
struct TB_Insert_Info
{
	using InsertInfo = struct {
		std::string column_name;
		std::string column_value;
	};

	std::string table_name;
	std::vector<InsertInfo> insert_info;
};
TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str);


#endif //__INTERPRETER_H__
