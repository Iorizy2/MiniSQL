#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include "Buffer.h"
using namespace std;

// 判断POD数据
void IsPod()
{
	cout << std::is_pod<PAGEHEAD>::value << endl;
	cout << std::is_pod<FileAddr>::value << endl;
	cout << std::is_pod<FILECOND>::value << endl;
}

int main()
{
	//IsPod();

	BUFFER buffer; 
	char FileName[] = "test8";
	//auto pMemFile = buffer.GetMemFile(FileName);
	//if (!pMemFile)
		//buffer.CreateFile(FileName);
	
	buffer.GetMemFile("test1")->GetFileFirstPage();
	buffer.GetMemFile("test2")->GetFileFirstPage();
	buffer.GetMemFile("test3")->GetFileFirstPage();
	buffer.GetMemFile("test4")->GetFileFirstPage();
	buffer.GetMemFile("test5")->GetFileFirstPage();
	buffer.GetMemFile("test6")->GetFileFirstPage();
	buffer.GetMemFile("test7")->GetFileFirstPage();
	buffer.GetMemFile("test8")->GetFileFirstPage();

	//if (pMemFile) pMemFile->AddOnePage();
	// 写入
	/*char s[] = "hello world";
	FileAddr fd;
	fd.SetFileAddr(0, sizeof(PAGEHEAD) + sizeof(FILECOND));
	pMemFile->MemWrite(s, sizeof(s), &fd);*/

	//cout << "total page: " << pMemFile->GetFileFirstPage()->GetFileCond()->NewInsert.offSet << endl;

	//cout << sizeof(PAGEHEAD) + sizeof(FILECOND) << endl;
	system("pause");
}