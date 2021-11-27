/*

Description: i节点的有关操作

*/

#include "INode.h"
#include <string.h>
#include <iostream>
#include "FSParams.h"

using namespace std;

/*
	磁盘i节点的操作
*/

INode::INode()  //构造函数
{
	hlinkNum = 1; // hard link count, default = 1
	iNodeId = 0;		// i节点号
	//blockId = iNodeIdCurrent % 32;	// 块号
	//++iNodeIdCurrent;
	//blockLocation = 0;

	mode = FILE_DEFAULT;
	owner = -1;

	blocks = 0;
	rem_bytes = 0;

	atime = mtime = time(NULL);
	for (int i = 0; i < 10; ++i) {
		direct_data[i] = 0;
	}
	inDirect1 = inDirect2 = 0;

}

INode::INode(inodeId_t id, fMode_t Type, userId_t uid) // 构造函数
{
	owner = uid;
	hlinkNum = 1;	// hard link count, default = 1
	iNodeId = id;		// i节点号
	//blockId = iNodeId % 32;	// 块号

	//blockLocation = 0;
	blocks = 0;
	rem_bytes = 0;

	atime = mtime = time(NULL);
	for (int i = 0; i < 10; ++i) {
		direct_data[i] = 0;
	}
	inDirect1 = inDirect2 = 0;

	if (Type == FILE_TYPE_DIR) {		// 目录文件

		this->mode = Type | DIR_DEFAULT;
	}
	else if (Type == FILE_TYPE_FILE) {	// 普通文件

		this->mode = Type | FILE_DEFAULT;
	}
	else {

		this->mode = Type | OWNER_ALL | PUBLIC_R | PUBLIC_X;
	}

}

// 拷贝函数
INode::INode(INode& inode)
{
	// 基本信息
	blockId = inode.blockId;	// Block ID
	iNodeId = inode.iNodeId;	// 当前i节点的id
	//blockLocation = inode.blockLocation;	// 存在当前磁盘块的哪个位置
	mode = inode.mode;			// (file type) (owner_rwx_flags) (other_rwx_flags)
	hlinkNum = inode.hlinkNum;	// hard link count, default = 1
	owner = inode.owner;		// owner user id

	// 计算文件大小用
	blocks = inode.blocks;			// full block count (default=-1)
	rem_bytes = inode.rem_bytes;	// remain bytes (default = BLOCK_SIZE)

	// 文件属性
	atime = inode.atime;			// last access time 上次访问的时间
	mtime = inode.mtime;			// last modify time 上次的修改时间
	// direct_data[INODE_DIRECT_CNT]  direct datablocks

	for (int i = 0; i < 10; ++i) {
		direct_data[i] = inode.direct_data[i];
	}

	inDirect1 = inode.inDirect1;	// inDirect datablocks
	inDirect2 = inode.inDirect2;	// 
}

// 文件大小
diskSize_t INode::size()
{
	return (diskSize_t)blocks * Block_Size + rem_bytes;
}


bool INode::check_AuthorityRead(userId_t t) 	// 检查权限
{
	return t == (t == owner ? (mode & OWNER_R) : (mode & PUBLIC_R));
}

bool INode::check_AuthorityWrite(userId_t t)
{
	return t == (t == owner ? (mode & OWNER_W) : (mode & PUBLIC_W));
}

bool INode::check_AuthorityX(userId_t t)
{
	return t == (t == owner ? (mode & OWNER_X) : (mode & PUBLIC_X));
}

bool INode::isDir() //是否是目录文件
{
	return (mode & FILE_TYPE_ALL) == FILE_TYPE_DIR;
}

bool INode::isFile() //是否是普通文件
{
	return (mode & FILE_TYPE_ALL) == FILE_TYPE_FILE;
}

bool INode::isLink() //是否是链接文件
{
	return (mode & FILE_TYPE_ALL) == FILE_TYPE_LINK;
}


/*
   HashINode 哈希节点的操作
*/

// 根据HashINode返回INode
INode& HashINode::get_INodeFromHINode(HashINode& hashINode)
{
	return hashINode.inode;
}

// 判断是否修改
bool HashINode::file_Ischanged(HashINode& hashINode)
{
	return hashINode.changed;
}


/*
   HashINodeTable 哈希表的操作
*/

// 创立一个HashINode
HashINode* HashINodeTable::create_HashINode(INode& node)
{
	HashINode* hinode = new HashINode(node);
	hinode->changed = false;
	hinode->next = NULL;

	return hinode;
}

// 将INode插入哈希表中
HashINode* HashINodeTable::insert_INode(INode& inode)
{
	HashINode* hinode = create_HashINode(inode);

	// 取余算哈希队列号
	int raw = inode.iNodeId % 32;
	int temp = iNodeHashChain[raw].num;
	// 空位置
	HashINode* p = iNodeHashChain[raw].start; // 循环找到
	if (temp == 0) {
		iNodeHashChain[raw].start = hinode;
	}
	else {
		while (--temp) {
			p = p->next;
		}
		p->next = hinode;
	}

	++iNodeHashChain[raw].num;

	return hinode;

}

// 判断是否有一个HashINode节点
HashINode* HashINodeTable::find_HashINode(inodeId_t id)
{
	int raw = (int)id % 32;

	HashINode* p = iNodeHashChain[raw].start; // 循环找到
	if (p == NULL) {
		return p;
	}


	int temp = iNodeHashChain[raw].num; // num=1 时，直接就是start
	do {
		if (p->inode.iNodeId == id) {
			return p;
		}
		p = p->next;
		--temp;
	} while (temp);

	return NULL; // 没找到
}

// 返回一个HashINode节点
bool HashINodeTable::get_HashINode(HashINode* hinode, inodeId_t id)
{
	HashINode* p = find_HashINode(id);
	if (p != NULL) {
		hinode = p;
		return true;
	}

	return false;

}

// 返回一个INode节点
bool HashINodeTable::get_INode(INode* inode, inodeId_t id)
{
	HashINode* hinode = new HashINode();
	if (get_HashINode(hinode, id)) {
		*inode = hinode->inode;
		return true;
	}
	return false;
}

// 删除一个HashINode节点，注意该删除应该级联删除内存i节点和磁盘i节点
bool HashINodeTable::delete_HashINode(inodeId_t id)
{
	int raw = (int)id % 32;

	HashINode* p = find_HashINode(id);
	if (p != NULL) {
		free(p);	// 释放
		p = NULL;
		--iNodeHashChain[raw].num;	//修改数量

		return true;
	}

	return false;

}


