/****************************************************************** 

** 文件名: Buffer.cpp

** Copyright (c) 

** 创建人: 谭东亮

** 日  期: 2018-6-3

** 描  述: 定义了MiniSQL buffer模块所有类和结构

** 版  本: 1.00

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

	// 文件不存在
	if (this->fileId == -1) 
	{
		this->fileId = open(file_name, _O_BINARY | O_RDWR | O_CREAT, 0664); // 新建文件(打开文件)
		if (this->fileId == -1)  // 文件不能被打开(新建)
		{
			this->fileId = open(file_name, _O_BINARY | O_RDWR | O_CREAT, 0664); // 新建文件(打开文件)
			if (this->fileId == -1)  // 文件不能被新建(打开)
				throw 1006; // 文件还是不能被打开(新建),可能为磁盘空间不足等意外
		}
		// 创建新的文件第一页并写入磁盘
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
	temp = write(this->fileId, this->Ptr2PageBegin, FILE_PAGESIZE); // 写回文件
	if (temp != FILE_PAGESIZE) throw 1002;  // 写失败
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
	// 先查找是否存在内存中
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (memPage[i]&&memPage[i]->fileId == fileId && memPage[i]->filePageID == filePageID)
		{
			return FileAddr(memPage[i]->filePageID, sizeof(PAGEHEAD));
		}
	}
	// 否则，从磁盘换入
	unsigned int freePage = GetSwapPage();
	memPage[freePage]->fileId = fileId;
	memPage[freePage]->filePageID = filePageID;
	memPage[freePage]->isModified = false;
	read(memPage[freePage]->fileId, memPage[freePage]->Ptr2PageBegin, FILE_PAGESIZE);
	return FileAddr(memPage[freePage]->filePageID, sizeof(PAGEHEAD));
}

unsigned int Clock::GetSwapPage()
{
	// 先查找没有使用的页
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (memPage[i] == nullptr)
		{
			memPage[i] = new MemPage();
			return i;
		}
			
	}

	// 查找被抛弃的页
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (memPage[i]->fileId == 0)
			return i;
	}

	// clock算法
	unsigned int i = rand() % MEM_PAGEAMOUNT;
	memPage[i]->Back2File();
	return i;
}

FileAddr BUFFER::ReadFile(const char *fileName, unsigned int file_page)
{
	// 如果文件已经打开
	for (int i = 0; i < memFile.size(); i++)
	{
		if ((strcmp(memFile[i]->fileName, fileName) == 0) && file_page<memFile[i]->total_page)
			return MemClock.GetMemFile(memFile[i]->fileId, file_page);
	}

	//----- 如果文件没有打开
	MemFile* newFile = new MemFile(fileName);
	memFile.push_back(newFile);
	return MemClock.GetMemFile(newFile->fileId, 0);
}
