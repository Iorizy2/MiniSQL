/****************************************************************** 

** �ļ���: Buffer.h

** Copyright (c) 

** ������: ̷����

** ��  ��: 2018-6-3

** ��  ��: MiniSQL bufferģ��������ͽṹ
           ͨ����ģ��ʵ�������ļ���д���ɾ��
** ��  ��: 1.00

******************************************************************/

#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define FILE_PAGESIZE		4096	// �ڴ�ҳ(==�ļ�ҳ)��С
#define MEM_PAGEAMOUNT		1024	// �ڴ�ҳ����
#define MAX_FILENAME_LEN    256		// �ļ���������·������󳤶�

/*********************************************************
*             �ļ���ַ
**********************************************************/
class FileAddr
{
	friend class FILECOND;
public:
	FileAddr() :filePageID(0), offSet(0) {}
	FileAddr(unsigned long _filePageID, unsigned int  _offSet) :filePageID(_filePageID), offSet(_offSet) {}
public:
	unsigned long filePageID;     // ҳ���
	unsigned int  offSet;         // ҳ��ƫ����
	void *Ptr2Mem;                // �ڴ��ַ
};


/*********************************************************
*             ҳͷ��Ϣ�����Ա�ʶ�ļ�ҳ
**********************************************************/
class PAGEHEAD
{
public:
	void Initialize();
	unsigned long pageId;		// ҳ���
	bool isFixed;				// ҳ�Ƿ�פ�ڴ�
};


/*********************************************************
*               �ļ�ͷ��Ϣ
**********************************************************/
class FILECOND
{
public:
	void Initialize();
	FILECOND()=default;
	FileAddr DelFirst;                // ��һ����ɾ����¼��ַ
	FileAddr DelLast;                 // ���һ����ɾ����¼��ַ  
	FileAddr NewInsert;               // �ļ�ĩβ�ɲ��������ݵĵ�ַ
	unsigned long total_page;         // Ŀǰ�ļ��й���ҳ��
};

class MemPage
{
public:
	MemPage();

	//**���ڴ��е�ҳд�ص��ļ���
	void Back2File() const;
public:
	unsigned long fileId;         // �ļ�ָ��
	unsigned long filePageID;     // �ļ�ҳ��
	bool isModified;              // �Ƿ���ҳ
	void *Ptr2PageBegin;
	PAGEHEAD *pageHead;
	FILECOND* GetFileCond();

};

class Clock
{
public:
	Clock();

	// ���ش����ļ��ڴ��ַ
	FileAddr GetMemFile(unsigned long fileId, unsigned long filePageID);
private:
	unsigned int GetSwapPage();
	MemPage* memPage[MEM_PAGEAMOUNT+1];
};

/*********************************************************
*               �ڴ��ļ���
**********************************************************/
class MemFile
{
public:
	MemFile(const char *file_name);
public:
	//void AddOnePage();
	char fileName[MAX_FILENAME_LEN];
	unsigned long fileId;             // �ļ�ָ��
	unsigned long total_page;         // Ŀǰ�ļ��й���ҳ��
};

class BUFFER
{
public:
	FileAddr ReadFile(const char *fileName, unsigned int file_page);
public:
	std::vector<MemFile*> memFile;
	Clock MemClock;
};

#endif //define _BUFFER_H_
