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
#include <string>
#include <vector>
#include <tuple>
#include "../Src/GLOBAL/global.h"
#include "../Src/BUFFER/Buffer.h"
#include "../Src/ERROR/Error.h"

/***********************************************************************************
*
*    定义记录各个字段的类型
*    字段类型 I---int  C---字符串  D---Doouble
*
***********************************************************************************/

enum class Column_Type { I, C, D };



/***********************************************************************************
*
*    定义索引文件关键字属性
*
***********************************************************************************/
class KeyAttr
{
public:
	using Key_Value = union {
		int   		        IntValue;		 //整形值
		double 		        DoubleValue;     //浮点型值
		char                StrValue[ColumnNameLength];	     //字符串指针 
	};
	Column_Type type;
	Key_Value value;

	bool operator<(const KeyAttr &rhs);
	bool operator>(const KeyAttr &rhs);
	bool operator==(const KeyAttr &rhs);
	bool operator<=(const KeyAttr &rhs);
	bool operator>=(const KeyAttr &rhs);
	bool operator!=(const KeyAttr &rhs);
};
std::ostream& operator<<(std::ostream &os, const KeyAttr &key);

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
*    定义每个字段的单元数据
*    字段单元记录了该字段的数据类型、数据的值、以及该字段的下一个字段指针
*
***********************************************************************************/

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
	Column_Cell& operator=(const Column_Cell&rhs);

	// 类型转换
	operator KeyAttr()const
	{
		KeyAttr key_attr;

		switch (column_type)
		{
		case Column_Type::I:
			key_attr.type = column_type;
			key_attr.value.IntValue = column_value.IntValue;
			break;

		case Column_Type::C:
			key_attr.type = column_type;
			if (strlen(column_value.StrValue) > ColumnNameLength)
			{
				throw SQLError::KeyAttr_NameLength_ERROR();
			}
			else
			{
				strcpy(key_attr.value.StrValue, column_value.StrValue);
			}
			break;

		case Column_Type::D:
			key_attr.type = column_type;
			key_attr.value.DoubleValue = column_value.DoubleValue;
			break;
		default:
			break;
		}

		return key_attr;
	}
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
	~RecordHead();
	void AddColumnCell(const Column_Cell &cc);

	size_t size()const;  // 返回整条记录的大小
	const Column_Cell* GetFirstColumn()const;
	
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
	FileAddr DeleteRecord(const std::string dbf_name, FileAddr fd, size_t record_size = 0);

	// 更新整条记录
	bool UpdateRecord(const std::string dbf_name, const RecordHead &rd, FileAddr fd);   

	// 更新特定字段

private:

	// 将以链表的形式传入的记录数据读取并保存为一个整数据块以便于写入数据文件
	// tuple的第一个元素为记录数据的大小，第二个元素为数据的指针
	std::tuple<unsigned long, char*> GetRecordData(const RecordHead &rd);
};


#endif //__RECORD_H__
