/******************************************************************

** �ļ���: Record.h

** Copyright (c) 

** ������: ReFantasy/TDL

** ��  ��: 2018-06-22

** ��  ��: �����¼ģ����������ݽṹ�Լ�����

** ��  ��: 1.00

******************************************************************/

#ifndef __RECORD_H__
#define __RECORD_H__
#include "../BUFFER/Buffer.h"
#include <string>
#include <vector>
#include <tuple>
#include "../ERROR/Error.h"

// �ֶ����� I---int  C---char   D---Doouble
enum class Column_Type { I, C, D };
// �ֶε�ֵ
union Column_Value
{
	int   		        IntValue;		 //����ֵ
	double 		        DoubleValue;	     //������ֵ
	char                *StrValue;	     //�ַ���ָ�� 
};

// �ֶεĵ�Ԫ��Ϣ
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

	size_t size()const;  // ����������¼�Ĵ�С
	const Column_Cell* GetFirstColumn()const;
	
private:
	Column_Cell *phead;  // ָ���¼�ĵ�һ���ֶ�
	Column_Cell *pLast;
	void *data;
};

// ���ɾ����¼ֻ��ӻ�ɾ�������ļ� dbf, �����ļ����޸��� B+�� ģ�鸺��
class Record
{
public:
	FileAddr InsertRecord(const std::string dbf_name, const RecordHead &rd);
	FileAddr DeleteRecord(const std::string dbf_name, FileAddr fd, size_t record_size);
	bool UpdateRecord(const std::string dbf_name, const RecordHead &rd, FileAddr fd);   // ���¼�¼
private:
	std::tuple<unsigned long, char*> GetRecordData(const RecordHead &rd);
};


#endif //__RECORD_H__
