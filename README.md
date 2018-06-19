# MiniSQL

**MiniSQL** is a  **Relational database**, written in Modern C/C++ style. 

- **项目名称**：MiniSQL 数据库系统设计与实现   
- **项目目标：**
  - 通过实验加深对DBMS及其内部实现技术的理解
  - 实践系统软件开发的工程化方法

## 程序模块说明

### Buffer模块

程序文件：Buffer.h  Buffer.cpp

#### 功能

- 使其他模块与物理磁盘完全隔开，使之完全转换为内存操作
- 提供系统缓冲， 模拟操作系统内存管理功能，加快数据文件的读取和写入

#### 接口实现

- 实现文件的创建和删除
- 实现数据（记录）在文件中的读取和写入
- 实现在文件中添加和删除数据时的空间自动管理
- 提供对上层模块的统一接口

#### 工作原理

- 所有对于文件的读写都通过Buffer类提供的接口实现
- Buffer类将磁盘文件以页的形式换入内存缓冲
- 当待读写的文件数据所在的文件页之前已经被读入内存，则直接对该缓冲数据进行操作
- 当待读写的文件数据所在的文件页不存在内存中
  - 如果内存缓冲页未满，则将需要读写的文件页读入内存缓冲页，执行读写操作
  - 如果内存缓冲页已满，则通过内存页置换算法，将部分之前被使用的内存页写回到原文件，将待读写的文件页换入内存页

#### 文件格式以及数据流动模型

**文件格式：**索引文件和数据文件有统一的文件格式

1. 每个文件都以若干固定大小的文件页的形式存在
2. 除了零号页面，所有文件页都由**页头**和**数据区**两部分组成
3. 页头由**页号**和**常驻内存标志**组成。页号表示该页在文件中的位置，常驻内存标志用于内存页置换算法
4. 零号文件页除了页号和常驻内存标志外，还拥有**文件头信息数据区**，文件头信息记录了该文件可以写入新数据的位置信息，以及被删除的数据的位置信息。此外，还有一块预留区域保留以后使用

![文件格式](C:\Users\Simple\Desktop\MiniSQL\Resource\文件格式.bmp)



**数据流通模型：**磁盘文件的数据被读入内存缓冲区，通过内存缓冲读写文件数据。

![数据流通模型](C:\Users\Simple\Desktop\MiniSQL\Resource\数据流通模型.bmp)

#### 数据写入的格式

任意类型的数据通过buffer写入磁盘文件时，都将被转化为**写入地址+原始数据**的形式重新写入文件。

比如：当在文件页号为12，页内偏移13（byte）的位置写入两个int(4byte)类型的数据10，14的时候，实际写入的数据如下(小端机器)：

| 文件内地址           | 数据（二进制值）                                   |
| -------------------- | -------------------------------------------------- |
| PageId:12  offset:13 | 00 00 00 0B\|00 00 00 0C\|0A 00 00 00\|0E 00 00 00 |



#### 数据结构

文件地址结构体，通过**页号**和**页内偏移**可以定位文件内的任意位置

```c++
/*********************************************************
*             文件地址,定位文件中的位置
**********************************************************/
class FileAddr
{
    ...
	unsigned long filePageID;     // 文件页编号
	unsigned int  offSet;         // 页内偏移量
	...
};
```

页头信息，标志该页在文件中的位置

```c++
/*********************************************************
*             页头信息，用以标识文件页
**********************************************************/
class PAGEHEAD
{
    ...
	unsigned long pageId;		// 页编号
	bool isFixed;				// 页是否常驻内存
    ...
};



```

文件头信息，记录该文件内的数据信息

```c++
/*********************************************************
*               文件头信息
**********************************************************/
class FILECOND
{
public:
	...
	FileAddr DelFirst;                         // 第一条被删除记录地址
	FileAddr DelLast;                          // 最后一条被删除记录地址  
	FileAddr NewInsert;                        // 文件末尾可插入新数据的地址
	unsigned long total_page;                  // 目前文件中共有页数
	char reserve[FILECOND_RESERVE_SPACE];      // 预留空间 
    ...
};
```

内存页类，每一个内存页都是某一文件的某一文件页在内存中的映射（缓冲），通过对内存页的读写，实现对磁盘文件的读写.同时内存页类记录着其对应的文件页自读入内存后的使用和读写操作，用以内存页置换算法。比如，某一文件页调入内存后被改写过，则该内存页释放时候，需要先将该内存页内容写回到文件，否则就不需要写回直接释放。

```C++
/*********************************************************
*
*   名称：  内存页类
*   功能：  提供保存文件页的空间，以及该页相关的信息
*   不变式：内存页的大小固定
*
**********************************************************/
class MemPage
{
    ...
	void Back2File() const;            // 把内存中的页写回到文件中
	bool SetModified();                // 设置为脏页

public:
	unsigned long fileId;              // 文件指针，while fileId==0 时为被抛弃的页
	unsigned long filePageID;          // 文件页号

	mutable bool bIsLastUsed;          // 最近一次访问内存是否被使用，用于Clock算法
	mutable bool isModified;           // 是否脏页

	void *Ptr2PageBegin;               // 实际保存物理文件数据的地址
	PAGEHEAD *pageHead;                // 页头指针
	FILECOND* GetFileCond()const;      // 文件头指针（while filePageID == 0）
};
```



内存页管理类，管理内存页资源。将需要读写的文件页读入内存页缓冲，将不再需要使用的内存页在适当的时候释放（这里的释放并不是指释放内存空间，而是将不再使用的内存页标志为空闲（需要写回先写回）以待后续使用）。

```C++
/*********************************************************
*
*   名称：  内存页管理类（Clock页面置换算法）
*   功能：  物理页面在内存中的缓存，加速对物理文件的读写
*   不变式：调用者保证需要被载入的物理文件都存在，且加载的页面不越界
*
**********************************************************/
class Clock
{
	friend class MemFile;
	friend class BUFFER;
	friend class BTree;
public:
	Clock();
	~Clock();
#ifndef NDEBUG
	void PrintFileNameInMemory();   // 打印所有在内存中有缓存的文件页
#endif
private:
	// 返回磁盘文件内存地址
	MemPage* GetMemAddr(unsigned long fileId, unsigned long filePageID);

	// 创建新页，适用于创建新文件或者添加新页的情况下
	MemPage* CreatNewPage(unsigned long fileId, unsigned long filePageID);

private:
	// 返回一个可替换的内存页索引
	// 原页面内容该写回先写回
	unsigned int GetReplaceablePage();

	// 如果目标文件页存在内存缓存则返回其地址，否则返回 nullptr
	MemPage* GetExistedPage(unsigned long fileId, unsigned long filePageID);
	MemPage* LoadFromFile(unsigned long fileId, unsigned long filePageID);

	// Clock置换算法
	unsigned long ClockSwap();

	 
private:
	MemPage* MemPages[MEM_PAGEAMOUNT+1];  // 内存页对象数组
};
```

内存文件类，内存文件类的实例对应磁盘文件的映射，调用内存页置换算法类Clock，读写磁盘文件。

```C++
/*********************************************************
*   名称：  内存文件类
*   功能：  通过物理文件在内存中的映射文件的操作，从而操作物理文件
*   不变式：假设所有被操作的文件都存在且已经打开
*   记录格式: 记录地址+记录数据
**********************************************************/
class MemFile
{
	friend class BUFFER;
	friend class BTree;
public:
	const void* ReadRecord(FileAddr *address_delete)const;         // 读取某条记录,返回记录指针(包括记录地址数据)
	FileAddr AddRecord(void*source_record, size_t sz_record);                        // 返回记录所添加的位置
	FileAddr DeleteRecord(FileAddr *address_delete, size_t record_sz);               // 返回删除的位置
	
private:
	// 构造
	MemFile(const char *file_name, unsigned long file_id);
	// 写入数据
	void* MemRead(FileAddr *mem_to_read);                           // 读取内存文件,返回读取位置指针
	FileAddr MemWrite(const void* source, size_t length);           // 在可写入地址写入数据
	FileAddr MemWrite(const void* source, size_t length, FileAddr* dest);
	
	void MemWipe(void*source, size_t sz_wipe, FileAddr *fd_to_wipe);

	MemPage * AddExtraPage();                                       // 当前文件添加一页空间
	MemPage* GetFileFirstPage();                                    // 得到文件首页

private:
	char fileName[MAX_FILENAME_LEN];
	unsigned long fileId;                                          // 文件指针
	unsigned long total_page;                                      // 目前文件中共有页数
};
```

Buffer类，通过调用内存文件类，实现对所有磁盘文件的读写操作。

```C++
class BUFFER
{
public:
	BUFFER() = default;
	~BUFFER();
	MemFile* operator[](const char *fileName);      // 打开文件，打开失败返回 nullptr

	void CreateFile(const char *fileName);          // 创建文件，并格式化
	void CloseFile(const char *FileName);
	void CloseAllFile();	
private:
	// 返回文件所映射的内存文件
	MemFile* GetMemFile(const char *fileName);
private:
	std::vector<MemFile*> memFiles;  // 保存已经打开的文件列表
};
```

### B+tree 模块

程序文件：bptree.h    bptree.cpp

#### 功能

- 通过B+树，创建和读写文件索引，提供数据库快速检索

#### 接口实现

- 创建和读写索引文件
- 实现对关键字的查找、插入和删除操作
- 打印B+树结点信息

#### 工作原理

**说明：**

关于B+树的具体定义，目前还有一些细节上的分歧。在本项目的B+树的实现中，**B+树按照结点关键字个数和孩子结点个数一样多的定义实现**。即在一个结点中，有N个关键字，k[0], k[1], ..., k[N-1]，就对应着N个孩子结点child[0], child[1], ..., child[N-1]。其中，k[0] <= child[0] < k[1] <= child[1], ..., <key[N-1] <= child[N-1]。

- B+树的相关算法。
- 参考：
  - [B+树介绍](https://www.cnblogs.com/wade-luffy/p/6292784.html)
  - [B+树的几点总结](https://www.jianshu.com/p/6f68d3c118d6)
  - [从MySQL Bug#67718浅谈B+树索引的分裂优化](http://hedengcheng.com/?p=525)
  - [B树和B+树的插入、删除图文详解](https://www.cnblogs.com/nullzx/p/8729425.html)

#### 数据结构

**索引文件信息结构体，该数据保存在文件头部的预留空间。**

- MostLeftNode指向最左边的叶子结点，用于遍历数据和按照范围查找关键字。
- RecordInfo记录了表中的一条记录的各个字段的类型和顺序。比如：

| RecordInfo数据 | 记录类型                                                     |
| -------------- | ------------------------------------------------------------ |
| IFD            | 一条记录三个字段，类型依次为：int,float,doouble              |
| IC10D          | 一条记录三个字段，类型依次为：int, char, double。其中char字段为10个字符的字符串 |



```C++
// 索引文件头信息结点
class IndexHeadNode
{
public:
	FileAddr    root;                               // the address of the root
	FileAddr    MostLeftNode;                       // the address of the most left node
	char        KeyType;                            // 关键字类型
	char        RecordInfo[RecordInfoLength];       // 记录字段相关信息
};
```

**B+树的结点结构体。**其中ROOT标志只在B+树的根结点关键字未满的情况下使用。在根节点分裂后，整个B+树只存在内结点和叶子结点两种类型。

```C++
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
```

B+树

```C++
class BTree
{
public:
	BTree(char *idx_name, char _KeyType = 'i', char *_RecordInfo="i");          // 创建索引文件的B+树
	~BTree() { delete idx_name; }
	FileAddr Search(KeyAttr search_key);                                        // 查找关键字是否已经存在
	bool Insert(KeyAttr k, FileAddr k_fd);                                      // 插入关键字k
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
	char *idx_name;
	int file_id;
	IndexHeadNode idx_head;
};
```

