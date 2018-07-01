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
#include "../RECORD/Record.h"
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



// �������ݿ�,����Ҫ����������
std::string CreateDbInfo(std::vector<std::string> sen_str);

// ɾ�����ݿ�,����Ҫɾ��������
std::string DeleteDbInfo(std::vector<std::string> sen_str);

// ʹ�����ݿ�
std::string UseDbInfo(std::vector<std::string> sen_str);

// ��ʾ���ݿ�
std::string ShowDbInfo(std::vector<std::string> sen_str);
// ���ɱ������Ϣ
bool CreateShowTableInfo(std::vector<std::string> sen_str);
TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str);
TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str);
std::string DropTableInfo(std::vector<std::string> sen_str);
TB_Select_Info TableSelectInfo(std::vector<std::string> sen_str);  //����select�����������Ϣ
TB_Update_Info TableUpdateInfo(std::vector<std::string> sen_str);
TB_Delete_Info TableDeleteInfo(std::vector<std::string> sen_str);
//TB_Update_Info
// ���������ִ��Ĳ�������,ͬʱ�����ͼ��
CmdType GetOpType(std::vector<std::string> sen_str);

void Interpreter(std::vector<std::string> sen_str, CmdType cmd_type, PrintWindow print_window);
#endif //__INTERPRETER_H__
