/*

Description: i�ڵ���йز���

*/

#include "INode.h"
#include <string.h>
#include <iostream>
#include "FSParams.h"

using namespace std;

/*
	����i�ڵ�Ĳ���
*/

INode::INode()  //���캯��
{
	hlinkNum = 1; // hard link count, default = 1
	iNodeId = 0;		// i�ڵ��
	//blockId = iNodeIdCurrent % 32;	// ���
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

INode::INode(inodeId_t id, fMode_t Type, userId_t uid) // ���캯��
{
	owner = uid;
	hlinkNum = 1;	// hard link count, default = 1
	iNodeId = id;		// i�ڵ��
	//blockId = iNodeId % 32;	// ���

	//blockLocation = 0;
	blocks = 0;
	rem_bytes = 0;

	atime = mtime = time(NULL);
	for (int i = 0; i < 10; ++i) {
		direct_data[i] = 0;
	}
	inDirect1 = inDirect2 = 0;

	if (Type == FILE_TYPE_DIR) {		// Ŀ¼�ļ�

		this->mode = Type | DIR_DEFAULT;
	}
	else if (Type == FILE_TYPE_FILE) {	// ��ͨ�ļ�

		this->mode = Type | FILE_DEFAULT;
	}
	else {

		this->mode = Type | OWNER_ALL | PUBLIC_R | PUBLIC_X;
	}

}

// ��������
INode::INode(INode& inode)
{
	// ������Ϣ
	blockId = inode.blockId;	// Block ID
	iNodeId = inode.iNodeId;	// ��ǰi�ڵ��id
	//blockLocation = inode.blockLocation;	// ���ڵ�ǰ���̿���ĸ�λ��
	mode = inode.mode;			// (file type) (owner_rwx_flags) (other_rwx_flags)
	hlinkNum = inode.hlinkNum;	// hard link count, default = 1
	owner = inode.owner;		// owner user id

	// �����ļ���С��
	blocks = inode.blocks;			// full block count (default=-1)
	rem_bytes = inode.rem_bytes;	// remain bytes (default = BLOCK_SIZE)

	// �ļ�����
	atime = inode.atime;			// last access time �ϴη��ʵ�ʱ��
	mtime = inode.mtime;			// last modify time �ϴε��޸�ʱ��
	// direct_data[INODE_DIRECT_CNT]  direct datablocks

	for (int i = 0; i < 10; ++i) {
		direct_data[i] = inode.direct_data[i];
	}

	inDirect1 = inode.inDirect1;	// inDirect datablocks
	inDirect2 = inode.inDirect2;	// 
}

// �ļ���С
diskSize_t INode::size()
{
	return (diskSize_t)blocks * Block_Size + rem_bytes;
}


bool INode::check_AuthorityRead(userId_t t) 	// ���Ȩ��
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

bool INode::isDir() //�Ƿ���Ŀ¼�ļ�
{
	return (mode & FILE_TYPE_ALL) == FILE_TYPE_DIR;
}

bool INode::isFile() //�Ƿ�����ͨ�ļ�
{
	return (mode & FILE_TYPE_ALL) == FILE_TYPE_FILE;
}

bool INode::isLink() //�Ƿ��������ļ�
{
	return (mode & FILE_TYPE_ALL) == FILE_TYPE_LINK;
}


/*
   HashINode ��ϣ�ڵ�Ĳ���
*/

// ����HashINode����INode
INode& HashINode::get_INodeFromHINode(HashINode& hashINode)
{
	return hashINode.inode;
}

// �ж��Ƿ��޸�
bool HashINode::file_Ischanged(HashINode& hashINode)
{
	return hashINode.changed;
}


/*
   HashINodeTable ��ϣ��Ĳ���
*/

// ����һ��HashINode
HashINode* HashINodeTable::create_HashINode(INode& node)
{
	HashINode* hinode = new HashINode(node);
	hinode->changed = false;
	hinode->next = NULL;

	return hinode;
}

// ��INode�����ϣ����
HashINode* HashINodeTable::insert_INode(INode& inode)
{
	HashINode* hinode = create_HashINode(inode);

	// ȡ�����ϣ���к�
	int raw = inode.iNodeId % 32;
	int temp = iNodeHashChain[raw].num;
	// ��λ��
	HashINode* p = iNodeHashChain[raw].start; // ѭ���ҵ�
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

// �ж��Ƿ���һ��HashINode�ڵ�
HashINode* HashINodeTable::find_HashINode(inodeId_t id)
{
	int raw = (int)id % 32;

	HashINode* p = iNodeHashChain[raw].start; // ѭ���ҵ�
	if (p == NULL) {
		return p;
	}


	int temp = iNodeHashChain[raw].num; // num=1 ʱ��ֱ�Ӿ���start
	do {
		if (p->inode.iNodeId == id) {
			return p;
		}
		p = p->next;
		--temp;
	} while (temp);

	return NULL; // û�ҵ�
}

// ����һ��HashINode�ڵ�
bool HashINodeTable::get_HashINode(HashINode* hinode, inodeId_t id)
{
	HashINode* p = find_HashINode(id);
	if (p != NULL) {
		hinode = p;
		return true;
	}

	return false;

}

// ����һ��INode�ڵ�
bool HashINodeTable::get_INode(INode* inode, inodeId_t id)
{
	HashINode* hinode = new HashINode();
	if (get_HashINode(hinode, id)) {
		*inode = hinode->inode;
		return true;
	}
	return false;
}

// ɾ��һ��HashINode�ڵ㣬ע���ɾ��Ӧ�ü���ɾ���ڴ�i�ڵ�ʹ���i�ڵ�
bool HashINodeTable::delete_HashINode(inodeId_t id)
{
	int raw = (int)id % 32;

	HashINode* p = find_HashINode(id);
	if (p != NULL) {
		free(p);	// �ͷ�
		p = NULL;
		--iNodeHashChain[raw].num;	//�޸�����

		return true;
	}

	return false;

}


