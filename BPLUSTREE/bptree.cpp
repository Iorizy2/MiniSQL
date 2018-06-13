#include "bptree.h"

BTree::BTree(char *_idx_name)
	:idx_name(_idx_name)
{
	auto &buffer = GetGlobalFileBuffer();
	auto pMemFile = buffer[_idx_name];

	// 如果索引文件不存在则创建
	if (!pMemFile)
	{
		// 创建索引文件
		buffer.CreateFile(_idx_name);
		pMemFile = buffer[_idx_name];

		// 创建一个结点
		BTNode leaf_node;
		leaf_node.node_type = NodeType::LEAF;
		leaf_node.count_valid_key = 0;
		FileAddr leaf_node_fd= buffer[idx_name]->AddRecord(&leaf_node, sizeof(leaf_node));

		// 将结点的地址写入文件头的预留空间区
		memcpy(buffer[idx_name]->GetFileFirstPage()->GetFileCond()->reserve, &leaf_node_fd, sizeof(leaf_node_fd));
	}
	file_id = pMemFile->fileId;
}

FileAddr BTree::Search(KeyAttr search_key)
{
	auto pMemPage = GetGlobalClock()->GetMemAddr(file_id, 0);
	auto pfilefd = (FileAddr*)pMemPage->GetFileCond()->reserve;  // 找到根结点的地址
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
