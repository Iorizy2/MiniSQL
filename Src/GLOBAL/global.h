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
#include "../Src/ERROR/error.h"
#include <vector>
#include <string>
#include <direct.h>
#include <algorithm>

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

// �������͵��ַ�����ʽת��Ϊö������
Column_Type StrConvertToEnumType(std::string str_type);
enum class CmdType
{
	TABLE_CREATE, TABLE_DROP, TABLE_SHOW, TABLE_SELECT, TABLE_INSERT, TABLE_UPDATE, TABLE_DELETE,
	DB_CREATE, DB_DROP, DB_SHOW, DB_USE
};

// ��ӡ�����д��ڣ�ʹ�ײ�ʵ�ֺ�GUI���룬������չ
class PrintWindow
{
public:
	void CreateTable(bool is_created);
	void ShowAllTable(std::vector<std::string> sen_str,std::string path);
	void CreateDB(bool is_created);
	void DropDB(bool is_dropped);
	void ShowDB(std::vector<std::string> db_names);
	void UseDB(bool isUsed);
};


/********************************************************  Buffer Module  ***************************************************************/

constexpr int FILE_PAGESIZE = 8192;	// �ڴ�ҳ(==�ļ�ҳ)��С
constexpr int MEM_PAGEAMOUNT = 4096;	// �ڴ�ҳ����
constexpr int MAX_FILENAME_LEN = 256;	// �ļ���������·������󳤶�

/****************************************************************************************************************************************/





/********************************************************  B+tree Module  ***************************************************************/

constexpr int RecordColumnCount = 12 * 4;  // ��¼�ֶ���������,���������ֶζ����ַ����飬һ���ַ������ֶ���Ҫ4���ַ�->CXXX
constexpr int ColumnNameLength = 20;     // �����ֶ����Ƴ�������
constexpr int bptree_t = 3;                         // B+tree's degree, bptree_t >= 2
constexpr int MaxKeyCount = 2 * bptree_t;            // the max number of keys in a b+tree node
constexpr int MaxChildCount = 2 * bptree_t;          // the max number of child in a b+tree node

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


// file name convert .idx to .dbf 
std::string IdxToDbf(std::string idx_name);
// file name convert .dbf to .idx 
std::string DbfToIdx(std::string idx_name);

// str to int
int StrToInt(std::string str);

std::string StrToLower(std::string str);

#endif
