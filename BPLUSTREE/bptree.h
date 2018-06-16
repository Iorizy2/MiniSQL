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
const int RecordInfoLength = 32;  //��¼�����ֶ�����
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

// �����ļ�ͷ��Ϣ���
class IndexHeadNode
{
public:
	FileAddr    root;                               // the address of the root
	FileAddr    MostLeftNode;                              // the address of the most left node
	char        KeyType;                             // �ؼ�������
	char        RecordInfo[RecordInfoLength];          // ��¼�ֶ������Ϣ
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
};

class BTree
{
public:
	BTree(char *idx_name, char _KeyType = 'i', char *_RecordInfo="i");                                                      // ���������ļ���B+��
	~BTree() { delete idx_name; }
	FileAddr Search(KeyAttr search_key);                                        // ���ҹؼ����Ƿ��Ѿ�����
	bool Insert(KeyAttr k, FileAddr k_fd);                                      // ����ؼ���k
	void Delete(KeyAttr k);
	void PrintBTree();                                                          // �����ӡ���н����Ϣ
	void Print();
private:
	void DeleteKeyAtInnerNode(FileAddr x, int i, KeyAttr key);             // x���±�Ϊi�Ľ��ΪҶ�ӽ��
	void DeleteKeyAtLeafNode(FileAddr x, int i, KeyAttr key);             // x���±�Ϊi�Ľ��ΪҶ�ӽ��
	void InsertNotFull(FileAddr x, KeyAttr k, FileAddr k_fd);
	void SplitChild(FileAddr x, int i, FileAddr y);                              // ����x�ĺ��ӽ��x.children[i] , y
	FileAddr Search(KeyAttr search_key, FileAddr node_fd);                       // �жϹؼ����Ƿ����
	FileAddr SearchInnerNode(KeyAttr search_key, FileAddr node_fd);              // ���ڲ��ڵ����
	FileAddr SearchLeafNode(KeyAttr search_key, FileAddr node_fd);               // ��Ҷ�ӽ�����
	BTNode *FileAddrToMemPtr(FileAddr node_fd);                                  // �ļ���ַת��Ϊ�ڴ�ָ��
	
private:
	char *idx_name;
	int file_id;
	IndexHeadNode idx_head;
};

void BTreeTest();
#endif
