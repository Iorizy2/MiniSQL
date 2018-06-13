#include <iostream>
#include <string>
#include "ERROR/Error.h"
#include "BUFFER/Buffer.h"
#include "BPLUSTREE/bptree.h"

using namespace std;


// 判断POD数据
void IsPod()
{
	cout << std::is_pod<PAGEHEAD>::value << endl;
	cout << std::is_pod<FileAddr>::value << endl;
	cout << std::is_pod<FILECOND>::value << endl;
	cout << std::is_pod<IndexFileHead>::value << endl;
	cout << std::is_pod<BTreeNode>::value << endl;
}

int main()
{
	IsPod();
	//const auto &buffer = GetGlobalFileBuffer();
	/*auto pMemFile = buffer["data"];
	if(!pMemFile)
		buffer.CreateFile("data");*/

	//BufferModuleTest();
	system("pause");
}