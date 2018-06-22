#include "Error.h"
#include <vector>

void SQLError::DispatchError(const BaseError &error)
{
	error.PrintError();
}

void SQLError::LSEEK_ERROR::PrintError() const
{
	std::cout << "(LSEEK_FAILED) The file handle is invalid or the value for origin is invalid "
		"or the position the position specified by offset is before the beginning of the file.";
}

void SQLError::READ_ERROR::PrintError() const
{
	std::cout << "(READ_FAILED) Illegal page number (less than zero).";
}

void SQLError::WRITE_ERROR::PrintError() const
{
	std::cout << "(WRITE_FAILED) The file handle is invalid or the file is not opened for writing "
		"or there is not enough space left on the device for the operation.";
}

void SQLError::FILENAME_CONVERT_ERROR::PrintError() const
{
	std::cout << "(FILENAME_CONVERT_FAILED) File name convert failed";
}

void SQLError::KEY_INSERT_ERROR::PrintError() const
{
	std::cout << "(KEY_INSERT_FAILED) Key Word Insert Failed! The record that to inset has been excisted!";
}

void SQLError::BPLUSTREE_DEGREE_TOOBIG_ERROR::PrintError() const
{
	std::cout << "(BPLUSTREE_DEGREE_TOOBIG) A page of file can not contain a tree node!";
}
