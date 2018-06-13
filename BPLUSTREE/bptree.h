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

class KeyAttr
{
public:
	bool operator<(const KeyAttr &rhs) {}
	bool operator>(const KeyAttr &rhs) {}
	bool operator==(const KeyAttr &rhs) {}
	bool operator<=(const KeyAttr &rhs) {}
};

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
	FileAddr Search(KeyAttr search_key);
private:
	FileAddr Search(KeyAttr search_key, FileAddr node_fd);                          // �жϹؼ����Ƿ����
	FileAddr SearchInnerNode(KeyAttr search_key, FileAddr node_fd);              // ���ڲ��ڵ����
	FileAddr SearchLeafNode(KeyAttr search_key, FileAddr node_fd);               // ��Ҷ�ӽ�����
	BTNode *FileAddrToMemPtr(FileAddr node_fd);                                  // �ļ���ַת��Ϊ�ڴ�ָ��
private:
	char *idx_name;
	int file_id;
};
#endif
