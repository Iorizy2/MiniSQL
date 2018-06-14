/******************************************************************

** 文件名: Error.h

** Copyright (c)

** 创建人: 谭东亮

** 日  期: 2018-6-10

** 描  述: 定义异常类型 

** 版  本: 1.00

******************************************************************/

#ifndef __ERROR_H__
#define __ERROR_H__
#include <iostream>
using ULONG = unsigned long;

enum class ERROR :ULONG {
	LSEEK_FAILED,
	READ_FAILED,
	WRITE_FAILED,
	FILENAME_CONVERT_FAILED,
	KEY_INSERT_FAILED
};

void DispatchError(ERROR error);

#endif
