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
#include <cassert>
#include "../ERROR/Error.h"

extern "C"
{
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
}

#define FILE_PAGESIZE        4096	// �ڴ�ҳ(==�ļ�ҳ)��С
#define MEM_PAGEAMOUNT       4096	// �ڴ�ҳ����
#define MAX_FILENAME_LEN     256	// �ļ���������·������󳤶�
#define FileAddrSize (sizeof(FileAddr))


class Clock;
class BUFFER;
Clock* GetGlobalClock();
BUFFER& GetGlobalFileBuffer();
const unsigned int FILECOND_RESERVE_SPACE = 256;  // �ļ�ͷԤ���ռ�

/*********************************************************
*             �ļ���ַ,��λ�ļ��е�λ��
**********************************************************/
class FileAddr
{
	friend class FILECOND;
public:
	void SetFileAddr(const unsigned long _filePageID, const unsigned int  _offSet);
	void ShiftOffset(const int OFFSET);

	unsigned long filePageID;     // �ļ�ҳ���
	unsigned int  offSet;         // ҳ��ƫ����
	
	bool operator==(const FileAddr &rhs) const
	{
		return (this->filePageID == rhs.filePageID && this->offSet == rhs.offSet);
	}
	bool operator!=(const FileAddr &rhs) const
	{
		return !(this->filePageID == rhs.filePageID && this->offSet == rhs.offSet);
	}
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
	FileAddr DelFirst;                         // ��һ����ɾ����¼��ַ
	FileAddr DelLast;                          // ���һ����ɾ����¼��ַ  
	FileAddr NewInsert;                        // �ļ�ĩβ�ɲ��������ݵĵ�ַ
	unsigned long total_page;                  // Ŀǰ�ļ��й���ҳ��
	char reserve[FILECOND_RESERVE_SPACE];      // Ԥ���ռ� 
};

/*********************************************************
*
*   ���ƣ�  �ڴ�ҳ��
*   ���ܣ�  �ṩ�����ļ�ҳ�Ŀռ䣬�Լ���ҳ��ص���Ϣ
*   ����ʽ���ڴ�ҳ�Ĵ�С�̶�
*
**********************************************************/
class MemPage
{
	friend class MemFile;
	friend class Clock;
	friend class BUFFER;
public:
	MemPage();
	~MemPage();
private:
	void Back2File() const;            // ���ڴ��е�ҳд�ص��ļ���
	bool SetModified();                // ����Ϊ��ҳ

public:
	unsigned long fileId;              // �ļ�ָ�룬while fileId==0 ʱΪ��������ҳ
	unsigned long filePageID;          // �ļ�ҳ��

	mutable bool bIsLastUsed;          // ���һ�η����ڴ��Ƿ�ʹ�ã�����Clock�㷨
	mutable bool isModified;           // �Ƿ���ҳ

	void *Ptr2PageBegin;               // ʵ�ʱ��������ļ����ݵĵ�ַ
	PAGEHEAD *pageHead;                // ҳͷָ��
	FILECOND* GetFileCond()const;      // �ļ�ͷָ�루while filePageID == 0��
};

/*********************************************************
*
*   ���ƣ�  �ڴ�ҳ�����ࣨClockҳ���û��㷨��
*   ���ܣ�  ����ҳ�����ڴ��еĻ��棬���ٶ������ļ��Ķ�д
*   ����ʽ�������߱�֤��Ҫ������������ļ������ڣ��Ҽ��ص�ҳ�治Խ��
*
**********************************************************/
class Clock
{
	friend class MemFile;
	friend class BUFFER;
	friend class BTree;
public:
	Clock();
	~Clock();
#ifndef NDEBUG
	void PrintFileNameInMemory();   // ��ӡ�������ڴ����л�����ļ�ҳ
#endif
private:
	// ���ش����ļ��ڴ��ַ
	MemPage* GetMemAddr(unsigned long fileId, unsigned long filePageID);

	// ������ҳ�������ڴ������ļ����������ҳ�������
	MemPage* CreatNewPage(unsigned long fileId, unsigned long filePageID);

private:
	// ����һ�����滻���ڴ�ҳ����
	// ԭҳ�����ݸ�д����д��
	unsigned int GetReplaceablePage();

	// ���Ŀ���ļ�ҳ�����ڴ滺���򷵻����ַ�����򷵻� nullptr
	MemPage* GetExistedPage(unsigned long fileId, unsigned long filePageID);
	MemPage* LoadFromFile(unsigned long fileId, unsigned long filePageID);

	// Clock�û��㷨
	unsigned long ClockSwap();

	 
private:
	MemPage* MemPages[MEM_PAGEAMOUNT+1];  // �ڴ�ҳ��������
};

/*********************************************************
*   ���ƣ�  �ڴ��ļ���
*   ���ܣ�  ͨ�������ļ����ڴ��е�ӳ���ļ��Ĳ������Ӷ����������ļ�
*   ����ʽ���������б��������ļ����������Ѿ���
*   ��¼��ʽ: ��¼��ַ+��¼����
**********************************************************/
class MemFile
{
	friend class BUFFER;
	friend class BTree;
public:
	const void* ReadRecord(FileAddr *address_delete)const;         // ��ȡĳ����¼,���ؼ�¼ָ��(������¼��ַ����)
	FileAddr AddRecord(void*source_record, size_t sz_record);                        // ���ؼ�¼����ӵ�λ��
	FileAddr DeleteRecord(FileAddr *address_delete, size_t record_sz);               // ����ɾ����λ��
	
private:
	// ����
	MemFile(const char *file_name, unsigned long file_id);
	// д������
	void* MemRead(FileAddr *mem_to_read);                           // ��ȡ�ڴ��ļ�,���ض�ȡλ��ָ��
	FileAddr MemWrite(const void* source, size_t length);           // �ڿ�д���ַд������
	FileAddr MemWrite(const void* source, size_t length, FileAddr* dest);
	
	void MemWipe(void*source, size_t sz_wipe, FileAddr *fd_to_wipe);

	MemPage * AddExtraPage();                                       // ��ǰ�ļ����һҳ�ռ�
	MemPage* GetFileFirstPage();                                    // �õ��ļ���ҳ

private:
	char fileName[MAX_FILENAME_LEN];
	unsigned long fileId;                                          // �ļ�ָ��
	unsigned long total_page;                                      // Ŀǰ�ļ��й���ҳ��
};


class BUFFER
{
public:
	BUFFER() = default;
	~BUFFER();
	MemFile* operator[](const char *fileName);      // ���ļ�����ʧ�ܷ��� nullptr

	void CreateFile(const char *fileName);          // �����ļ�������ʽ��
	void CloseFile(const char *FileName);
	void CloseAllFile();	
private:
	// �����ļ���ӳ����ڴ��ļ�
	MemFile* GetMemFile(const char *fileName);
private:
	std::vector<MemFile*> memFiles;  // �����Ѿ��򿪵��ļ��б�
};

// BUFFER ģ����Ժ���
std::string IntToStr(int i);
void BufferModuleTest();

#endif //define _BUFFER_H_
