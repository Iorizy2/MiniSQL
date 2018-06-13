#include "bptree.h"
IndexFileHeadManage::IndexFileHeadManage(const char*_idx_file, char* KeyStr)
{
	idx_file = (char*)malloc(strlen(_idx_file));
	strcpy(idx_file, _idx_file);

	auto& buffer = GetGlobalFileBuffer();
	auto pMemPage = buffer[idx_file];
	// 索引文件还不存在则创建
	if (!pMemPage)
	{
		buffer.CreateFile(_idx_file);
		CreateIdxHead(idx_file);
	}
}

void IndexFileHeadManage::CreateIdxHead(char* KeyStr)
{
	auto& buffer = GetGlobalFileBuffer();

	SetKeyInfo(KeyStr);    // calculate the Keysize and KeyAttrNum

	auto pMemPage= buffer[idx_file]->GetFileFirstPage();
	auto pIdxHead = (IndexFileHead*)pMemPage->GetFileCond()->reserve;
	pIdxHead->ROOT.filePageID = 0;
	pIdxHead->ROOT.offSet = sizeof(PAGEHEAD) + sizeof(FILECOND) - FILECOND_RESERVE_SPACE;

	pIdxHead->start.filePageID = 0;
	pIdxHead->start.offSet = 0;
	
	KeyInfo = KeyStr;
}


FileAddr IndexFileHeadManage::GetRoot()
{
	auto& buffer = GetGlobalFileBuffer();
	auto pMemPage = buffer[idx_file]->GetFileFirstPage();
	auto pIdxHead = (IndexFileHead*)pMemPage->GetFileCond()->reserve;
	return pIdxHead->ROOT;
}

void IndexFileHeadManage::SetKeyInfo(char* KeyStr)
{
	auto& buffer = GetGlobalFileBuffer();
	auto pMemPage = buffer[idx_file]->GetFileFirstPage();
	auto pIdxHead = (IndexFileHead*)pMemPage->GetFileCond()->reserve;


	pIdxHead->FieldsNum = 0;		 // the number of fields(attributes) in primary key
	pIdxHead->KeySize = 0;         // sizeof(Key_Attr) * KeyAttrNum
	char* temp = KeyStr;
	for (int i = 0; temp[i] != 0; i++)
	{
		pIdxHead->FieldsNum++;
		switch (temp[i])
		{
			case 'i': pIdxHead->KeySize += sizeof(int);     break;
			case 'f': pIdxHead->KeySize += sizeof(float);   break;
			case 'c':
			{
				//deal with the max length in a string defined by the user
				int num = 0;
				for (i = i + 1; isdigit(temp[i]); i++)
				{
					num = num * 10 + temp[i] - '0';
				}
				i--;        // move the index back
				num++;      // for '\0' which is the end of a string
				pIdxHead->KeySize += num;
				break;
			}
			default: throw(1021);  //1021 字段类行错误
		}
	}
}

BTree::BTree(const char*idx_file, char* KeyStr)
	:idx_head(idx_file, KeyStr)
{

}

KeyPostion BTree::search(NodePointer x, KeyAttr k)
{
	auto& buffer = GetGlobalFileBuffer();
	BTreeNode *pnode = (BTreeNode *)buffer[idx_head.idx_file]->ReadRecord(&x);
	int i = 1;
	while (i <= MaxKeyCount && k > pnode->key[i])
		i += 1;
	if (i <= MaxKeyCount && k == pnode->key[i])
		return KeyPostion{ x, i };
}
