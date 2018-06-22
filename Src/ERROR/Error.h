/******************************************************************

** 文件名: Error.h

** Copyright (c)

** 创建人: ReFantasy

** 日  期: 2018-6-10

** 描  述: 定义异常类型 

** 修改记录：

   2018-06-22 ReFantasy
              修改错误异常的实现方式，由枚举类型改为具体类的方式

** 版  本: 1.00

******************************************************************/

#ifndef __ERROR_H__
#define __ERROR_H__
#include <iostream>
namespace SQLError
{
	/************************************************************************
	*
	*   接口类
	*
	*************************************************************************/
	class BaseError
	{
	public:
		virtual void PrintError()const = 0;
	};

	// 错误处理函数
	void DispatchError(const SQLError::BaseError &error);


	/************************************************************************
	*                     
	*   具体类
	*
	*************************************************************************/

	// 派生类错误
	class LSEEK_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// 文件读错误
	class READ_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// 文件写错误
	class WRITE_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// 文件名转换错误
	class FILENAME_CONVERT_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// 索引文件插入关键字失败
	class KEY_INSERT_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// B+树的度偏大
	class BPLUSTREE_DEGREE_TOOBIG_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	
}


#endif
