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
	DelFirst = FileAddr();
	DelLast = FileAddr();
	NewInsert = FileAddr(0, sizeof(PAGEHEAD)+sizeof(FILECOND));
}

MemFile::MemFile(const char *file_name, unsigned long file_id, Clock *pMemClock)
{
	strcpy(this->fileName, file_name);
	this->fileId = file_id;
	this->total_page = pMemClock->GetMemAddr(this->fileId, 0)->GetFileCond()->total_page;

	// �ļ�������
	//if (this->fileId == -1) 
	//{
	//	// �½��ļ�(���ļ�)
	//	this->fileId = open(file_name, _O_BINARY | O_RDWR | O_CREAT, 0664); 
	//	
	//	// �����µ��ļ���һҳ��д�����
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
	return pMemClock->GetMemAddr(this->fileId, 0);  // �ļ���ҳ
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
	// ��ҳ��Ҫд��
	if (this->isModified)
		this->Back2File();
	delete Ptr2PageBegin;
}

void MemPage::Back2File() const
{
	// ��ҳ��Ҫд��
	if (isModified)
	{
		lseek(this->fileId, this->filePageID*FILE_PAGESIZE, 0);
		write(this->fileId, this->Ptr2PageBegin, FILE_PAGESIZE); // д���ļ�
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
	// �Ȳ����Ƿ�����ڴ���
	MemPage* pMemPage = GetExistedPage(fileId, filePageID);
	if (pMemPage != nullptr)
		return pMemPage;

	// ���򣬴Ӵ��̻���
	return LoadFromFile(fileId, filePageID);
}

MemPage* Clock::CreatNewPage(unsigned long file_id, unsigned long file_page_id)
{
	// ��ʼ���ڴ�ҳ����
	auto i = GetReplaceablePage();
	MemPages[i]->fileId = file_id;
	MemPages[i]->filePageID = file_page_id;
	MemPages[i]->isModified = true;  // ��ҳ����Ϊ��ҳ����Ҫд��

	// ��ʼ����ҳ��ҳͷ��Ϣ
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

	lseek(fileId, filePageID*FILE_PAGESIZE, 0);  // ��λ����Ҫȡ�����ļ�ҳ���׵�ַ
	read(fileId, MemPages[freePage]->Ptr2PageBegin, FILE_PAGESIZE); // �����ڴ���
	return MemPages[freePage];
}

unsigned int Clock::GetReplaceablePage()
{
	// ����û�з�����ڴ�ҳ
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (MemPages[i] == nullptr)
		{
			MemPages[i] = new MemPage();
			return i;
		}
			
	}

	// ���ұ�������ҳ
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
	{
		if (MemPages[i]->fileId == 0)
			return i;
	}

	// clock�㷨
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
	// ����ļ��Ѿ���
	for (int i = 0; i < memFile.size(); i++)
	{
		if ((strcmp(memFile[i]->fileName, fileName) == 0))
			return memFile[i];
	}

	// �ļ����ڵ���û��
	int Ptr2File = open(fileName, _O_BINARY | O_RDWR, 0664);
	if (Ptr2File != -1)
	{
		MemFile* newFile = new MemFile(fileName,Ptr2File, &MemClock);
		memFile.push_back(newFile);
	}

	// �ļ�������
	return nullptr;

}

//MemPage* BUFFER::ReadFile(const char *fileName, unsigned int file_page_id)
//{
//	// ����ļ��Ѿ���
//	for (int i = 0; i < memFile.size(); i++)
//	{
//		if ((strcmp(memFile[i]->fileName, fileName) == 0) && file_page_id<memFile[i]->total_page)
//			return MemClock.GetMemAddr(memFile[i]->fileId, file_page_id);
//	}
//
//	//----- ����ļ�û�д�,Ҫô���ļ���Ҫô�����ļ�
//	MemFile* newFile = new MemFile(fileName, &MemClock);
//	memFile.push_back(newFile);
//	return MemClock.GetMemAddr(newFile->fileId, file_page_id);
//}

MemPage* BUFFER::CreateFile(const char *fileName)
{
	// �ļ����� ����ʧ��
	int Ptr2File = open(fileName, _O_BINARY | O_RDWR, 0664);
	if (Ptr2File != -1)
	{
		close(Ptr2File);
		return nullptr;
	}

	//�����ļ�
	int newFile = open(fileName, _O_BINARY | O_RDWR | O_CREAT, 0664); // �½��ļ�(���ļ�)

	void *ptr = malloc(FILE_PAGESIZE);
	((PAGEHEAD*)ptr)->pageId = 0;
	((PAGEHEAD*)ptr)->isFixed = 1;
	((FILECOND*)((char*)ptr + sizeof(PAGEHEAD)))->Initialize();
	close(newFile);
}
