#include "bptree.h"

BTree::BTree(char *_idx_name, char _KeyType, char *_RecordInfo)
	:idx_name(_idx_name)
{
	idx_name = (char*)malloc(strlen(_idx_name) + 1);
	strcpy(idx_name, _idx_name);
	
	auto &buffer = GetGlobalFileBuffer();
	auto pMemFile = buffer[_idx_name];

	// ��������ļ��������򴴽�
	if (!pMemFile)
	{
		// ���������ļ�
		buffer.CreateFile(_idx_name);
		pMemFile = buffer[_idx_name];

		// ��ʼ�������ļ�������һ�������
		BTNode root_node;
		if (sizeof(BTNode) > (FILE_PAGESIZE - sizeof(PAGEHEAD)))
			throw ERROR::BPLUSTREE_DEGREE_TOOBIG;
		root_node.node_type = NodeType::ROOT;
		root_node.count_valid_key = 0;
		root_node.next = FileAddr{ 0,0 };
		FileAddr root_node_fd= buffer[idx_name]->AddRecord(&root_node, sizeof(root_node));

		// ��ʼ�����������ļ�ͷ��Ϣ
		idx_head.root = root_node_fd;
		idx_head.MostLeftNode = root_node_fd;
		idx_head.KeyType = _KeyType;
		strcpy(idx_head.RecordInfo, _RecordInfo);

		// �����ĵ�ַд���ļ�ͷ��Ԥ���ռ���
		memcpy(buffer[idx_name]->GetFileFirstPage()->GetFileCond()->reserve, &idx_head, sizeof(idx_head));

	}
	file_id = pMemFile->fileId;
}

void BTree::DeleteKeyAtInnerNode(FileAddr x, int i, KeyAttr key)
{
	auto px = FileAddrToMemPtr(x);
	auto py = FileAddrToMemPtr(px->children[i]);

	if (py->node_type == NodeType::LEAF)
	{
		DeleteKeyAtLeafNode(x, i, key);
	}
	else
	{
		int j = py->count_valid_key-1;
		while (py->key[j] > key)j--;
		assert(j>=0);
		DeleteKeyAtInnerNode(px->children[i], j, key);
	}

	// �ж�ɾ����Ľ�����
	if (py->count_valid_key >= MaxKeyCount / 2)
		return;

	// ���ɾ����Ĺؼ��ָ���������B+���Ĺ涨�����ֵܽ�����key

	// ������ֵܴ������и���ؼ���
	if ((i <= px->count_valid_key - 2) && (FileAddrToMemPtr(px->children[i + 1])->count_valid_key > MaxKeyCount / 2))
	{
		auto RBrother = FileAddrToMemPtr(px->children[i + 1]);
		// �����Ĺؼ���
		auto key_bro = RBrother->key[0];
		auto fd_bro = RBrother->children[0];


		// �������ֵܵ��������
		px->key[i + 1] = RBrother->key[1];
		// �������ֵܽ��
		for (int j = 1; j <= RBrother->count_valid_key - 1; j++)
		{
			RBrother->key[j - 1] = RBrother->key[j];
			RBrother->children[j - 1] = RBrother->children[j];
		}
		RBrother->count_valid_key -= 1;

		// ���±�Ҷ�ӽ��
		py->key[py->count_valid_key] = key_bro;
		py->children[py->count_valid_key] = fd_bro;
		py->count_valid_key += 1;

		return;
	}

	// ������ֵܴ������и���ؼ���
	if (i > 0 && FileAddrToMemPtr(px->children[i - 1])->count_valid_key > MaxKeyCount / 2)
	{
		auto LBrother = FileAddrToMemPtr(px->children[i - 1]);
		// �����Ĺؼ���
		auto key_bro = LBrother->key[LBrother->count_valid_key - 1];
		auto fd_bro = LBrother->children[LBrother->count_valid_key - 1];

		// �������ֵܽ��
		LBrother->count_valid_key -=1;

		// ���±����
		px->key[i] = key_bro;
		for (int j = py->count_valid_key - 1; j > 0; j--)
		{
			py->key[j + 1] = py->key[j];
			py->children[j + 1] = py->children[j];
		}
		py->key[0] = key_bro;
		py->children[0] = fd_bro;

		py->count_valid_key += 1;

		return;
	}

	// ���ֵܽ����û�и����key,��ǰ�����ֵܽ��ϲ���һ���µ�Ҷ�ӽ�㣬��ɾ��������е�key

	// �����ֵܴ��ڽ���ϲ�
	if (i < px->count_valid_key - 2)
	{
		auto RBrother = FileAddrToMemPtr(px->children[i + 1]);
		for (int j = 0; j < RBrother->count_valid_key; j++)
		{
			py->key[py->count_valid_key] = RBrother->key[j];
			py->children[py->count_valid_key] = RBrother->children[j];
			py->count_valid_key++;
		}

		// ����next
		py->next = RBrother->next;
		// ɾ���ҽ��
		GetGlobalFileBuffer()[idx_name]->DeleteRecord(&px->children[i + 1], sizeof(BTNode));
		// ���¸��ڵ�����
		for (int j = i + 2; j < px->count_valid_key; j++)
		{
			px->key[j - 1] = px->key[j];
			px->children[j - 1] = px->children[j];
		}
		px->count_valid_key--;
	}
	else
	{// ������ϲ�
		auto LBrother = FileAddrToMemPtr(px->children[i - 1]);
		for (int j = 0; j < py->count_valid_key; j++)
		{
			LBrother->key[py->count_valid_key] = py->key[j];
			LBrother->children[py->count_valid_key] = py->children[j];
			LBrother->count_valid_key++;
		}

		// ����next
		LBrother->next = py->next;

		// ɾ�������
		GetGlobalFileBuffer()[idx_name]->DeleteRecord(&px->children[i], sizeof(BTNode));
		// ���¸��ڵ�����
		for (int j = i + 1; j < px->count_valid_key; j++)
		{
			px->key[j - 1] = px->key[j];
			px->children[j - 1] = px->children[j];
		}
		px->count_valid_key--;
	}

}

// �����ɾ���Ĺؼ����Ѿ�����
void BTree::DeleteKeyAtLeafNode(FileAddr x, int i, KeyAttr key)
{
	auto px = FileAddrToMemPtr(x);
	auto py = FileAddrToMemPtr(px->children[i]);
	
	int j = py->count_valid_key - 1;
	while (py->key[j] != key)j--;
	assert(j >= 0);
	// ɾ��Ҷ�ڵ�����С�Ĺؼ��֣����¸��ڵ�
	if (j == 0)
	{
		px->key[i] = py->key[j+1];
	}

	j++;
	while (j <= py->count_valid_key - 1)
	{
		py->children[j - 1] = py->children[j];
		py->key[j - 1] = py->key[j];
		j++;
	}
	py->count_valid_key -= 1;
}

// ��һ��������� x, ����ؼ��� k, k�����ݵ�ַΪ k_fd
void BTree::InsertNotFull(FileAddr x, KeyAttr k, FileAddr k_fd)
{
	auto px = FileAddrToMemPtr(x);
	int i = px->count_valid_key-1;

	// ����ý����Ҷ�ӽ�㣬ֱ�Ӳ���
	if (px->node_type == NodeType::LEAF|| px->node_type == NodeType::ROOT)
	{
		while (i >= 0 && k < px->key[i])
		{
			px->key[i + 1] = px->key[i];
			px->children[i + 1] = px->children[i];
			i--;
		}
		px->key[i + 1] = k;
		px->children[i + 1] = k_fd;
		px->count_valid_key += 1;
	}
	else
	{
		while (i >= 0 && k < px->key[i])  i = i - 1;

		// ��������ֵ���ڽڵ��ֵ��С
		if (i < 0){
			i = 0;
			px->key[i] = k;
		}
		assert(i >= 0);

		FileAddr ci = px->children[i];
		auto pci = FileAddrToMemPtr(ci);
		if (pci->count_valid_key == MaxKeyCount)
		{
			SplitChild(x, i, ci);
			if (k >= px->key[i + 1])
				i += 1;
		}
		InsertNotFull(px->children[i], k, k_fd);
	}
}

// ��x�±�Ϊi�ĺ�����������
void BTree::SplitChild(FileAddr x, int i, FileAddr y)
{
	auto pMemPageX = GetGlobalClock()->GetMemAddr(file_id, x.filePageID);
	auto pMemPageY = GetGlobalClock()->GetMemAddr(file_id, y.filePageID);
	pMemPageX->isModified = true;
	pMemPageY->isModified = true;

	BTNode*px = FileAddrToMemPtr(x);
	BTNode*py = FileAddrToMemPtr(y);
	BTNode z;         // ���ѳ������½��
	FileAddr z_fd;    // �½����ļ��ڵ�ַ
	
	z.node_type = py->node_type;
	z.count_valid_key = MaxKeyCount / 2;

	// ��y����һ������ת�Ƶ��½��
	for (int i = MaxKeyCount / 2; i < MaxKeyCount; i++)
	{
		z.key[i - MaxKeyCount / 2] = py->key[i];
		z.children[i - MaxKeyCount / 2] = py->children[i];
	}
	py->count_valid_key = MaxKeyCount / 2;

	// ��y�ĸ��ڵ�x������´������ӽ�� z
	int j;
	for ( j= px->count_valid_key-1; j> i; j--)
	{
		px->key[j+1] = px->key[j];
		px->children[j+1] = px->children[j];
	}
	
	j++; // after j++, j should be i+1;
	px->key[j] = z.key[0];

	if (py->node_type == NodeType::LEAF)
	{
		z.next = py->next;
		z_fd = GetGlobalFileBuffer()[idx_name]->AddRecord(&z, sizeof(z));
		py->next = z_fd;
	}
	else
		z_fd = GetGlobalFileBuffer()[idx_name]->AddRecord(&z, sizeof(z));
	
	px->children[j] = z_fd;
	px->count_valid_key++;
}

FileAddr BTree::Search(KeyAttr search_key)
{
	auto pMemPage = GetGlobalClock()->GetMemAddr(file_id, 0);
	auto pfilefd = (FileAddr*)pMemPage->GetFileCond()->reserve;  // �ҵ������ĵ�ַ
	return Search(search_key, *pfilefd);
}

FileAddr BTree::Search(KeyAttr search_key, FileAddr node_fd)
{
	BTNode* pNode = FileAddrToMemPtr(node_fd);

	if (pNode->node_type == NodeType::LEAF|| pNode->node_type == NodeType::ROOT)
	{
		return SearchLeafNode(search_key, node_fd);
	}
	else
	{
		return SearchInnerNode(search_key, node_fd);
	}
}

bool BTree::Insert(KeyAttr k, FileAddr k_fd)
{
	// ����ùؼ����Ѿ����������ʧ��
	try
	{
		auto key_fd = Search(k);
		if (key_fd != FileAddr{ 0,0 })
			throw ERROR::KEY_INSERT_FAILED;
	}
	catch (const ERROR error)
	{
		DispatchError(error);
		std::cout << std::endl;
		return false;
	}

	// �õ�������fd
	FileAddr root_fd = *(FileAddr*)GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->GetFileCond()->reserve;
	auto proot = FileAddrToMemPtr(root_fd);
	if (proot->count_valid_key == MaxKeyCount)
	{
		// �����µĽ�� s ,��Ϊ�����
		BTNode s;
		s.node_type = NodeType::INNER;  // ֻ�г�ʼ����ʹ�� NodeType::ROOT
		s.count_valid_key = 1;
		s.key[0] = proot->key[0];
		s.children[0] = root_fd;
		FileAddr s_fd = GetGlobalFileBuffer()[idx_name]->AddRecord(&s, sizeof(BTNode));

		// ���µĸ��ڵ��ļ���ַд��
		*(FileAddr*)GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->GetFileCond()->reserve = s_fd;
		GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->isModified = true;

		//���ɵĸ��������ΪҶ�ӽ��
		auto pOldRoot = FileAddrToMemPtr(root_fd);
		if(pOldRoot->node_type == NodeType::ROOT)
			pOldRoot->node_type = NodeType::LEAF;

		// �ȷ����ٲ���
		SplitChild(s_fd, 0, s.children[0]);
		InsertNotFull(s_fd, k, k_fd);
	}
	else
	{
		InsertNotFull(root_fd, k, k_fd);
	}
	return true;
}

void BTree::Delete(KeyAttr key)
{
	auto search_res = Search(key);
	if (search_res.offSet == 0)
		return;

	// �õ�������fd
	FileAddr root_fd = *(FileAddr*)GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->GetFileCond()->reserve;
	auto proot = FileAddrToMemPtr(root_fd);

	if (proot->node_type == NodeType::ROOT|| proot->node_type == NodeType::LEAF)
	{
		// ֱ��ɾ��
		int j = proot->count_valid_key - 1;
		while (proot->key[j] != key)j--;
		assert(j >= 0);
		for (j++; j < proot->count_valid_key; j++)
		{
			proot->key[j - 1] = proot->key[j];
			proot->children[j - 1] = proot->children[j];
		}
		proot->count_valid_key--;
		return;
	}

	int i = proot->count_valid_key - 1;
	while (proot->key[i] > key)i--;
	assert(i >= 0);
	auto px = FileAddrToMemPtr(root_fd);
	auto py = FileAddrToMemPtr(px->children[i]);

	if (py->node_type == NodeType::LEAF)
	{
		DeleteKeyAtLeafNode(root_fd, i, key);
	}
	else
	{
		DeleteKeyAtInnerNode(root_fd, i, key);
		
	}


	if (proot->count_valid_key == 1)
	{
		// ���µĸ��ڵ��ļ���ַд��
		*(FileAddr*)GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->GetFileCond()->reserve = proot->children[0];
		GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->isModified = true;

		GetGlobalFileBuffer()[idx_name]->DeleteRecord(&root_fd, sizeof(BTNode));
	}
}

void BTree::PrintBTree()
{
	std::queue<FileAddr> fds;
	static int n = 0;
	// �õ�������fd
	FileAddr root_fd = *(FileAddr*)GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->GetFileCond()->reserve;
	auto pRoot = FileAddrToMemPtr(root_fd);
	if (pRoot->node_type == NodeType::ROOT)
	{
		for (int i = 0; i < pRoot->count_valid_key; i++)
		{
			n++;
			std::cout << pRoot->key[i];
		}
		std::cout << std::endl << "total nodes: " << n << std::endl;
		return;
	}
	fds.push(root_fd);
	while (!fds.empty())
	{
		// ��ӡ�ý������еĹؼ���
		FileAddr tmp = fds.front();
		fds.pop();
		auto pNode = FileAddrToMemPtr(tmp);

		if (pNode->node_type != NodeType::LEAF)
		{
			for (int i = 0; i < pNode->count_valid_key; i++)
			{
				fds.push(pNode->children[i]);
			}
		}
		else
		{
			for (int i = 0; i < pNode->count_valid_key; i++)
			{
				n++;
				std::cout << pNode->key[i];
			}
		}
	}
	std::cout << std::endl << "total nodes: " << n << std::endl;
}

void BTree::Print()
{
	auto phead = (IndexHeadNode*)GetGlobalFileBuffer()[idx_name]->GetFileFirstPage()->GetFileCond()->reserve;
	auto pNode = FileAddrToMemPtr(phead->MostLeftNode);
	static int n = 0;
	while (pNode->next.offSet != 0)
	{
		for (int i = 0; i < pNode->count_valid_key; i++)
		{
			n++;
			std::cout << pNode->key[i];
		}
			
		pNode = FileAddrToMemPtr(pNode->next);
	}
	for (int i = 0; i < pNode->count_valid_key; i++)
	{
		n++;
		std::cout << pNode->key[i];
	}
	std::cout << std::endl << n << std::endl;
}

FileAddr BTree::SearchInnerNode(KeyAttr search_key, FileAddr node_fd)
{
	FileAddr fd_res{0,0};
	BTNode* pNode = FileAddrToMemPtr(node_fd);
	for (int i = pNode->count_valid_key - 1; i >= 0; i--)
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
			return SearchInnerNode(search_key, fd_res);
	}
	return fd_res;
}

FileAddr BTree::SearchLeafNode(KeyAttr search_key, FileAddr node_fd)
{

	BTNode* pNode = FileAddrToMemPtr(node_fd);
	for (int i = 0; i <pNode->count_valid_key; i++)
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
	pMemPage->isModified = true;
	return (BTNode*)((char*)pMemPage->Ptr2PageBegin + node_fd.offSet+sizeof(FileAddr));
}

std::ostream& operator<<(std::ostream &os, const KeyAttr &key)
{
	os << key.x << " ";
	return os;
}


void BTreeTest()
{
	using std::string;
	using std::vector;
	auto &buffer = GetGlobalFileBuffer();
	string idx_name = "test.idx";
	string dbf_name = "test.dbf";

	// ɾ�����е��ļ�
	remove(idx_name.c_str());
	remove(dbf_name.c_str());

	// �����ļ�
	//buffer.CreateFile(idx_name.c_str());  //�����ļ���Ҫ���������������ܳ�ʼ��b+���ĸ����
	buffer.CreateFile(dbf_name.c_str());

	// ����������
	BTree tree(const_cast<char*>(idx_name.c_str()));

	// ��������ؼ���
	srand(time(time_t(0)));
	const int key_count = 8;
	vector<KeyAttr> keys;
	vector<FileAddr> rec_fds;
	for (int i = 0; i < key_count; i++)
	{
		if (i == 14)
			int a = 1;
		KeyAttr key;
		key.x = rand();
		key.s[0] = rand()%32+32;
		key.s[1] = '\0';
		keys.push_back(key);
		FileAddr fd = buffer[dbf_name.c_str()]->AddRecord(&key, sizeof(key));
		rec_fds.push_back(fd);

		bool b_insert = tree.Insert(key, fd);
		if (!b_insert)
		{
			rec_fds.pop_back();
			buffer[dbf_name.c_str()]->DeleteRecord(&fd, sizeof(key));
			i--;
		}
	}

	// ����������д���ı��ļ�
	using std::fstream;
	using std::ios;
	std::ofstream output;
	output.open("test.txt");
	for (int i = 0; i < key_count; i++)
	{
		output << keys[i].x << "\t" << keys[i].s << "\tfd: " << rec_fds[i].filePageID << "  " << rec_fds[i].offSet << std::endl;
	}
	output.close();
	tree.Print();
	//ʹ�������ļ�����
	char c = 'c';
	while (1)
	{
		fflush(stdin);
		std::cout << "operator: insert(i), delete(d), quit(q)" << std::endl;
		c = getchar();
		if (c == 'd')
		{
			std::cout << "delete key" << std::endl;
			KeyAttr key;
			int x;
			std::cin >> x;
			key.x = x;
			tree.Delete(key);

			
		}
		if (c == 'i')
		{
			std::cout << "insert key" << std::endl;
			KeyAttr key;
			int x;
			std::cin >> x;
			key.x = x;
			tree.Insert(key, FileAddr{11,11});
		}

		std::cout << "�����б�"<<std::endl;
		tree.Print();
		std::cout << std::endl;

		if (c == 'q')
		{
			break;
		}
		
		//auto start = std::chrono::system_clock::now();
		//auto fd = tree.Search(key);
		//auto end = std::chrono::system_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		//if (fd.offSet == 0)
		//{
		//	std::cout << "�ؼ��ֲ�����";
		//}
		//else
		//{
		//	FileAddr*p = (FileAddr*)buffer[dbf_name.c_str()]->ReadRecord(&fd);
		//	std::cout << "���ҽ��" << std::endl;
		//	std::cout << p->filePageID << "  " << p->offSet << std::endl;
		//	std::cout << "������"
		//		<< double(duration.count()) //* std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
		//		<< "΢��" << std::endl;
		//}
		
		//std::cout << "���������:";
		fflush(stdin);
		fflush(stdin);
		fflush(stdin);
		char x = getchar();
		
	}
	tree.Print();
}


