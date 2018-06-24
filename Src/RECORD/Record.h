/***********************************************************************************

** �ļ���: Record.h

** Copyright (c) 

** ������: ReFantasy/TDL

** ��  ��: 2018-06-22

** ��  ��: �����¼ģ����������ݽṹ�Լ�����

** ��  ��: 1.00

** ��  ���������� �չ�� ͵Ц���˻�����  ---- ������ ���� ��

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
*    �����¼�����ֶε�����
*    �ֶ����� I---int  C---�ַ���  D---Doouble
*
***********************************************************************************/

enum class Column_Type { I, C, D };



/***********************************************************************************
*
*    ���������ļ��ؼ�������
*
***********************************************************************************/
class KeyAttr
{
public:
	using Key_Value = union {
		int   		        IntValue;		 //����ֵ
		double 		        DoubleValue;     //������ֵ
		char                StrValue[ColumnNameLength];	     //�ַ���ָ�� 
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
*    ���ϵ����ݽṹ���� �ֶε�ֵ
*
***********************************************************************************/
union Column_Value
{
	int   		        IntValue;		 //����ֵ
	double 		        DoubleValue;     //������ֵ
	char*               StrValue;	     //�ַ���ָ�� 
};

/***********************************************************************************
*
*    ����ÿ���ֶεĵ�Ԫ����
*    �ֶε�Ԫ��¼�˸��ֶε��������͡����ݵ�ֵ���Լ����ֶε���һ���ֶ�ָ��
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

	// ����ת��
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
*    ����һ����¼��ͷ�ṹ��Ψһ��־һ����¼
*    ��¼��ͷ��¼�˸ü�¼�ĵ�һ���ֶεĵ�ַ
*    ��¼�����ֶ����������ʽ�γ�������¼
*
***********************************************************************************/
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




/***********************************************************************************
*
*    ��¼��:  �������еļ�¼���ݲ�����
*    ע��:    ��¼��д������������(.idx)�����ݲ���(.dbf)��
*             �������ֵ��޸���B+����Ӧ��ģ�鸺��
*             ��ģ������в���ֻ�޸ļ�¼��ʵ�����ݲ��֡�
*
*    ˵����   �������еļ�¼�����ļ���ַΨһ��־��ɾ���Ͳ��Ҽ�¼�ĵ�ַ���������ṩ
*
***********************************************************************************/
class Record
{
public:
	// �����µļ�¼�������²����¼�����������ļ��ĵ�ַ
	FileAddr InsertRecord(const std::string dbf_name, const RecordHead &rd);

	// ɾ����¼������ɾ���ļ�¼���������ļ��ĵ�ַ
	FileAddr DeleteRecord(const std::string dbf_name, FileAddr fd, size_t record_size = 0);

	// ����������¼
	bool UpdateRecord(const std::string dbf_name, const RecordHead &rd, FileAddr fd);   

	// �����ض��ֶ�

private:

	// �����������ʽ����ļ�¼���ݶ�ȡ������Ϊһ�������ݿ��Ա���д�������ļ�
	// tuple�ĵ�һ��Ԫ��Ϊ��¼���ݵĴ�С���ڶ���Ԫ��Ϊ���ݵ�ָ��
	std::tuple<unsigned long, char*> GetRecordData(const RecordHead &rd);
};


#endif //__RECORD_H__
