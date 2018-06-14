#include "Error.h"
#include <vector>


void DispatchError(ERROR error)
{
	switch (error)
	{
	case ERROR::LSEEK_FAILED:
		std::cout << "(LSEEK_FAILED)the file handle is invalid or the value for origin is invalid "
			         "or the position the position specified by offset is before the beginning of the file.";
		break;

	case ERROR::READ_FAILED:
		std::cout << "(READ_FAILED)illegal page number (less than zero).";
		break;

	case ERROR::WRITE_FAILED:
		std::cout << "(WRITE_FAILED)the file handle is invalid or the file is not opened for writing "
			         "or there is not enough space left on the device for the operation.";
		break;

	case ERROR::FILENAME_CONVERT_FAILED:
		std::cout << "(FILENAME_CONVERT_FAILED)file name convert failed";
		break;

	case ERROR::KEY_INSERT_FAILED:
		std::cout << "(KEY_INSERT_FAILED)Key Word Insert Failed! The record that to inset has been excisted!";
		break;

	default:
		break;
	}
}
