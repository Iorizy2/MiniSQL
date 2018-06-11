#include "Error.h"
#include <vector>


void DispatchError(ERROR error)
{
	switch (error)
	{
	case ERROR::LSEEK_FAILED:
		std::cout << "(lseek)the file handle is invalid or the value for origin is invalid "
			         "or the position the position specified by offset is before the beginning of the file.";
		break;

	case ERROR::READ_FAILED:
		std::cout << "illegal page number (less than zero).";
		break;

	case ERROR::WRITE_FAILED:
		std::cout << "(write)the file handle is invalid or the file is not opened for writing "
			         "or there is not enough space left on the device for the operation.";
		break;

	default:
		break;
	}
}
