/******************************************************************

** �ļ���: Error.h

** Copyright (c)

** ������: ReFantasy

** ��  ��: 2018-6-10

** ��  ��: �����쳣���� 

** �޸ļ�¼��

   2018-06-22 ReFantasy
              �޸Ĵ����쳣��ʵ�ַ�ʽ����ö�����͸�Ϊ������ķ�ʽ

** ��  ��: 1.00

******************************************************************/

#ifndef __ERROR_H__
#define __ERROR_H__
#include <iostream>
namespace SQLError
{
	/************************************************************************
	*
	*   �ӿ���
	*
	*************************************************************************/
	class BaseError
	{
	public:
		virtual void PrintError()const = 0;
	};

	// ��������
	void DispatchError(const SQLError::BaseError &error);


	/************************************************************************
	*                     
	*   ������
	*
	*************************************************************************/

	// ���������
	class LSEEK_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// �ļ�������
	class READ_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// �ļ�д����
	class WRITE_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// �ļ���ת������
	class FILENAME_CONVERT_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// �����ļ�����ؼ���ʧ��
	class KEY_INSERT_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	// B+���Ķ�ƫ��
	class BPLUSTREE_DEGREE_TOOBIG_ERROR :public BaseError
	{
	public:
		void PrintError() const override;
	};

	
}


#endif
