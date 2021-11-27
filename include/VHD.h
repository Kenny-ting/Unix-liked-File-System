#pragma once
#ifndef VHD_H
#define VHD_H

/*

Description: VHD (Virtual Hard Disk)
			 创建一个虚拟的磁盘，包括了对 SuperBlock 的操作

*/

#include "FSParams.h"
#include <string>
#include <fstream>
#include <io.h>
#include <iostream>
#include <bitset>
#include "INode.h"

using namespace std;

class VHD
{
public:
	string vhdName;			// VHD name 
	fstream  _file;			// 文件流		
	VHD();
	VHD(string name);
	~VHD();

	// diskSize_t getSize() { return info.size; }
	string getVHDname() { return vhdName; }
	bool formatVHD();				//格式化当前VHD 
	void setVhdName(string name);	// 设置VHD的名字
	bool create(std::string name);   //Init a Virtual HD 
	bool exists();					// Check the exitence of VHD
	bool load(std::string name);	// Load VHD  
	bool save();					// 保存当前的VHD;
	bool readBlock(char* buff, blockId_t blockID, blockSize_t begin = 0, int len = Block_Size);
	//ReadBlock函数一次只读一个Block，blockID是要读的块的id，begin相当于偏移量？？？
	bool writeBlock(const char* buff, blockId_t blockID, blockSize_t begin = 0, int len = Block_Size);
	//WriteBlock函数一次只写一个Block
	bool writeGroup(blockId_t* leader, blockId_t bid);	//仅用于格式化磁盘
	bool readGroup(blockId_t* leader, blockId_t bid);	//仅用于测试
	bool readInode(char* buff, unsigned short iid, blockSize_t begin = 0, int len = INODE_SIZE);
	bool writeInode(const char* buff, unsigned short iid, blockSize_t begin = 0, int len = INODE_SIZE);
};

#endif // !VHD_H