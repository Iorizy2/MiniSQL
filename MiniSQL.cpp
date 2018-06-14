#include <iostream>
#include <string>
#include "ERROR/Error.h"
#include "BUFFER/Buffer.h"
#include "BPLUSTREE/bptree.h"
#define NDEBUG 
using namespace std;

// 判断POD数据
void IsPod();

int main()
{
	//IsPod();
	BTreeTest();
	system("pause");
}

void IsPod()
{
	cout << std::is_pod<PAGEHEAD>::value << endl;
	cout << std::is_pod<FileAddr>::value << endl;
	cout << std::is_pod<FILECOND>::value << endl;
	cout << std::is_pod<BTNode>::value << endl;
	//cout << std::is_pod<BTreeNode>::value << endl;
}