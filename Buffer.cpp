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
	DelFirst = FileAddr();
	DelLast = FileAddr();
	NewInsert = FileAddr(0, sizeof(PAGEHEAD)+sizeof(FILECOND));
}

MemFile::MemFile(const char *file_name, unsigned long file_id, Clock *pMemClock)
{
	strcpy(this->fileName, file_name);
	this->fileId = file_id;
	this->total_page = pMemClock->GetMemAddr(this->fileId, 0)->GetFileCond()->total_page;

	// 文件不存在
	//if (this->fileId == -1) 
	//{
	//	// 新建文件(打开文件)
	//	this->fileId = open(file_name, _O_BINARY | O_RDWR | O_CREAT, 0664); 
	//	
	//	// 创建新的文件第一页并写入磁盘
	//	pMemClock->CreatNewPage(this->fileId, 0);
	//}
	//this->total_page = pMemClock->GetMemAddr(this->fileId, 0)->GetFileCond()->total_page;
}

MemPage * MemFile::AddOnePage(Clock *pMemClock)
{
	MemPage * newMemPage = pMemClock->CreatNewPage(this->fileId, this->total_page);
	this->total_page += 1;
	pMemClock->GetMemAddr(this->fileId, 0)->GetFileCond()->total_page = this->total_page;
	pMemClock->GetMemAddr(this->fileId, 0)->isModified = true;
	return newMemPage;
}

MemPage* MemFile::GetFileFirstPage(Clock *pMemClock)
{
	return pMemClock->GetMemAddr(this->fileId, 0);  // 文件首页
}

MemPage::MemPage()
{
	Ptr2PageBegin = malloc(FILE_PAGESIZE);
	pageHead = (PAGEHEAD*)Ptr2PageBegin;
	isModified = true;
	bIsLastUsed = false;
}

MemPage::~MemPage()
{
	// 脏页需要写回
	if (this->isModified)
		this->Back2File();
	delete Ptr2PageBegin;
}

void MemPage::Back2File() const
{
	// 脏页需要写回
	if (isModified)
	{
		lseek(this->fileId, this->filePageID*FILE_PAGESIZE, 0);
		write(this->fileId, this->Ptr2PageBegin, FILE_PAGESIZE); // 写回文件
	}
	
}

bool MemPage::SetModified()
{
	isModified = true;
	return true;
}

FILECOND* MemPage::GetFileCond()
{
	return (FILECOND*)((char*)Ptr2PageBegin + sizeof(PAGEHEAD));
}

Clock::Clock()
{
	for (int i = 0; i <= MEM_PAGEAMOUNT; i++)
	{
		MemPages[i] = nullptr;
	}
}

Clock::~Clock()
{
	for (int i = 0; i <= MEM_PAGEAMOUNT; i++)
	{
		if (MemPages[i]!=nullptr)
			delete MemPages[i];
	}
}

MemPage* Clock::GetMemAddr(unsigned long fileId, unsigned long filePageID)
{
	// 先查找是否存在内存中
	MemPage* pMemPage = GetExistedPage(fileId, filePageID);
	if (pMemPage != nullptr)
		return pMemPage;

	// 否则，从磁盘换入
	return LoadFromFile(fileId, filePageID);
}

MemPage* Clock::CreatNewPage(unsigned long file_id, unsigned long file_page_id)
{
	// 初始化内存页对象
	auto i = GetReplaceablePage();
	MemPages[i]->fileId = file_id;
	MemPages[i]->filePageID = file_page_id;
	MemPages[i]->isModified = true;  // 新页设置为脏页，需要写回

	// 初始化新页的页头信息
	MemPages[i]->pageHead->pageId = file_page_id;
	if (file_page_id != 0)
	{
		MemPages[i]->pageHead->isFixed = 0;
	}
	else
	{
		MemPages[i]->pageHead->isFixed = 1;
		MemPages[i]->GetFileCond()->Initialize();
	}
	return MemPages[i];

}

MemPage* Clock::GetExistedPage(unsigned long fileId, unsigned long filePageID)
{
	// look up for the page in memPage list
	for (int i = 0; i <= MEM_PAGEAMOUNT; i++)
	{
		if (MemPages[i] && MemPages[i]->fileId == fileId && MemPages[i]->filePageID == filePageID)
			return MemPages[i];
	}
	return nullptr;
}

MemPage* Clock::LoadFromFile(unsigned long fileId, unsigned long filePageID)
{
	unsigned int freePage = GetReplaceablePage();
	MemPages[freePage]->fileId = fileId;
	MemPages[freePage]->filePageID = filePageID;
	MemPages[freePage]->isModified = false;

	lseek(fileId, filePageID*FILE_PAGESIZE, 0);  // 定位到将要取出的文件页的首地址
	read(fileId, MemPages[freePage]->Ptr2PageBegin, FILE_PAGESIZE); // 读到内存中
	return MemPages[freePage];
}

unsigned int Clock::GetReplaceablePage()
{
	// 查找没有分配的内存页
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (MemPages[i] == nullptr)
		{
			MemPages[i] = new MemPage();
			return i;
		}
			
	}

	// 查找被抛弃的页
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (MemPages[i]->fileId == 0)
			return i;
	}

	// clock算法
	unsigned int i = rand() % MEM_PAGEAMOUNT;
	MemPages[i]->Back2File();
	return i;
}

BUFFER::~BUFFER()
{
	for (int i = 0; i < memFile.size(); i++)
	{
		delete memFile[i];
	}
}

MemFile* BUFFER::GetMemFile(const char *fileName)
{
	// 如果文件已经打开
	for (int i = 0; i < memFile.size(); i++)
	{
		if ((strcmp(memFile[i]->fileName, fileName) == 0))
			return memFile[i];
	}

	// 文件存在但是没打开
	int Ptr2File = open(fileName, _O_BINARY | O_RDWR, 0664);
	if (Ptr2File != -1)
	{
		MemFile* newFile = new MemFile(fileName,Ptr2File, &MemClock);
		memFile.push_back(newFile);
	}

	// 文件不存在
	return nullptr;

}

//MemPage* BUFFER::ReadFile(const char *fileName, unsigned int file_page_id)
//{
//	// 如果文件已经打开
//	for (int i = 0; i < memFile.size(); i++)
//	{
//		if ((strcmp(memFile[i]->fileName, fileName) == 0) && file_page_id<memFile[i]->total_page)
//			return MemClock.GetMemAddr(memFile[i]->fileId, file_page_id);
//	}
//
//	//----- 如果文件没有打开,要么打开文件，要么创建文件
//	MemFile* newFile = new MemFile(fileName, &MemClock);
//	memFile.push_back(newFile);
//	return MemClock.GetMemAddr(newFile->fileId, file_page_id);
//}

MemPage* BUFFER::CreateFile(const char *fileName)
{
	// 文件存在 创建失败
	int Ptr2File = open(fileName, _O_BINARY | O_RDWR, 0664);
	if (Ptr2File != -1)
	{
		close(Ptr2File);
		return nullptr;
	}

	//创建文件
	int newFile = open(fileName, _O_BINARY | O_RDWR | O_CREAT, 0664); // 新建文件(打开文件)

	void *ptr = malloc(FILE_PAGESIZE);
	((PAGEHEAD*)ptr)->pageId = 0;
	((PAGEHEAD*)ptr)->isFixed = 1;
	((FILECOND*)((char*)ptr + sizeof(PAGEHEAD)))->Initialize();
	close(newFile);
}
