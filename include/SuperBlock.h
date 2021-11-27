#pragma once
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

/*

Description: the Parameters of File System
			 ���ļ��������ǽ������ļ�ϵͳ�ı�Ҫ����

*/

#include "FSParams.h"
#include <cstring>
#include "VHD.h"
#include <bitset>

using namespace std;

class SuperBlock	// ������ sizeof(SuperBlock) = BLOCKSIZE
{
private:
	blockId_t group[GROUP_SIZE + 1];
	VHD* vhd;
public:
	bitset<160> inodes;
	// Free blocks in the group, also used for stack top pointer
	SuperBlock(VHD* vhd);		//the initialization of superblock Ҫ��Ҫ������һ�ſ����
	~SuperBlock();
	bool load();		//��1�Žڵ��ж�
	bool save();		//д��1�Žڵ�
	bool recycleBlock(blockId_t bid); //���տ�
	bool AllocateBlock(blockId_t* bid);//�����
	bool showSB();		//������
	bool AllocateInode(unsigned short* iid);	//����i�ڵ�
	bool recycleInode(unsigned short iid);		//����i�ڵ�

};

#endif // !SUPERBLOCK_H	