#include "Error.h"
#include <vector>

namespace SQLError
{
	static std::fstream log_file;
}

void SQLError::DispatchError(const BaseError &error)
{
	error.PrintError();
}

void SQLError::LSEEK_ERROR::PrintError() const
{
	std::string error_info =  "(LSEEK_FAILED) The file handle is invalid or the value for origin is invalid "
                              "or the position the position specified by offset is before the beginning of the file.";
	// 输出异常
	std::cout << error_info;

	// 写入日志
	log_file.open("log", std::ios::out | std::ios::app);
	log_file << error_info << std::endl;
	log_file.close();
}

void SQLError::READ_ERROR::PrintError() const
{
	std::string error_info = "(READ_FAILED) Illegal page number (less than zero).";

	// 输出异常
	std::cout << error_info;

	// 写入日志
	log_file.open("log", std::ios::out | std::ios::app);
	log_file << error_info << std::endl;
	log_file.close();
}

void SQLError::WRITE_ERROR::PrintError() const
{
	std::string error_info = "(WRITE_FAILED) The file handle is invalid or the file is not opened for writing "
		"or there is not enough space left on the device for the operation.";

	// 输出异常
	std::cout << error_info;

	// 写入日志
	log_file.open("log", std::ios::out | std::ios::app);
	log_file << error_info << std::endl;
	log_file.close();
}

void SQLError::FILENAME_CONVERT_ERROR::PrintError() const
{
	std::string error_info = "(FILENAME_CONVERT_FAILED) File name convert failed";

	// 输出异常
	std::cout << error_info;

	// 写入日志
	log_file.open("log", std::ios::out | std::ios::app);
	log_file << error_info << std::endl;
	log_file.close();
}

void SQLError::KEY_INSERT_ERROR::PrintError() const
{
	std::string error_info = "(KEY_INSERT_FAILED) Key Word Insert Failed! The record that to inset has been excisted!";

	// 输出异常
	std::cout << error_info;

	// 写入日志
	log_file.open("log", std::ios::out | std::ios::app);
	log_file << error_info << std::endl;
	log_file.close();
}

void SQLError::BPLUSTREE_DEGREE_TOOBIG_ERROR::PrintError() const
{
	std::string error_info = "(BPLUSTREE_DEGREE_TOOBIG) A page of file can not contain a tree node!";

	// 输出异常
	std::cout << error_info;

	// 写入日志
	log_file.open("log", std::ios::out | std::ios::app);
	log_file << error_info << std::endl;
	log_file.close();
}
