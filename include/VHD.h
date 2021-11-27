#pragma once
#ifndef VHD_H
#define VHD_H

/*

Description: VHD (Virtual Hard Disk)
			 ����һ������Ĵ��̣������˶� SuperBlock �Ĳ���

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
	fstream  _file;			// �ļ���		
	VHD();
	VHD(string name);
	~VHD();

	// diskSize_t getSize() { return info.size; }
	string getVHDname() { return vhdName; }
	bool formatVHD();				//��ʽ����ǰVHD 
	void setVhdName(string name);	// ����VHD������
	bool create(std::string name);   //Init a Virtual HD 
	bool exists();					// Check the exitence of VHD
	bool load(std::string name);	// Load VHD  
	bool save();					// ���浱ǰ��VHD;
	bool readBlock(char* buff, blockId_t blockID, blockSize_t begin = 0, int len = Block_Size);
	//ReadBlock����һ��ֻ��һ��Block��blockID��Ҫ���Ŀ��id��begin�൱��ƫ����������
	bool writeBlock(const char* buff, blockId_t blockID, blockSize_t begin = 0, int len = Block_Size);
	//WriteBlock����һ��ֻдһ��Block
	bool writeGroup(blockId_t* leader, blockId_t bid);	//�����ڸ�ʽ������
	bool readGroup(blockId_t* leader, blockId_t bid);	//�����ڲ���
	bool readInode(char* buff, unsigned short iid, blockSize_t begin = 0, int len = INODE_SIZE);
	bool writeInode(const char* buff, unsigned short iid, blockSize_t begin = 0, int len = INODE_SIZE);
};

#endif // !VHD_H