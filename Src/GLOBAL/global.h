/****************************************************************************************************************************************

** �ļ���: global.h

** Copyright (c)

** ������: ReFantasy/TDL

** ��  ��: 2018-06-23

** ��  ��: ����ȫ�ֳ��������ݽṹ

** ��  ��: 1.00

****************************************************************************************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <iostream>
#include <vector>
#include <string>
#include <direct.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
using namespace std::chrono;
//#define NDEBUG

#include "../Src/ERROR/error.h"
extern "C"
{
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
}

/***********************************************************************************
*
*    �����¼�����ֶε�����
*    �ֶ����� I---int  C---�ַ���  D---Doouble
*
***********************************************************************************/

enum class Column_Type { I, C, D };

/********************************************************  Buffer Module  ***************************************************************/

constexpr int FILE_PAGESIZE = 8192;	// �ڴ�ҳ(==�ļ�ҳ)��С
constexpr int MEM_PAGEAMOUNT = 4096;	// �ڴ�ҳ����
constexpr int MAX_FILENAME_LEN = 256;	// �ļ���������·������󳤶�

/****************************************************************************************************************************************/





/********************************************************  B+tree Module  ***************************************************************/

constexpr int RecordColumnCount = 12 * 4;  // ��¼�ֶ���������,���������ֶζ����ַ����飬һ���ַ������ֶ���Ҫ4���ַ�->CXXX
constexpr int ColumnNameLength = 16;     // �����ֶ����Ƴ�������
constexpr int bptree_t = 3;                         // B+tree's degree, bptree_t >= 2
constexpr int MaxKeyCount = 2 * bptree_t;            // the max number of keys in a b+tree node
constexpr int MaxChildCount = 2 * bptree_t;          // the max number of child in a b+tree node

/***********************************************************************************
*
*    ���������ļ��ؼ�������
*
***********************************************************************************/
class KeyAttr
{
public:
	using Key_Value = union {
		char                StrValue[ColumnNameLength];	     //�ַ���ָ�� 
		int   		        IntValue;		 //����ֵ
		double 		        DoubleValue;     //������ֵ	
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

// �������͵��ַ�����ʽת��Ϊö������
Column_Type StrConvertToEnumType(std::string str_type);
enum class CmdType
{
	TABLE_CREATE, TABLE_DROP, TABLE_SHOW, TABLE_SELECT, TABLE_INSERT, TABLE_UPDATE, TABLE_DELETE,
	DB_CREATE, DB_DROP, DB_SHOW, DB_USE,
	QUIT,HELP

};

/*********************************************************
*             �ļ���ַ,��λ�ļ��е�λ��
**********************************************************/
class FileAddr
{
	friend class FILECOND;
public:
	void SetFileAddr(const unsigned long _filePageID, const unsigned int  _offSet);
	void ShiftOffset(const int OFFSET);

	unsigned long filePageID;     // �ļ�ҳ���
	unsigned int  offSet;         // ҳ��ƫ����

	bool operator==(const FileAddr &rhs) const
	{
		return (this->filePageID == rhs.filePageID && this->offSet == rhs.offSet);
	}
	bool operator!=(const FileAddr &rhs) const
	{
		return !(this->filePageID == rhs.filePageID && this->offSet == rhs.offSet);
	}
	bool operator<(const FileAddr &rhs)const
	{
		return (this->filePageID < rhs.filePageID) || ((this->filePageID == rhs.filePageID) && (this->offSet < rhs.offSet));
	}
};





/****************************************************************************************************************************************/


/************************************************************************
*    �����������ִ���
*    ���ܣ��������ַ�������Ϊ�����ִ�
*	 �����ִ����壺�����ִ���ָһ���Իس���ո���־�����ָ���ж���������ַ�
*                 ��,��ʶ����Ҫ�ж��š����š��Ƚ���������ֺŵȡ���ʶ��Ҳ����
*                 �����ִ�
************************************************************************/
class SensefulStr
{
public:
	SensefulStr(std::string srcstr = "");
	void SetSrcStr(std::string senstr);
	std::vector<std::string> GetSensefulStr()const;
private:
	void Parse();
	std::string src_str;  // ԭʼ�����ַ���
	std::vector<std::string> sen_str; // ���������һ�ִ�
};


/************************************************************************
*    ������Ϣ
************************************************************************/
struct TB_Create_Info
{
	using ColumnInfo = struct ColumnInfo
	{
		std::string name;
		Column_Type type;
		bool isPrimary;       // �Ƿ�����
		int length;           // ���ݴ�С��ֻ�����ַ����ֶ�
	};

	std::string table_name;
	std::vector<ColumnInfo> columns_info;  //�ֶ���Ϣ����

};

/************************************************************************
*    �������Ϣ
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

class CompareCell;
struct TB_Select_Info
{
	std::string table_name;                        // ѡ��ı���
	std::vector<std::string> name_selected_column; // ѡ����ֶ�����
	std::vector<CompareCell> vec_cmp_cell;         // ѡ������
};

struct TB_Update_Info
{
	using NewValue = struct {
		std::string field;
		std::string value;
	};
	using Expr = struct {
		std::string field;
		std::string op;
		std::string value;
	};

	std::string table_name;
	std::vector<NewValue> field_value;  // �ֶΡ���ֵ ����
	std::vector<Expr> expr;             // ���µ��ֶ�����
};

struct TB_Delete_Info
{
	using Expr = struct {
		std::string field;
		std::string op;
		std::string value;
	};
	std::string table_name;
	std::vector<Expr> expr;             // ɾ�����ֶ�����
};
// Ŀ¼��λ���л� �������ݿ�ͱ��ʹ��
class CatalogPosition
{
	friend bool UseDatabase(std::string db_name, CatalogPosition &cp);
public:
	CatalogPosition();
	bool ResetRootCatalog(std::string root_new);  // ���ø�Ŀ¼

	void SwitchToDatabase();// ת�����ݿ��б�Ŀ¼��


	bool SwitchToDatabase(std::string db_name);// ת����������ݿ���

	std::string GetCurrentPath()const;
	std::string GetRootPath()const;
	std::string SetCurrentPath(std::string cur);
	bool GetIsInSpeDb() { return isInSpeDb; }
private:
	static bool isInSpeDb;          //�Ƿ���ĳ����������ݿ�Ŀ¼��
	std::string root; // ��Ŀ¼�����ݿ��ļ��ı���λ��
	std::string current_catalog;
};
CatalogPosition& GetCp();

// ����һ���������� ͳ�����ݿ�����ĺ�ʱ
class SQLTimer
{
public:
	void Start() { start_t = steady_clock::now(); }
	void Stop() { stop_t = steady_clock::now(); }

	// ���ؾ�����ʱ�䣬��λ��second
	double TimeSpan()
	{
		time_span = duration_cast<duration<double>>(stop_t - start_t);
		return time_span.count();
	}

	// ��ӡʱ��
	void PrintTimeSpan()
	{
		std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(precision) << TimeSpan() << " seconds ";
	}
private:
	steady_clock::time_point start_t;
	steady_clock::time_point stop_t;
	duration<double> time_span;
	const static unsigned int precision = 8; // ���ʱ���С��λ����
};
SQLTimer& GetTimer();  // ȫ�ּ�ʱ��


// file name convert .idx to .dbf 
std::string IdxToDbf(std::string idx_name);
// file name convert .dbf to .idx 
std::string DbfToIdx(std::string idx_name);

// str to int
int StrToInt(std::string str);

std::string StrToLower(std::string str);
std::string IntToStr3(unsigned int x); // С��1000������תΪ�����ַ����ַ���
#endif
