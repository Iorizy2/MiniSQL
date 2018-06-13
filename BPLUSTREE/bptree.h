/******************************************************************

** 文件名: bptree.h

** Copyright (c)

** 创建人: ReFantasy/TDL

** 日  期: 2018-6-13

** 描  述: 实现 B+tree 保存索引文件

** 版  本: 1.00

******************************************************************/
#ifndef __BPTREE_H__
#define __BPTREE_H__
#include "../BUFFER/Buffer.h"
using NodePointer = FileAddr;
class KeyAttr
{
public:
	bool operator<(const KeyAttr &rhs) {}
	bool operator>(const KeyAttr &rhs) {}
	bool operator==(const KeyAttr &rhs) {}
};

constexpr int bptree_t = 100;                      // B+tree's degree, bptree_t >= 2
constexpr int MaxKeyCount = 2 * bptree_t - 1;      // the max number of keys in a b+tree node
constexpr int MaxChildCount = 2 * bptree_t;        // the max number of child in a b+tree node
// the head of index file
// it contains the basic informatior used for b+ tree
struct IndexFileHead
{
	FileAddr ROOT;                                // the address of the B+tree root
	FileAddr start;                               // the address of the most left node
	int      FieldsNum;                           // the number of fields 
	int      KeySize;                             // the size of key word
	int      MaxKeyNum;                           // the max number of keys in a node
};

// 
class IndexFileHeadManage
{
public:
	IndexFileHeadManage(const char*idx_file, char* KeyStr);
	FileAddr GetRoot();                             // get the File Point of the root
	char *idx_file;
	char *KeyInfo;                                 // a string record the information of the key

private:
	void    CreateIdxHead(char* KeyStr);            // produce the information in the IdxHead
	IndexFileHead IdxHead;
	void    SetKeyInfo(char* Key_Info);             // calculate the Keysize and KeyAttrNum
};

// B+Tree Node
class BTreeNode
{
public:
	int         valid_key_count;               // the number of the valid Keys
	KeyAttr     key[MaxKeyCount+1];              // the array of Keys
	FileAddr    child[MaxChildCount];          // the array of points who point to the son nodes
	bool is_leaf;                              // leaf is true
};


// the position of a key in node
struct KeyPostion
{
	NodePointer pos;
	int i;
};
class BTree
{
public:
	BTree(const char*idx_file, char* KeyStr);
private:
	KeyPostion search(NodePointer x, KeyAttr k);    // search key k in subtree x
private:
	IndexFileHeadManage idx_head;
};
#endif
