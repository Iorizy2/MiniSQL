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
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>

#define FILE_PAGESIZE		4096	// �ڴ�ҳ(==�ļ�ҳ)��С
#define MEM_PAGEAMOUNT		6	// �ڴ�ҳ����
#define MAX_FILENAME_LEN    256		// �ļ���������·������󳤶�

class Clock;
Clock* GetGlobalClock();

/*********************************************************
*             �ļ���ַ,��λ�ļ��е�λ��
**********************************************************/
class FileAddr
{
	friend class FILECOND;
public:
	void SetFileAddr(unsigned long _filePageID, unsigned int  _offSet);
	unsigned long filePageID;     // �ļ�ҳ���
	unsigned int  offSet;         // ҳ��ƫ����
};


/*********************************************************
*             ҳͷ��Ϣ�����Ա�ʶ�ļ�ҳ
**********************************************************/
class PAGEHEAD
{
public:
	void Initialize();          // ��ʼ��Ϊ�ļ���һҳ
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
	FileAddr DelFirst;                // ��һ����ɾ����¼��ַ
	FileAddr DelLast;                 // ���һ����ɾ����¼��ַ  
	FileAddr NewInsert;               // �ļ�ĩβ�ɲ��������ݵĵ�ַ
	unsigned long total_page;         // Ŀǰ�ļ��й���ҳ��
};

/*********************************************************
*
*   ���ƣ��ڴ�ҳ��
*   ���ܣ��ṩ�����ļ�ҳ�Ŀռ䣬�Լ���ҳ��ص���Ϣ
*   Լ�����ڴ�ҳ�Ĵ�С�̶�
*
**********************************************************/
class MemPage
{
	friend class MemFile;
	friend class Clock;
public:
	MemPage();
	~MemPage();
	void Back2File() const;       // ���ڴ��е�ҳд�ص��ļ���
	bool SetModified();           // ����Ϊ��ҳ

public:
	unsigned long fileId;         // �ļ�ָ�룬while fileId==0 ʱΪ��������ҳ
	unsigned long filePageID;     // �ļ�ҳ��

	mutable bool bIsLastUsed;     // ���һ�η����ڴ��Ƿ�ʹ�ã�����Clock�㷨
	mutable bool isModified;      // �Ƿ���ҳ

	void *Ptr2PageBegin;          // ʵ�ʱ��������ļ����ݵĵ�ַ
	PAGEHEAD *pageHead;           // ҳͷָ��
	FILECOND* GetFileCond();      // �ļ�ͷָ�루while filePageID == 0��
};

/*********************************************************
*
*   ���ƣ��ڴ�ҳ�����ࣨClockҳ���û��㷨��
*   ���ܣ�����ҳ�����ڴ��еĻ��棬���ٶ������ļ��Ķ�д
*   Լ���������߱�֤��Ҫ������������ļ������ڣ��Ҽ��ص�ҳ�治Խ��
*
**********************************************************/
class Clock
{
	friend class MemFile;
public:
	Clock();
	~Clock();
	// ���ش����ļ��ڴ��ַ
	MemPage* GetMemAddr(unsigned long fileId, unsigned long filePageID);

	// ������ҳ�������ڴ������ļ����������ҳ�������
	MemPage* CreatNewPage(unsigned long fileId, unsigned long filePageID);

private:
	// ����һ�����滻���ڴ�ҳ����
	// ԭҳ�����ݸ�д����д��
	unsigned int GetReplaceablePage();

	// return the file page memory address if it is in memory
	// otherwise return nullptr;
	MemPage* GetExistedPage(unsigned long fileId, unsigned long filePageID);
	MemPage* LoadFromFile(unsigned long fileId, unsigned long filePageID);

	// Clock�û��㷨
	unsigned long ClockSwap();

	 
private:
	MemPage* MemPages[MEM_PAGEAMOUNT+1];  // �ڴ�ҳ��������
};

/*********************************************************
*   ���ƣ��ڴ��ļ���
*   ���ܣ�ͨ�������ļ����ڴ��е�ӳ���ļ��Ĳ������Ӷ����������ļ�
*   Լ�����������б��������ļ����������Ѿ���
**********************************************************/
class MemFile
{
	friend class BUFFER;
public:
	// д������
	FileAddr MemWrite(const void* source, size_t length, FileAddr* dest);
	MemPage * AddOnePage();                  // ��ǰ�ļ����һҳ�ռ�
private:
	// ����
	MemFile(const char *file_name, unsigned long file_id);
public:
	
	MemPage* GetFileFirstPage();            //�õ��ļ���ҳ
private:
	char fileName[MAX_FILENAME_LEN];
	unsigned long fileId;                   // �ļ�ָ��
	unsigned long total_page;               // Ŀǰ�ļ��й���ҳ��
};


class BUFFER
{
public:
	BUFFER() = default;
	~BUFFER();
	MemFile* GetMemFile(const char *fileName);
	void CreateFile(const char *fileName);
	
public:
	std::vector<MemFile*> memFile;  // �����Ѿ��򿪵��ļ��б�
};



#endif //define _BUFFER_H_
