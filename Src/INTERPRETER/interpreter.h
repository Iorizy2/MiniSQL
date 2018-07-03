/***********************************************************************************

** �ļ���: interpreter.h

** Copyright (c)

** ������: ReFantasy/TDL

** ��  ��: 2018-06-27

** ��  ��: ������ģ�飬�����û�������²�APIģ��

** ��  �ܣ����û���������������﷨����������������õ���Ҫ���������,
           ��󽫸����������װ�ɶ�Ӧ����Ĳ��������,���ص� API ģ��

** ��  ��: 1.00

***********************************************************************************/
#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "../APILIB/APILIB.h"

// ��ӡ�����д��ڣ�ʹ�ײ�ʵ�ֺ�GUI���룬������չ
#define PRINTLENGTH 63
class PrintWindow
{
public:
	void CreateTable(bool is_created);
	void ShowAllTable(std::vector<std::string> sen_str, std::string path);
	void DropTable(bool is_dropped);
	void SelectTable(SelectPrintInfo select_table_print_info);
	void InsertRecord(bool is_inserted);
	void CreateDB(bool is_created);
	void DropDB(bool is_dropped);
	void ShowDB(std::vector<std::string> db_names);
	void UseDB(bool isUsed);
	void UpdateTable(bool isUpdated);
	void DeleteTable(bool isDeleted);
private:
	void Print(int len, std::string s); // ��ӡ |xxxx        | ���������ڳ���Ϊ len
	int GetColumnLength(std::string name,std::vector<std::string> col_name, std::vector<int> col_len);
	
};

// ������Ľ����ӿ� ���룺���������GUI�� ����APILIB
void Interpreter(std::vector<std::string> sen_str, CmdType cmd_type, PrintWindow print_window); 


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
	SensefulStr(std::string srcstr = "")
		:src_str(srcstr)
	{
		Parse();
	}
	void SetSrcStr(std::string _srcstr)
	{
		src_str = _srcstr;
		sen_str.clear();
		Parse();
	}

	std::vector<std::string> GetSensefulStr()const
	{
		return sen_str;
	}
private:
	// ��������Ϊ�����ִ�
	void Parse()
	{
		int i = 0;
		std::string token;
		while (i < src_str.size())
		{
			// ���жϱ�ʶ��
			if (src_str[i] == ' ')
			{
				if (!token.empty())
					sen_str.push_back(token);
				token.clear();
				i++;
				continue;
			}
			if (src_str[i] == '\n')
			{
				i++;
				continue;
			}

			else if (src_str[i] == ',' || src_str[i] == '(' || src_str[i] == ')')// || src_str[i] == '=')
			{
				if (!token.empty())
					sen_str.push_back(token);
				token.clear();

				sen_str.push_back(std::string() + src_str[i]);
				i++;
				continue;
			}
			else if (src_str[i] == ';')
			{
				if (!token.empty())
					sen_str.push_back(token);
				token.clear();

				sen_str.push_back(";");
				break;
			}

			token += src_str[i++];
		}
	}

	std::string src_str;  // ԭʼ�����ַ���
	std::vector<std::string> sen_str; // ���������һ�ִ�
};

// ���������ִ��Ĳ�������,ͬʱ�����ͼ��
CmdType GetOpType(std::vector<std::string> sen_str);

// ��ʾ���ݿ�
std::string ShowDbInfo(std::vector<std::string> sen_str);

// �������ݿ�,����Ҫ����������
std::string CreateDbInfo(std::vector<std::string> sen_str);

// ɾ�����ݿ�,����Ҫɾ��������
std::string DeleteDbInfo(std::vector<std::string> sen_str);

// ʹ�����ݿ�
std::string UseDbInfo(std::vector<std::string> sen_str);


// ����������Ϣ
bool CreateShowTableInfo(std::vector<std::string> sen_str);
TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str);
TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str);
std::string DropTableInfo(std::vector<std::string> sen_str);
TB_Select_Info TableSelectInfo(std::vector<std::string> sen_str);  //����select�����������Ϣ
TB_Update_Info TableUpdateInfo(std::vector<std::string> sen_str);
TB_Delete_Info TableDeleteInfo(std::vector<std::string> sen_str);




#endif //__INTERPRETER_H__
