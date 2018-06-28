/***********************************************************************************

** �ļ���: interpreter.h

** Copyright (c)

** ������: ReFantasy/TDL

** ��  ��: 2018-06-27

** ��  ��: ������ģ�飬�����û�������²�APIģ��

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
#include "../BUFFER/Buffer.h"

void GenerateParameterObj(std::vector<std::string> sen_str);

// ���������ִ��������������Ϣ
TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str);
TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str);


#endif //__INTERPRETER_H__
