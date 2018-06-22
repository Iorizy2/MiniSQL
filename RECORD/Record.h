/******************************************************************

** 文件名: Record.h

** Copyright (c) 

** 创建人: ReFantasy/TDL

** 日  期: 2018-06-22

** 描  述: 定义记录模块所需的数据结构以及操作

** 版  本: 1.00

******************************************************************/

#ifndef __RECORD_H__
#define __RECORD_H__
#include "../BUFFER/Buffer.h"
#include <string>
#include <vector>
#include <tuple>
#include "../ERROR/Error.h"

// 字段类型 I---int  C---char   D---Doouble
enum class Column_Type { I, C, D };
// 字段的值
union Column_Value
{
	int   		        IntValue;		 //整形值
	double 		        DoubleValue;	     //浮点型值
	char                *StrValue;	     //字符串指针 
};

// 字段的单元信息
class Column_Cell
{
public:
	Column_Cell()=default;
	size_t size()const;
	void* data()const;
	~Column_Cell();

	Column_Type column_type;
	std::string columu_name;
	Column_Value column_value;
	Column_Cell *next;
};

class RecordHead
{
public:
	RecordHead();
	~RecordHead();
	void AddColumnCell(const Column_Cell &cc);

	size_t size()const;  // 返回整条记录的大小
	const Column_Cell* GetFirstColumn()const;
	
private:
	Column_Cell *phead;  // 指向记录的第一个字段
	Column_Cell *pLast;
	void *data;
};

// 添加删除记录只添加或删除数据文件 dbf, 索引文件的修改由 B+树 模块负责
class Record
{
public:
	FileAddr InsertRecord(const std::string dbf_name, const RecordHead &rd);
	FileAddr DeleteRecord(const std::string dbf_name, FileAddr fd, size_t record_size);
	bool UpdateRecord(const std::string dbf_name, const RecordHead &rd, FileAddr fd);   // 更新记录
private:
	std::tuple<unsigned long, char*> GetRecordData(const RecordHead &rd);
};


#endif //__RECORD_H__
