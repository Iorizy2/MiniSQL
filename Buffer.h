/****************************************************************** 

** 文件名: Buffer.h

** Copyright (c) 

** 创建人: 谭东亮

** 日  期: 2018-6-3

** 描  述: MiniSQL buffer模块所有类和结构
           通过该模块实现物理文件的写入和删除
** 版  本: 1.00

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

#define FILE_PAGESIZE		4096	// 内存页(==文件页)大小
#define MEM_PAGEAMOUNT		1024	// 内存页数量
#define MAX_FILENAME_LEN    256		// 文件名（包含路径）最大长度

/*********************************************************
*             文件地址
**********************************************************/
class FileAddr
{
	friend class FILECOND;
public:
	FileAddr() :filePageID(0), offSet(0) {}
	FileAddr(unsigned long _filePageID, unsigned int  _offSet) :filePageID(_filePageID), offSet(_offSet) {}
public:
	unsigned long filePageID;     // 页编号
	unsigned int  offSet;         // 页内偏移量
	void *Ptr2Mem;                // 内存地址
};


/*********************************************************
*             页头信息，用以标识文件页
**********************************************************/
class PAGEHEAD
{
public:
	void Initialize();
	unsigned long pageId;		// 页编号
	bool isFixed;				// 页是否常驻内存
};


/*********************************************************
*               文件头信息
**********************************************************/
class FILECOND
{
public:
	void Initialize();
	FILECOND()=default;
	FileAddr DelFirst;                // 第一条被删除记录地址
	FileAddr DelLast;                 // 最后一条被删除记录地址  
	FileAddr NewInsert;               // 文件末尾可插入新数据的地址
	unsigned long total_page;         // 目前文件中共有页数
};

class MemPage
{
public:
	MemPage();

	//**把内存中的页写回到文件中
	void Back2File() const;
public:
	unsigned long fileId;         // 文件指针
	unsigned long filePageID;     // 文件页号
	bool isModified;              // 是否脏页
	void *Ptr2PageBegin;
	PAGEHEAD *pageHead;
	FILECOND* GetFileCond();

};

class Clock
{
public:
	Clock();

	// 返回磁盘文件内存地址
	FileAddr GetMemFile(unsigned long fileId, unsigned long filePageID);
private:
	unsigned int GetSwapPage();
	MemPage* memPage[MEM_PAGEAMOUNT+1];
};

/*********************************************************
*               内存文件类
**********************************************************/
class MemFile
{
public:
	MemFile(const char *file_name);
public:
	//void AddOnePage();
	char fileName[MAX_FILENAME_LEN];
	unsigned long fileId;             // 文件指针
	unsigned long total_page;         // 目前文件中共有页数
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
