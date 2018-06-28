/****************************************************************************************************************************************

** 文件名: global.h

** Copyright (c)

** 创建人: ReFantasy/TDL

** 日  期: 2018-06-23

** 描  述: 定义全局常量和数据结构

** 版  本: 1.00

****************************************************************************************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "../Src/ERROR/error.h"
#include <vector>
#include <string>

/***********************************************************************************
*
*    定义记录各个字段的类型
*    字段类型 I---int  C---字符串  D---Doouble
*
***********************************************************************************/

enum class Column_Type { I, C, D };

// 数据类型的字符串形式转换为枚举类型
Column_Type StrConvertToEnumType(std::string str_type);

/********************************************************  Buffer Module  ***************************************************************/

constexpr int FILE_PAGESIZE = 8192;	// 内存页(==文件页)大小
constexpr int MEM_PAGEAMOUNT = 4096;	// 内存页数量
constexpr int MAX_FILENAME_LEN = 256;	// 文件名（包含路径）最大长度

/****************************************************************************************************************************************/





/********************************************************  B+tree Module  ***************************************************************/

constexpr int RecordColumnCount = 12 * 4;  // 记录字段数量限制,假设所有字段都是字符数组，一个字符数组字段需要4个字符->CXXX
constexpr int ColumnNameLength = 20;     // 单个字段名称长度限制
constexpr int bptree_t = 3;                         // B+tree's degree, bptree_t >= 2
constexpr int MaxKeyCount = 2 * bptree_t;            // the max number of keys in a b+tree node
constexpr int MaxChildCount = 2 * bptree_t;          // the max number of child in a b+tree node

/****************************************************************************************************************************************/


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


// file name convert .idx to .dbf 
std::string IdxToDbf(std::string idx_name);
// file name convert .dbf to .idx 
std::string DbfToIdx(std::string idx_name);

// str to int
int StrToInt(std::string str);

std::string StrToLower(std::string str);

#endif
