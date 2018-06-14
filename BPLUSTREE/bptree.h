/******************************************************************

** �ļ���: bptree.h

** Copyright (c)

** ������: ReFantasy/TDL

** ��  ��: 2018-6-13

** ��  ��: ʵ�� B+tree ���������ļ�

** ��  ��: 1.00

******************************************************************/
#ifndef __BPTREE_H__
#define __BPTREE_H__
#include "../BUFFER/Buffer.h"
#include "../GLOBAL/global.h"
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <chrono>

class KeyAttr
{
public:
	int x;
	char s[10];
	bool operator<(const KeyAttr &rhs) { return x < rhs.x; }
	bool operator>(const KeyAttr &rhs) { return x > rhs.x; }
	bool operator==(const KeyAttr &rhs) { return x == rhs.x; }
	bool operator<=(const KeyAttr &rhs) { return x <= rhs.x; }
	bool operator>=(const KeyAttr &rhs) { return x >= rhs.x; }
};
std::ostream& operator<<(std::ostream &os, const KeyAttr &key);

constexpr int bptree_t = 60;                      // B+tree's degree, bptree_t >= 2
constexpr int MaxKeyCount = 2 * bptree_t;      // the max number of keys in a b+tree node
constexpr int MaxChildCount = 2 * bptree_t;        // the max number of child in a b+tree node

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
};

class BTree
{
public:
	BTree(char *idx_name);                                     // ���������ļ���B+��
	~BTree() { delete idx_name; }
	FileAddr Search(KeyAttr search_key);                       // ���ҹؼ����Ƿ��Ѿ�����
	void Insert(KeyAttr k, FileAddr k_fd);                           //����ؼ���k
	void PrintBTree();                                         // �����ӡ���н����Ϣ
private:
	void InsertNotFull(FileAddr x, KeyAttr k, FileAddr k_fd);
	void SplitChild(FileAddr x, int i, FileAddr y);                  // ����x�ĺ��ӽ��x.children[i] , y
	FileAddr Search(KeyAttr search_key, FileAddr node_fd);                          // �жϹؼ����Ƿ����
	FileAddr SearchInnerNode(KeyAttr search_key, FileAddr node_fd);              // ���ڲ��ڵ����
	FileAddr SearchLeafNode(KeyAttr search_key, FileAddr node_fd);               // ��Ҷ�ӽ�����
	BTNode *FileAddrToMemPtr(FileAddr node_fd);                                  // �ļ���ַת��Ϊ�ڴ�ָ��
private:
	char *idx_name;
	int file_id;
};

void BTreeTest();
#endif
