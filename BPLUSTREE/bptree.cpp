#include "bptree.h"

BTree::BTree(char *_idx_name)
	:idx_name(_idx_name)
{
	auto &buffer = GetGlobalFileBuffer();
	auto pMemFile = buffer[_idx_name];

	// ��������ļ��������򴴽�
	if (!pMemFile)
	{
		// ���������ļ�
		buffer.CreateFile(_idx_name);
		pMemFile = buffer[_idx_name];

		// ����һ�����
		BTNode leaf_node;
		leaf_node.node_type = NodeType::LEAF;
		leaf_node.count_valid_key = 0;
		FileAddr leaf_node_fd= buffer[idx_name]->AddRecord(&leaf_node, sizeof(leaf_node));

		// �����ĵ�ַд���ļ�ͷ��Ԥ���ռ���
		memcpy(buffer[idx_name]->GetFileFirstPage()->GetFileCond()->reserve, &leaf_node_fd, sizeof(leaf_node_fd));
	}
	file_id = pMemFile->fileId;
}

FileAddr BTree::Search(KeyAttr search_key)
{
	auto pMemPage = GetGlobalClock()->GetMemAddr(file_id, 0);
	auto pfilefd = (FileAddr*)pMemPage->GetFileCond()->reserve;  // �ҵ������ĵ�ַ
	Search(search_key, *pfilefd);
}

FileAddr BTree::Search(KeyAttr search_key, FileAddr node_fd)
{
	BTNode* pNode = FileAddrToMemPtr(node_fd);

	if (pNode->node_type == NodeType::LEAF)
	{
		return SearchLeafNode(search_key, node_fd);
	}
	else
	{
		return SearchInnerNode(search_key, node_fd);
	}
}

FileAddr BTree::SearchInnerNode(KeyAttr search_key, FileAddr node_fd)
{
	FileAddr fd_res;
	BTNode* pNode = FileAddrToMemPtr(node_fd);
	for (int i = MaxKeyCount - 1; i >= 0; i--)
	{
		if (pNode->key[i] <= search_key)
		{
			fd_res = pNode->children[i];
			break;
		}
	}
	if (fd_res == FileAddr{ 0,0 })
	{
		return fd_res;
	}	
	else
	{
		BTNode* pNextNode = FileAddrToMemPtr(fd_res);
		if (pNextNode->node_type == NodeType::LEAF)
			return SearchLeafNode(search_key, fd_res);
		else
			SearchInnerNode(search_key, fd_res);
	}
}

FileAddr BTree::SearchLeafNode(KeyAttr search_key, FileAddr node_fd)
{

	BTNode* pNode = FileAddrToMemPtr(node_fd);
	for (int i = MaxKeyCount - 1; i >= 0; i--)
	{
		if (pNode->key[i] == search_key)
		{
			return pNode->children[i];
		}
	}
	return FileAddr{ 0,0 };
}

BTNode * BTree::FileAddrToMemPtr(FileAddr node_fd)
{
	auto pMemPage = GetGlobalClock()->GetMemAddr(file_id, node_fd.filePageID);

	return (BTNode*)((char*)pMemPage->Ptr2PageBegin + node_fd.offSet);
}
