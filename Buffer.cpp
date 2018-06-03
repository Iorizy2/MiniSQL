/****************************************************************** 

** �ļ���: Buffer.cpp

** Copyright (c) 

** ������: ̷����

** ��  ��: 2018-6-3

** ��  ��: ������MiniSQL bufferģ��������ͽṹ

** ��  ��: 1.00

******************************************************************/
#include"Buffer.h"

void PAGEHEAD::Initialize()
{
	pageId = 0;
	isFixed = 1;
}

void FILECOND::Initialize()
{
	total_page = 1; 
}

MemFile::MemFile(const char *file_name)
{
	strcpy(this->fileName, file_name);
	this->fileId = open(file_name, _O_BINARY | O_RDWR, 0664);

	// �ļ�������
	if (this->fileId == -1) 
	{
		this->fileId = open(file_name, _O_BINARY | O_RDWR | O_CREAT, 0664); // �½��ļ�(���ļ�)
		if (this->fileId == -1)  // �ļ����ܱ���(�½�)
		{
			this->fileId = open(file_name, _O_BINARY | O_RDWR | O_CREAT, 0664); // �½��ļ�(���ļ�)
			if (this->fileId == -1)  // �ļ����ܱ��½�(��)
				throw 1006; // �ļ����ǲ��ܱ���(�½�),����Ϊ���̿ռ䲻�������
		}
		// �����µ��ļ���һҳ��д�����
		this->total_page = 1;
		void* ptr = malloc(FILE_PAGESIZE);
		((PAGEHEAD*)ptr)->Initialize();
		((FILECOND*)((char*)ptr + sizeof(PAGEHEAD)))->Initialize();
		write(this->fileId, ptr, FILE_PAGESIZE);
		delete ptr;
	}
	
}

MemPage::MemPage()
{
	Ptr2PageBegin = malloc(FILE_PAGESIZE);
	pageHead = (PAGEHEAD*)Ptr2PageBegin;
	isModified = true;
}

void MemPage::Back2File() const
{
	int temp = 0;
	temp = lseek(this->fileId, this->filePageID*FILE_PAGESIZE, 0);
	if (temp == -1L) throw 1005;
	temp = write(this->fileId, this->Ptr2PageBegin, FILE_PAGESIZE); // д���ļ�
	if (temp != FILE_PAGESIZE) throw 1002;  // дʧ��
}

FILECOND* MemPage::GetFileCond()
{
	return (FILECOND*)((char*)Ptr2PageBegin + sizeof(PAGEHEAD));
}

Clock::Clock()
{
	for (int i = 0; i <= MEM_PAGEAMOUNT; i++)
	{
		memPage[i] = nullptr;
	}
}

FileAddr Clock::GetMemFile(unsigned long fileId, unsigned long filePageID)
{
	// �Ȳ����Ƿ�����ڴ���
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (memPage[i]&&memPage[i]->fileId == fileId && memPage[i]->filePageID == filePageID)
		{
			return FileAddr(memPage[i]->filePageID, sizeof(PAGEHEAD));
		}
	}
	// ���򣬴Ӵ��̻���
	unsigned int freePage = GetSwapPage();
	memPage[freePage]->fileId = fileId;
	memPage[freePage]->filePageID = filePageID;
	memPage[freePage]->isModified = false;
	read(memPage[freePage]->fileId, memPage[freePage]->Ptr2PageBegin, FILE_PAGESIZE);
	return FileAddr(memPage[freePage]->filePageID, sizeof(PAGEHEAD));
}

unsigned int Clock::GetSwapPage()
{
	// �Ȳ���û��ʹ�õ�ҳ
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (memPage[i] == nullptr)
		{
			memPage[i] = new MemPage();
			return i;
		}
			
	}

	// ���ұ�������ҳ
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (memPage[i]->fileId == 0)
			return i;
	}

	// clock�㷨
	unsigned int i = rand() % MEM_PAGEAMOUNT;
	memPage[i]->Back2File();
	return i;
}

FileAddr BUFFER::ReadFile(const char *fileName, unsigned int file_page)
{
	// ����ļ��Ѿ���
	for (int i = 0; i < memFile.size(); i++)
	{
		if ((strcmp(memFile[i]->fileName, fileName) == 0) && file_page<memFile[i]->total_page)
			return MemClock.GetMemFile(memFile[i]->fileId, file_page);
	}

	//----- ����ļ�û�д�
	MemFile* newFile = new MemFile(fileName);
	memFile.push_back(newFile);
	return MemClock.GetMemFile(newFile->fileId, 0);
}
