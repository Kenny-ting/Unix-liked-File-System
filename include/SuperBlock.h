#pragma once
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

/*

Description: the Parameters of File System
			 该文件包括我们建立的文件系统的必要参数

*/

#include "FSParams.h"
#include <cstring>
#include "VHD.h"
#include <bitset>

using namespace std;

class SuperBlock	// 超级块 sizeof(SuperBlock) = BLOCKSIZE
{
private:
	blockId_t group[GROUP_SIZE + 1];
	VHD* vhd;
public:
	bitset<160> inodes;
	// Free blocks in the group, also used for stack top pointer
	SuperBlock(VHD* vhd);		//the initialization of superblock 要不要包括从一号块读？
	~SuperBlock();
	bool load();		//从1号节点中读
	bool save();		//写回1号节点
	bool recycleBlock(blockId_t bid); //回收块
	bool AllocateBlock(blockId_t* bid);//分配块
	bool showSB();		//测试用
	bool AllocateInode(unsigned short* iid);	//分配i节点
	bool recycleInode(unsigned short iid);		//回收i节点

};

#endif // !SUPERBLOCK_H	