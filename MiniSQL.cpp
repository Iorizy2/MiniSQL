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
	cout << std::is_pod<BTNode>::value << endl;
	//cout << std::is_pod<BTreeNode>::value << endl;
}

int main()
{
	//IsPod();
	auto &buffer = GetGlobalFileBuffer();
	KeyAttr k1;
	k1.x = 192;
	strcpy(k1.s, "hello");
	KeyAttr k2{ 2 };
	strcpy(k2.s, "world");

	buffer.CreateFile("test.dbf");
	FileAddr k1_fd = buffer["test.dbf"]->AddRecord(&k1, sizeof(k1));
	FileAddr k2_fd = buffer["test.dbf"]->AddRecord(&k2, sizeof(k2));

	BTree tree("test.idx");
	tree.Insert(k1, k1_fd);
	tree.Insert(k2, k2_fd);

	auto k1_res = tree.Search(k1);
	auto k2_res = tree.Search(k2);
	char*p = (char*)buffer["test.dbf"]->ReadRecord(&k1_fd);
	p += sizeof(FileAddr);
	//cout << ((KeyAttr*)p)->x << endl;
	//cout << ((KeyAttr*)p)->s << endl;

	tree.PrintBTree();
	system("pause");
}