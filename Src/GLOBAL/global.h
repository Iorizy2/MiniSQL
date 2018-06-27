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
#include <string>

/***********************************************************************************
*
*    �����¼�����ֶε�����
*    �ֶ����� I---int  C---�ַ���  D---Doouble
*
***********************************************************************************/

enum class Column_Type { I, C, D };

// �������͵��ַ�����ʽת��Ϊö������
Column_Type StrConvertToEnumType(std::string str_type);

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


// file name convert .idx to .dbf 
std::string IdxToDbf(std::string idx_name);
// file name convert .dbf to .idx 
std::string DbfToIdx(std::string idx_name);

// str to int
int StrToInt(std::string str);

std::string StrToLower(std::string str);

#endif
