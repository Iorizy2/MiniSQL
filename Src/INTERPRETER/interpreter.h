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
#include "../RECORD/Record.h"
#include "../APILIB/APILIB.h"




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
// ���������ִ��Ĳ�������,ͬʱ�����ͼ��
CmdType GetOpType(std::vector<std::string> sen_str);

void Interpreter(std::vector<std::string> sen_str, CmdType cmd_type, PrintWindow print_window);
#endif //__INTERPRETER_H__
