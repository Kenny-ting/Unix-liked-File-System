#pragma once

/*

Description: i�ڵ���йض���Ͳ���

*/


#include <ctime>
#include "FSParams.h"

struct INode;

typedef struct INode			// ����i�ڵ�	
{
	// ������Ϣ
	blockId_t blockId;	// Block ID
	inodeId_t iNodeId;	// ��ǰi�ڵ��id
	//blockAddr_t	blockLocation;	// ���ڵ�ǰ���̿���ĸ�λ��
	fMode_t mode;		// (file type), default = FILE_DEFAULT
	unsigned hlinkNum;	// hard link count, default = 1
	userId_t owner;		// owner user id, default = -1

	// �����ļ���С��
	int blocks;				// full block count (default=-1)
	blockSize_t rem_bytes;	// remain bytes (default = BLOCK_SIZE)

	// �ļ�����
	time_t atime;			// last access time �ϴη��ʵ�ʱ��
	time_t mtime;			// last modify time �ϴε��޸�ʱ��
	blockId_t direct_data[INODE_DIRECT_CNT];	// direct datablocks
	blockId_t inDirect1, inDirect2;				// inDirect datablocks

	// ���캯������������
	INode();
	INode(inodeId_t id, fMode_t Type, userId_t uid);
	INode(INode& inode);

	// ���Ȩ�޺���
	bool check_AuthorityRead(userId_t t);
	bool check_AuthorityWrite(userId_t t);
	bool check_AuthorityX(userId_t t);

	// �ж��ļ����
	bool isDir();
	bool isFile();
	bool isLink();

	// ���ص����ļ���С
	diskSize_t size();


}INode;

typedef struct HashINode
{
	bool changed;		// �Ƿ����
	INode inode;		// ��ϣ���е�i�ڵ�
	HashINode* next;	// ��һ��i�ڵ�

	// ���캯��
	HashINode() : changed(false), inode(), next(NULL) {}
	// ���캯��
	HashINode(INode node) : changed(false), inode(node), next(NULL) {}
	// ����HashINode����INode
	INode& get_INodeFromHINode(HashINode& hashINode);
	// �ж��Ƿ��޸�
	bool file_Ischanged(HashINode& hashINode);

}HashINode;

typedef struct HashINodeHead // ����ͷ
{
	HashINode* start;

	int num;  // ��ǵ�ǰ�����м��� 

	HashINodeHead() : start(NULL), num(0) {}

}HashINodeHead;

typedef struct HashINodeTable {	//��ϣ��

	HashINodeHead iNodeHashChain[32];

	// ����һ��HashINode
	HashINode* create_HashINode(INode& node);
	// ��INode�����ϣ����
	HashINode* insert_INode(INode& inode);
	// �ж��Ƿ���һ��HashINode�ڵ�
	HashINode* find_HashINode(inodeId_t id);
	// ����һ��HashINode�ڵ�
	bool get_HashINode(HashINode* hinode, inodeId_t id);
	// ����һ��INode�ڵ�
	bool get_INode(INode* inode, inodeId_t id);
	// ɾ��һ��HashINode�ڵ㣬ע���ɾ��Ӧ�ü���ɾ���ڴ�i�ڵ�ʹ���i�ڵ�
	bool delete_HashINode(inodeId_t id);


}HashINodeTable;