#pragma once

/*

Description: i节点的有关定义和操作

*/


#include <ctime>
#include "FSParams.h"

struct INode;

typedef struct INode			// 磁盘i节点	
{
	// 基本信息
	blockId_t blockId;	// Block ID
	inodeId_t iNodeId;	// 当前i节点的id
	//blockAddr_t	blockLocation;	// 存在当前磁盘块的哪个位置
	fMode_t mode;		// (file type), default = FILE_DEFAULT
	unsigned hlinkNum;	// hard link count, default = 1
	userId_t owner;		// owner user id, default = -1

	// 计算文件大小用
	int blocks;				// full block count (default=-1)
	blockSize_t rem_bytes;	// remain bytes (default = BLOCK_SIZE)

	// 文件属性
	time_t atime;			// last access time 上次访问的时间
	time_t mtime;			// last modify time 上次的修改时间
	blockId_t direct_data[INODE_DIRECT_CNT];	// direct datablocks
	blockId_t inDirect1, inDirect2;				// inDirect datablocks

	// 构造函数、拷贝函数
	INode();
	INode(inodeId_t id, fMode_t Type, userId_t uid);
	INode(INode& inode);

	// 检查权限函数
	bool check_AuthorityRead(userId_t t);
	bool check_AuthorityWrite(userId_t t);
	bool check_AuthorityX(userId_t t);

	// 判断文件类别
	bool isDir();
	bool isFile();
	bool isLink();

	// 返回的是文件大小
	diskSize_t size();


}INode;

typedef struct HashINode
{
	bool changed;		// 是否更改
	INode inode;		// 哈希链中的i节点
	HashINode* next;	// 下一个i节点

	// 构造函数
	HashINode() : changed(false), inode(), next(NULL) {}
	// 构造函数
	HashINode(INode node) : changed(false), inode(node), next(NULL) {}
	// 根据HashINode返回INode
	INode& get_INodeFromHINode(HashINode& hashINode);
	// 判断是否修改
	bool file_Ischanged(HashINode& hashINode);

}HashINode;

typedef struct HashINodeHead // 队列头
{
	HashINode* start;

	int num;  // 标记当前链中有几个 

	HashINodeHead() : start(NULL), num(0) {}

}HashINodeHead;

typedef struct HashINodeTable {	//哈希表

	HashINodeHead iNodeHashChain[32];

	// 创立一个HashINode
	HashINode* create_HashINode(INode& node);
	// 将INode插入哈希表中
	HashINode* insert_INode(INode& inode);
	// 判断是否有一个HashINode节点
	HashINode* find_HashINode(inodeId_t id);
	// 返回一个HashINode节点
	bool get_HashINode(HashINode* hinode, inodeId_t id);
	// 返回一个INode节点
	bool get_INode(INode* inode, inodeId_t id);
	// 删除一个HashINode节点，注意该删除应该级联删除内存i节点和磁盘i节点
	bool delete_HashINode(inodeId_t id);


}HashINodeTable;