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
#include "../Src/BUFFER/Buffer.h"
#include "../Src/GLOBAL/global.h"
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <chrono>
const int RecordInfoLength = 32;  //记录最大的字段数量
class KeyAttr
{
public:
	int x;
	char s[1];
	bool operator<(const KeyAttr &rhs) { return x < rhs.x; }
	bool operator>(const KeyAttr &rhs) { return x > rhs.x; }
	bool operator==(const KeyAttr &rhs) { return x == rhs.x; }
	bool operator<=(const KeyAttr &rhs) { return x <= rhs.x; }
	bool operator>=(const KeyAttr &rhs) { return x >= rhs.x; }
	bool operator!=(const KeyAttr &rhs) { return x != rhs.x; }
};
std::ostream& operator<<(std::ostream &os, const KeyAttr &key);

constexpr int bptree_t = 3;                         // B+tree's degree, bptree_t >= 2
constexpr int MaxKeyCount = 2 * bptree_t;            // the max number of keys in a b+tree node
constexpr int MaxChildCount = 2 * bptree_t;          // the max number of child in a b+tree node

// 索引文件头信息结点
class IndexHeadNode
{
public:
	FileAddr    root;                               // the address of the root
	FileAddr    MostLeftNode;                              // the address of the most left node
	char        KeyType;                             // 关键字类型
	char        RecordInfo[RecordInfoLength];          // 记录字段相关信息
};

// define B+tree Node
enum class NodeType {ROOT, INNER, LEAF};
class BTNode
{
public:
	NodeType node_type;                              // node type
	int count_valid_key;                             // the number of key has stored in the node

	KeyAttr key[MaxKeyCount];                        // array of keys
	FileAddr children[MaxChildCount];                // if the node is not a leaf node, children store the children pointer
                                                     // otherwise it store record address;

	FileAddr next;                                   // if leaf node
	void PrintSelf();
};

class BTree
{
public:
	BTree(const std::string idx_name, const char _KeyType, const std::string _RecordInfo);          // 创建索引文件的B+树
	~BTree() { }
	FileAddr Search(KeyAttr search_key);                                        // 查找关键字是否已经存在
	bool Insert(KeyAttr k, FileAddr k_fd);                                      // 插入关键字k
	FileAddr UpdateKey(KeyAttr k, KeyAttr k_new);                               // 返回关键字对应的记录地址
	FileAddr Delete(KeyAttr k);                                                 // 返回该关键字记录在数据文件中的地址
	void PrintBTreeStruct();                                                    // 层序打印所有结点信息
	void PrintAllLeafNode();
private:
	FileAddr DeleteKeyAtInnerNode(FileAddr x, int i, KeyAttr key);              // x的下标为i的结点为叶子结点
	FileAddr DeleteKeyAtLeafNode(FileAddr x, int i, KeyAttr key);               // x的下标为i的结点为叶子结点
	void InsertNotFull(FileAddr x, KeyAttr k, FileAddr k_fd);
	void SplitChild(FileAddr x, int i, FileAddr y);                             // 分裂x的孩子结点x.children[i] , y
	FileAddr Search(KeyAttr search_key, FileAddr node_fd);                      // 判断关键字是否存在
	FileAddr SearchInnerNode(KeyAttr search_key, FileAddr node_fd);             // 在内部节点查找
	FileAddr SearchLeafNode(KeyAttr search_key, FileAddr node_fd);              // 在叶子结点查找
	BTNode *FileAddrToMemPtr(FileAddr node_fd);                                 // 文件地址转换为内存指针
	
private:
	int file_id;
	std::string str_idx_name;
	IndexHeadNode idx_head;
};

#endif
