/***********************************************************************************

** 文件名: interpreter.h

** Copyright (c)

** 创建人: ReFantasy/TDL

** 日  期: 2018-06-27

** 描  述: 解释器模块，连接用户界面和下层API模块

** 功  能：将用户的输入命令进行语法分析和语义解析并得到需要的命令参数,
          最后将该命令参数封装成对应命令的参数类对象,传回到 APILIB 模块

** 版  本: 1.00

***********************************************************************************/
#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <iostream>
#include <string>
#include <vector>
#include "../RECORD/Record.h"
#include "../BUFFER/Buffer.h"

void GenerateParameterObj(std::vector<std::string> sen_str);

// 根据有意字串创建相关命令信息
TB_Create_Info CreateTableInfo(std::vector<std::string> sen_str);
TB_Insert_Info CreateInsertInfo(std::vector<std::string> sen_str);


#endif //__INTERPRETER_H__
