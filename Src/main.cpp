#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "GLOBAL/global.h"
#include "ERROR/Error.h"
#include "BUFFER/Buffer.h"
#include "BPLUSTREE/bptree.h"
#include "RECORD/Record.h"
#include "INTERPRETER/interpreter.h"
#include "APILIB/APILIB.h"
#include <time.h>
#include<direct.h>

using namespace std;
const std::string PROMPT = "MiniSQL:";

void IsPod(); // 判断POD数据
void MySleep(unsigned int n); // 程序睡眠

// 读取用户的输入，以 ";"结束
std::string GetCommand();
void Help();
void InitMiniSQL();
void RunMiniSQL();

int main()
{
	// 初始化
	InitMiniSQL();

	// 运行数据库
	RunMiniSQL();  

	// 退出程序
	cout << "bye." << endl;
	MySleep(1);
	return 0;
}

std::string GetCommand()
{
	std::string res;
	std::string tmp;
	int n = 0;
	do	{
		if (n == 0) {
			cout << PROMPT;
		}
		else {
			cout << "        ";
		}
		n++;
		getline(cin, tmp);
		res += tmp;
	} while (tmp[tmp.size() - 1] != ';');
	return res;
}
void MySleep(unsigned int n)
{
	auto t1 = time(0);
	time_t t2 = t1;
	while ((t2 - t1) < n)
	{
		t2 = time(0);
	}
}
void IsPod()
{
	cout << std::is_pod<PAGEHEAD>::value << endl;
	cout << std::is_pod<FileAddr>::value << endl;
	cout << std::is_pod<FILECOND>::value << endl;
	cout << std::is_pod<BTNode>::value << endl;
	cout << std::is_pod<Column_Value>::value << endl;
	cout << std::is_pod<KeyAttr>::value << endl; 
    //cout << std::is_pod<CondtionInfo>::value << endl;
}


void Help()
{
	std::cout << "+---------------------------------------------------------------------------------------------+" << std::endl;
	std::cout << "|A simple example to create a student databae named STU                                       |" << std::endl;
	std::cout << "+---------------------------------------------------------------------------------------------+" << std::endl;
	std::cout << "|Create database :create database STU;                                                        |" << std::endl;
	std::cout << "|Use database    :use database STU;                                                           |" << std::endl;
	std::cout << "|Show database   :show databases;                                                             |" << std::endl;
	std::cout << "|Create Table    :create table student(id int primary, socre double, name char(20));          |" << std::endl;
	std::cout << "|Insert Record(1):insert into student(id,score,name)values(1,95.5,ZhangSan);                  |" << std::endl;
	std::cout << "|Insert Record(2):insert into student(id,name)values(2,LiSi); Note:LiSi has no score          |" << std::endl; 
	std::cout << "|UPDATE Table    :update student set score = 96.5 where name = LiSi;                          |" << std::endl;
	std::cout << "|Delete Table    :delete from student where id = 1; Note: ZhangSan is deleted                 |" << std::endl;
	std::cout << "|Select Table(1) :select * from student where id = 2;                                         |" << std::endl;
	std::cout << "|Select Table(2) :select * from student where id > 1 and score < 98;                          |" << std::endl;
	std::cout << "|Select Table(3) :select id,score from student where id > 1 and score < 98;                   |" << std::endl;
	std::cout << "|Drop database   :drop database STU;                                                          |" << std::endl;
	std::cout << "|Quit   :quit;                                                                                |" << std::endl;
	std::cout << "+---------------------------------------------------------------------------------------------+" << std::endl;
}


void InitMiniSQL()
{
	std::cout << "                  WELCOME TO USE MY MINISQL!" << std::endl;
	std::cout << "+---------------------------------------------------------------+" << std::endl;
	std::cout << "|Declare: It is just a test version without any error process.  |" << std::endl;
	std::cout << "|         So you should use it take care.                       |" << std::endl;
	std::cout << "|                                                               |" << std::endl;
	std::cout << "|Usage  : You can typing the help cammand to get a help.        |" << std::endl;
	std::cout << "|         More details @ https://github.com/ReFantasy/MiniSQL   |" << std::endl;
	std::cout << "|                                                               |" << std::endl;
	std::cout << "|Contact: ReFantasy.cn                                          |" << std::endl;
	std::cout << "|                                                               |" << std::endl;
	std::cout << "|Copyright(c) by TDL/ReFantasy.All rights reserved.             |" << std::endl;
	std::cout << "+---------------------------------------------------------------+" << std::endl;
}


void RunMiniSQL()
{
	SensefulStr senstr;
	PrintWindow print_window;
	while (true)
	{
		try
		{
			std::string cmd = GetCommand();
			senstr.SetSrcStr(cmd);
			auto cmd_type = GetOpType(senstr.GetSensefulStr());

			if (cmd_type == CmdType::QUIT)break;
			if (cmd_type == CmdType::HELP)
			{
				Help();
				continue;
			}

			Interpreter(senstr.GetSensefulStr(), cmd_type, print_window);
		}
		catch (SQLError::BaseError &e)
		{
			SQLError::DispatchError(e);
			cout << endl;
			continue;
		}

	}
}
