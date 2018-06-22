#include "Error.h"
#include <vector>


void DispatchError(ERROR error)
{
	switch (error)
	{
	case ERROR::LSEEK_FAILED:
		std::cout << "(LSEEK_FAILED) The file handle is invalid or the value for origin is invalid "
			         "or the position the position specified by offset is before the beginning of the file.";
		break;

	case ERROR::READ_FAILED:
		std::cout << "(READ_FAILED) Illegal page number (less than zero).";
		break;

	case ERROR::WRITE_FAILED:
		std::cout << "(WRITE_FAILED) The file handle is invalid or the file is not opened for writing "
			         "or there is not enough space left on the device for the operation.";
		break;

	case ERROR::FILENAME_CONVERT_FAILED:
		std::cout << "(FILENAME_CONVERT_FAILED) File name convert failed";
		break;

	case ERROR::KEY_INSERT_FAILED:
		std::cout << "(KEY_INSERT_FAILED) Key Word Insert Failed! The record that to inset has been excisted!";
		break;

	case ERROR::BPLUSTREE_DEGREE_TOOBIG:
		std::cout << "(BPLUSTREE_DEGREE_TOOBIG) A page of file can not contain a tree node!";
	default:
		break;
	}
}
