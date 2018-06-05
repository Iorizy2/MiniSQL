/****************************************************************** 

** �ļ���: Buffer.cpp

** Copyright (c) 

** ������: ̷����

** ��  ��: 2018-6-3

** ��  ��: ������MiniSQL bufferģ��������ͽṹ

** ��  ��: 1.00

******************************************************************/
#include"Buffer.h"
#include <iostream>

// ����ȫ���ڴ滺��ҳ
Clock* GetGlobalClock()
{
	static Clock MemClock;
	return &MemClock;
}

void PAGEHEAD::Initialize()
{
	pageId = 0;
	isFixed = 1;
}

void FILECOND::Initialize()
{
	total_page = 1;

	FileAddr fd_tmp;
	fd_tmp.SetFileAddr(0, 0);
	DelFirst = fd_tmp;
	DelLast = fd_tmp;
	fd_tmp.offSet = sizeof(PAGEHEAD) + sizeof(FILECOND);
	NewInsert = fd_tmp;
}

// ����λ��д����������,����д�����µ�ַλ�ã�д��ʧ�ܷ���ԭ��ַ
FileAddr MemFile::MemWrite(const void* source, size_t length, FileAddr* dest)
{
	auto pMemPage = GetGlobalClock()->GetMemAddr(this->fileId, dest->filePageID);
	// �����ҳʣ��ռ䲻��
	if ((FILE_PAGESIZE - dest->offSet) < length)
	{
		return *dest;
	}
	memcpy((void*)((char*)pMemPage->Ptr2PageBegin+dest->offSet), source, length);
	printf("in");
	pMemPage->isModified = true;
	pMemPage->bIsLastUsed = true;

	dest->offSet += length;
	return *dest;
}

// �ڿ�д���ַд�����ݣ����ռ䲻���������µ�ҳ
FileAddr MemFile::MemWrite(const void* source, size_t length)
{
	// ��ȡ��д���ַ
	FileAddr InsertPos = GetGlobalClock()->GetMemAddr(this->fileId, 0)->GetFileCond()->NewInsert;

	// д��
	FileAddr write_res = MemWrite(source, length, &InsertPos);
	if (write_res.offSet == 0)  //�ռ䲻�㣬��Ҫ�����µ�ҳ
	{
		AddOnePage();
		InsertPos.SetFileAddr(InsertPos.filePageID + 1, sizeof(PAGEHEAD));
		write_res = MemWrite(source, length, &InsertPos);  // ����д��
	}

	// ���¿�д��λ��
	GetGlobalClock()->GetMemAddr(this->fileId, 0)->GetFileCond()->NewInsert = write_res;
	GetGlobalClock()->GetMemAddr(this->fileId, 0)->SetModified();
	GetGlobalClock()->GetMemAddr(this->fileId, 0)->bIsLastUsed = true;
	return write_res;
}

MemFile::MemFile(const char *file_name, unsigned long file_id)
{
	strcpy(this->fileName, file_name);
	this->fileId = file_id;
	this->total_page = GetGlobalClock()->GetMemAddr(this->fileId, 0)->GetFileCond()->total_page;
}

MemPage * MemFile::AddOnePage()
{
	Clock *pMemClock = GetGlobalClock();
	//��ȡ�ļ���ҳ
	MemPage *FileFirstPage = pMemClock->GetMemAddr(this->fileId, 0);
	this->total_page = FileFirstPage->GetFileCond()->total_page+1;
	FileFirstPage->GetFileCond()->total_page += 1;
	FileFirstPage->SetModified();
	FileFirstPage->bIsLastUsed = true;
	//�������ڴ�ҳ
	MemPage * newMemPage = pMemClock->CreatNewPage(this->fileId, FileFirstPage->GetFileCond()->total_page-1);
	newMemPage->isModified = true;
	newMemPage->bIsLastUsed = true;
	
	return newMemPage;
}

MemPage* MemFile::GetFileFirstPage()
{
	return GetGlobalClock()->GetMemAddr(this->fileId, 0);  // �ļ���ҳ
}

MemPage::MemPage()
{
	Ptr2PageBegin = malloc(FILE_PAGESIZE);
	pageHead = (PAGEHEAD*)Ptr2PageBegin;
	isModified = false;
	bIsLastUsed = false;
}

MemPage::~MemPage()
{
	// ��ҳ�Ҳ���������ҳ��Ҫд��
	if (this->isModified && this->fileId>0)
		this->Back2File();
	delete Ptr2PageBegin;
}

void MemPage::Back2File() const
{
#ifndef NDEBUG
	std::cout << this->fileId << " : write back to file " << std::endl;
#endif
	// ��ҳ��Ҫд��
	if (isModified)
	{
		lseek(this->fileId, this->filePageID*FILE_PAGESIZE, 0);
		write(this->fileId, this->Ptr2PageBegin, FILE_PAGESIZE); // д���ļ�
		isModified = false;
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
	memset(MemPages[i]->Ptr2PageBegin, 0, FILE_PAGESIZE);
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
	for (int i = 1; i <= MEM_PAGEAMOUNT; i++)
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

//unsigned long Clock::ClockSwap()
//{
//	static unsigned long index = 1;
//	assert(MemPages[index] != nullptr);
//
//	while (MemPages[index]->bIsLastUsed)     // �����ʹ�ù�
//	{
//		MemPages[index]->bIsLastUsed = 0;
//		index = (index + 1) % MEM_PAGEAMOUNT;
//		if (index == 0)index++;
//	}
//
//	auto res = index;
//	MemPages[index]->bIsLastUsed = 1;
//	index = (index + 1) % MEM_PAGEAMOUNT;
//	if (index == 0)index++;
//	return res;
//	
//}

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
	//unsigned int i = ClockSwap();
	unsigned int i = rand() % MEM_PAGEAMOUNT;
	if (i == 0)i++;
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
#ifndef NDEBUG
		std::cout << "Open File --> pointer:"<<Ptr2File << "\tname:" << fileName << std::endl;
#endif
		MemFile* newFile = new MemFile(fileName,Ptr2File);
		memFile.push_back(newFile);
		return newFile;
	}

	// �ļ�������
	return nullptr;

}


void BUFFER::CreateFile(const char *fileName)
{
	// �ļ����� ����ʧ��
	int Ptr2File = open(fileName, _O_BINARY | O_RDWR, 0664);
	if (Ptr2File != -1)
	{
		close(Ptr2File);
		return;
	}

	//�����ļ�
	int newFile = open(fileName, _O_BINARY | O_RDWR | O_CREAT, 0664); // �½��ļ�(���ļ�)

	void *ptr = malloc(FILE_PAGESIZE);
	memset(ptr, 0, FILE_PAGESIZE);
	PAGEHEAD *pPageHead = (PAGEHEAD *)ptr;
	FILECOND *pFileCond = (FILECOND *)((char*)ptr + sizeof(PAGEHEAD));
	pPageHead->pageId = 0;
	pPageHead->isFixed = 1;
	pFileCond->Initialize();
	// д��
	write(newFile, ptr, FILE_PAGESIZE);
	close(newFile);
	delete ptr;
	return;
}

void BUFFER::CloseFile()
{
	for (int i = 0; i < memFile.size(); i++)
	{
		close(memFile[i]->fileId);
	}
}

void FileAddr::SetFileAddr(unsigned long _filePageID /*= 0*/, unsigned int _offSet /*= 0*/)
{
	filePageID = _filePageID;
	offSet = _offSet;
}
