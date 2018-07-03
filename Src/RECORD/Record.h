/***********************************************************************************

** 文件名: Record.h

** Copyright (c) 

** 创建人: ReFantasy/TDL

** 日  期: 2018-06-22

** 描  述: 定义记录模块所需的数据结构以及操作

** 版  本: 1.00

** 其  他：暴雨天 照逛街 偷笑别人花了脸  ---- 歌曲《 素颜 》

***********************************************************************************/

#ifndef __RECORD_H__
#define __RECORD_H__
#include <tuple>
#include "../Src/ERROR/Error.h"
#include "../Src/BUFFER/Buffer.h"

/***********************************************************************************
*
*    定义记录各个字段的类型
*    字段类型 I---int  C---字符串  D---Doouble
*
***********************************************************************************/
enum class Column_Type { I, C, D };

// 数据类型的字符串形式转换为枚举类型
Column_Type StrConvertToEnumType(std::string str_type);




/***********************************************************************************
*
*    联合的数据结构定义 字段的值
*
***********************************************************************************/
union Column_Value
{
	int   		        IntValue;		 //整形值
	double 		        DoubleValue;     //浮点型值
	char*               StrValue;	     //字符串指针 
};


/***********************************************************************************
*
*    定义索引文件关键字属性
*
***********************************************************************************/
class KeyAttr
{
public:
	using Key_Value = union {
		char                StrValue[ColumnNameLength];	     //字符串指针 
		int   		        IntValue;		 //整形值
		double 		        DoubleValue;     //浮点型值	
	};
	Column_Type type;
	Key_Value value;

	bool operator<(const KeyAttr &rhs)const;
	bool operator>(const KeyAttr &rhs)const;
	bool operator==(const KeyAttr &rhs)const;
	bool operator<=(const KeyAttr &rhs)const;
	bool operator>=(const KeyAttr &rhs)const;
	bool operator!=(const KeyAttr &rhs)const;

};
std::ostream& operator<<(std::ostream &os, const KeyAttr &key);

/***********************************************************************************
*
*    定义每个字段的单元数据
*    字段单元记录了该字段的数据类型、数据的值、以及该字段的下一个字段指针
*    注意拷贝和赋值操作
*
***********************************************************************************/

class Column_Cell
{
public:
	Column_Cell() { memset(&column_value, 0, sizeof(column_value)); }
	Column_Cell(KeyAttr key);
	Column_Cell(const Column_Cell& rhs); // 拷贝构造
	Column_Cell& operator=(const Column_Cell&rhs); // 拷贝赋值

	Column_Cell(Column_Cell&& rhs); // 移动构造
	Column_Cell& operator=(Column_Cell&&rhs); // 移动赋值

	size_t size()const;
	void* data()const;
	~Column_Cell();

	Column_Type column_type;
	std::string columu_name;
	Column_Value column_value;
	Column_Cell *next;
	// 类型转换
	operator KeyAttr()const;
};


/***********************************************************************************
*
*    定义一条记录的头结构，唯一标志一条记录
*    记录的头记录了该记录的第一个字段的地址
*    记录各个字段以链表的形式形成整条记录
*
***********************************************************************************/
class RecordHead
{
public:
	RecordHead();
	RecordHead(const RecordHead &rhs); // 拷贝构造
	RecordHead& operator=(const RecordHead&rhs);  // 拷贝赋值
	~RecordHead();
	void AddColumnCell(const Column_Cell &cc);

	size_t size()const;  // 返回整条记录的大小
    Column_Cell* GetFirstColumn()const;
	
private:
	Column_Cell *phead;  // 指向记录的第一个字段
	Column_Cell *pLast;
	void *data;
};





/***********************************************************************************
*
*    记录类:  定义所有的记录数据操作。
*    注意:    记录读写包括索引部分(.idx)和数据部分(.dbf)。
*             索引部分的修改由B+树对应的模块负责
*             本模块的所有操作只修改记录的实际数据部分。
*
*    说明：   本类所有的记录均以文件地址唯一标志。删除和查找记录的地址由索引树提供
*
***********************************************************************************/
class Record
{
public:
	// 插入新的记录，返回新插入记录的所在数据文件的地址
	FileAddr InsertRecord(const std::string dbf_name, const RecordHead &rd);

	// 删除记录，返回删除的记录所在数据文件的地址
	FileAddr DeleteRecord(const std::string dbf_name, FileAddr fd, size_t);

	// 更新整条记录
	bool UpdateRecord(const std::string dbf_name, const RecordHead &rd, FileAddr fd);   

private:

	// 将以链表的形式传入的记录数据读取并保存为一个整数据块以便于写入数据文件
	// tuple的第一个元素为记录数据的大小，第二个元素为数据的指针
	std::tuple<unsigned long, char*> GetRecordData(const RecordHead &rd);
};


#endif //__RECORD_H__
