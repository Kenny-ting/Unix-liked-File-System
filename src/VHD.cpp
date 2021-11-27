/*

Description: VHD.cpp (Virtual Hard Disk)
			 创建一个虚拟的磁盘，包括了对 SuperBlock 的操作

*/

#include "VHD.h"
#include "FSParams.h"
using namespace std;

VHD::VHD() : _file(), vhdName("") {};

VHD::VHD(string name) : _file(), vhdName(name)
{
	if (!this->load(name)) {
		create(name);
		load(name);
		formatVHD();
	}
}

VHD::~VHD() {
	if (_file.is_open())
		save();
	_file.close();
}

void VHD::setVhdName(string name)  // 设置VHD的名字
{
	this->vhdName = name;
}

bool VHD::exists()
{
	string disk_path = "./" + this->vhdName + ".disk";
	if (_access(disk_path.c_str(), 0)) {
		return false;
	}
	return true;
}

bool VHD::create(string name)   //Init a Virtual HD 
{
	this->setVhdName(name);
	diskSize_t mysz = (data_blockNum + index_blockNum + 2) * Block_Size;
	ofstream disk;
	string disk_path = "./" + name + ".disk";
	disk.open(disk_path, ios::out | ios::binary);
	disk.seekp(mysz, ios::beg);				//移动写指针到
	char eof = 0;
	disk << eof;
	disk.close();
	return true;
}

bool VHD::load(std::string name)	// Load VHD
{
	this->vhdName = name;
	string disk_path = "./" + this->vhdName + ".disk";
	if (_access(disk_path.c_str(), 0)) {
		return false;
	}
	this->_file.open(disk_path, ios::in | ios::out | ios::binary);
	return true;

}

bool VHD::save()					// 保存当前的VHD;
{
	this->_file.flush();
	return true;
}

bool VHD::formatVHD()				//格式化当前VHD 
{
	//成组链接

	//初始化一个开头
	blockId_t group[GROUP_SIZE + 1];
	group[0] = 1;
	for (blockId_t i = BlockNum - 1; i > 2 + index_blockNum; i--) {
		if (group[0] == GROUP_SIZE) {
			this->writeGroup(group, i);
			group[0] = 1;
			group[1] = i;
		}
		else {
			group[0]++;
			group[group[0]] = i;
		}
	}
	this->writeGroup(group, Super_BlcokId);
	bitset<160> inodes;
	inodes.reset();
	inodes.set(0);
	inodes.set(1);
	char stri[24];
	memcpy(stri, &inodes, sizeof(inodes));
	this->writeBlock(stri, Super_BlcokId, Bitmap_offset, sizeof(inodes));

	return true;
}

bool VHD::readBlock(char* buff, blockId_t blockID, blockSize_t begin, int len)
//ReadBlock函数一次只读一个Block，blockID是要读的块的id，begin相当于偏移量？？？
{
	long myseek = blockID * Block_Size + begin;
	this->_file.seekp(blockID * Block_Size + begin, ios::beg);
	this->_file.read(buff, len);
	return true;
}

bool VHD::writeBlock(const char* buff, blockId_t blockID, blockSize_t begin, int len)
//WriteBlock函数一次只写一个Block
{
	long myseek = blockID * Block_Size + begin;
	this->_file.seekp(myseek, ios::beg);
	this->_file.write(buff, len);
	return true;
}

bool VHD::writeGroup(blockId_t* leader, blockId_t bid)
{
	char block[Block_Size];
	char* p;
	p = block;
	for (int i = 0; i < GROUP_SIZE + 1; i++) {
		memcpy(p, leader + i, BID_LEN);
		p += BID_LEN;
	}
	writeBlock(block, bid);
	return true;
}
bool VHD::readGroup(blockId_t* leader, blockId_t bid)
{
	char block[Block_Size];
	readBlock(block, bid);
	char* p;
	p = block;
	for (int i = 0; i < GROUP_SIZE + 1; i++) {
		memcpy(leader + i, p, BID_LEN);
		p += BID_LEN;
	}
	return true;
}

bool VHD::readInode(char* buff, unsigned short iid, blockSize_t begin, int len)
{
	blockId_t blockID = 2 + iid / 4;
	begin += INODE_SIZE * (iid % 4);
	this->_file.seekp(blockID * Block_Size + begin, ios::beg);
	this->_file.read(buff, len);
	return true;
}

bool VHD::writeInode(const char* buff, unsigned short iid, blockSize_t begin, int len)
{
	blockId_t blockID = 2 + iid / 4;
	begin += INODE_SIZE * (iid % 4);
	this->_file.seekp(blockID * Block_Size + begin, ios::beg);
	this->_file.write(buff, len);
	return true;
}
