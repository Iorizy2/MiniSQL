#include <iostream>
#include "./INTERPRETER/interpreter.h"

void Help();
void InitMiniSQL();
void RunMiniSQL();
void MySleep(unsigned int n = 1); // 程序睡眠
void IsPod(); // 判断POD数据
std::string GetCommand(); // 读取用户的输入，以 ";"结束
const std::string PROMPT = "MiniSQL:";

int main()
{
	// Initialize DB
	InitMiniSQL();

	// Run DB
	RunMiniSQL();  

	// Exit DB
	std::cout << "bye." << std::endl;

	MySleep();
	return 0;
}

void InitMiniSQL()
{
	std::cout << R"(                   WELCOME TO USE MY MINISQL!"                      )" << std::endl;
	std::cout << R"(+------------------------------------------------------------------+)" << std::endl;
	std::cout << R"(|Declare: It is just a test version without any error process.     |)" << std::endl;
	std::cout << R"(|         So you should use it take care.                          |)" << std::endl;
	std::cout << R"(|                                                                  |)" << std::endl;
	std::cout << R"(|Usage  : You can typing the "help;" cammand to get a help.        |)" << std::endl;
	std::cout << R"(|         More details @ https://github.com/ReFantasy/MiniSQL      |)" << std::endl;
	std::cout << R"(|                                                                  |)" << std::endl;
	std::cout << R"(|Contact: ReFantasy.cn                                             |)" << std::endl;
	std::cout << R"(|                                                                  |)" << std::endl;
	std::cout << R"(|Version: Beta 1.0                                                 |)" << std::endl;
	std::cout << R"(|                                                                  |)" << std::endl;
	std::cout << R"(|Copyright(c) by TDL/ReFantasy.All rights reserved.                |)" << std::endl;
	std::cout << R"(+------------------------------------------------------------------+)" << std::endl;

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
			std::cout << std::endl;
			continue;
		}

	}
}

void IsPod()
{
	std::cout << std::is_pod<PAGEHEAD>::value << std::endl;
	std::cout << std::is_pod<FileAddr>::value << std::endl;
	std::cout << std::is_pod<FILECOND>::value << std::endl;
	std::cout << std::is_pod<BTNode>::value << std::endl;
	std::cout << std::is_pod<Column_Value>::value << std::endl;
	std::cout << std::is_pod<KeyAttr>::value << std::endl;
}


void Help()
{
	std::cout << R"(+------------------------------------------------------------------------------------------------+)" << std::endl;
	std::cout << R"(|A simple example to create a student databae named STU                                          |)" << std::endl;
	std::cout << R"(+------------------------------------------------------------------------------------------------+)" << std::endl;
	std::cout << R"(|Create database  : create database STU;                                                         |)" << std::endl;
	std::cout << R"(|Use database     : use database STU;                                                            |)" << std::endl;
	std::cout << R"(|Show database    : show databases;                                                              |)" << std::endl;
	std::cout << R"(|Create Table     : create table student(id int primary, socre double, name char(20));           |)" << std::endl;
	std::cout << R"(|Insert Record(1) : insert into student(id,score,name)values(1,95.5,ZhangSan);                   |)" << std::endl;
	std::cout << R"(|Insert Record(2) : insert into student(id,name)values(2,LiSi); Note:LiSi has no score           |)" << std::endl;
	std::cout << R"(|UPDATE Table     : update student set score = 96.5 where name = LiSi;                           |)" << std::endl;
	std::cout << R"(|Delete Table     : delete from student where id = 1; Note: ZhangSan is deleted                  |)" << std::endl;
	std::cout << R"(|Select Table(1)  : select * from student where id = 2;                                          |)" << std::endl;
	std::cout << R"(|Select Table(2)  : select * from student where id > 1 and score < 98;                           |)" << std::endl;
	std::cout << R"(|Select Table(3)  : select id,score from student where id > 1 and score < 98;                    |)" << std::endl;
	std::cout << R"(|Drop database    : drop database STU;                                                           |)" << std::endl;
	std::cout << R"(|Quit             : quit;                                                                        |)" << std::endl;
	std::cout << R"(+------------------------------------------------------------------------------------------------+)" << std::endl;
	std::cout << R"(|Note             : Anytime you want to end MiniSQL use "quit;" command please.                  |)" << std::endl;
	std::cout << R"(+------------------------------------------------------------------------------------------------+)" << std::endl;
}

std::string GetCommand()
{
	std::string res;
	std::string tmp;
	int n = 0;
	do {
		if (n == 0) {
			std::cout << PROMPT;
		}
		else {
			std::cout << "        ";
		}
		n++;
		getline(std::cin, tmp);
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
