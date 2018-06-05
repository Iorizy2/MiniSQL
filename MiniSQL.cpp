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
	//MemFile file("test.data");
	BUFFER buffer; 
	char FileName[] = "test.idf";
	auto pMemFile = buffer.GetMemFile(FileName);
	if (!pMemFile)
		buffer.CreateFile(FileName);
	
	if (pMemFile) pMemFile->AddOnePage();
	// 写入
	/*char s[] = "hello world";
	FileAddr fd;
	fd.SetFileAddr(0, sizeof(PAGEHEAD) + sizeof(FILECOND));
	pMemFile->MemWrite(s, sizeof(s), &fd);*/

	//cout << "total page: " << pMemFile->GetFileFirstPage()->GetFileCond()->NewInsert.offSet << endl;

	//cout << sizeof(PAGEHEAD) + sizeof(FILECOND) << endl;
	system("pause");
}