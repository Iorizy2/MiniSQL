/***********************************************************************************

** �ļ���: interpreter.h

** Copyright (c)

** ������: ReFantasy/TDL

** ��  ��: 2018-06-27

** ��  ��: ������ģ��

** ��  �ܣ����û���������������﷨����������������õ���Ҫ���������,
          ��󽫸����������װ�ɶ�Ӧ����Ĳ��������,���ص� APILIB ģ��

** ��  ��: 1.00

***********************************************************************************/
#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <iostream>
#include <string>
#include <vector>
#include "../RECORD/Record.h"

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


void GenerateParameterObj(std::vector<std::string> sen_str);

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
TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str);


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
TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str);


#endif //__INTERPRETER_H__
