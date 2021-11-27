#pragma once

/*

Description: 文件系统的相关操作

*/

#include "VHD.h"
#include "SuperBlock.h"
#include "INode.h"
#include "FSParams.h"
#include "FileDir.h"
#include <vector>
#include <set>
#include <map>
#include "Users.h"

using namespace std;


class FileSystem
{
	
	
	FileDir* rootDir;				// 指向根目录的FileDir
	FileDir* curDir;				// 当前目录
	HashINodeTable hashINodeTable;	// 哈希表组织放入内存的i节点

public:
	FileSystem(string name) : rootDir(NULL), disk(name), superblock(&disk){}
	~FileSystem();

	Users users;
	VHD disk;						// 模拟一块磁盘
	SuperBlock superblock;			// 超级块
	/* 
		VHD 的操作
	*/
	
	// 初始化文件系统
	void init_system();

	/*
		INode 磁盘i节点的所有操作
	*/

	// 得到第k块的块号
	blockId_t get_kBlock(INode& inode, blockId_t k);
	// 设置某一个块到索引数组中, k = 当前已有块数 + 1
	void set_kBlock(INode& inode, blockId_t k, blockId_t blockID);
	// 增加一块文件块
	blockId_t append_Block(INode& inode);
	// 回收所有的文件块
	bool recycle_allBlock(INode& inode);
	// 创建一个文件
	inodeId_t create_file(FileDir* curDir, string fname, userId_t uid, fMode_t type);
	// 给定size大小的字节，读入到缓冲区中，例：buff大小1025，size大小1024，主要是方便输出
	bool read_file(HashINode* hinode, char* buff, size_t size, userId_t uid);
	// 给定缓冲区域的内容，写入文件
	bool write_file(HashINode* hinode, const char* buff, size_t size, userId_t uid);
	// 得到文件大小
	diskSize_t get_fileSize(FileDir* curDir, string fname);

	/*
		HashINodeTable 哈希表
	*/

	// 如果当前不在内存，将磁盘i节点放入内存
	HashINode* insert_HashINode(inodeId_t id);
	// 保存一个HashINode节点中的INode内容到磁盘中
	bool save_HashINode(HashINode* hinode);
	// 将内存组织的Hash队列中的一个HashINode根据changed更新外存
	void save_allINode();


	/*
		Dir: 文件目录
	*/

	// 返回根目录
	FileDir* get_rootDir() { return rootDir; };
	// 创建根目录
	bool create_rootDir();
	// 从磁盘中读出所有目录
	FileDir* read_dirFromDisk(char name[], inodeId_t id, FileDir* parent = NULL);
	// 向磁盘中保存所有目录
	bool write_dirToDisk(FileDir* curDir);
	// 将文件插入到当前目录下的部分
	bool insert_InodeId_toCurDir(FileDir* curDir, string fname, inodeId_t id);
	// 根据当前目录找到文件i节点的部分
	bool find_InodeId_inCurDir(FileDir* curDir, string fname, inodeId_t* id);
	// 根据当前目录名找到位置
	FileDir* find_srcDir_inCurDir(FileDir* curDir, const char* srcDir);
	// 删除当前目录中指向文件i节点的部分
	bool del_InodeId_inCurDir(FileDir* curDir, inodeId_t id);
	// 根据目录名找到位置
	FileDir* find_srcDir(FileDir* curDir, const char* srcDir);


	/* 
		所有的CMD命令操作
	*/

	// 登录
	bool login();
	// 退出
	bool logout();

	// 创建文件（当文件不存在时）create fild {fname} if not exist
	bool touch(FileDir* curDir, string fname, userId_t uid);
	// 删除文件（包括i节点和数据），需要判断是否有链接
	bool del(FileDir* curDir, string fname, userId_t uid);
	// 打开文件
	bool open(FileDir* curDir, string fname, userId_t uid, HashINode* hinode);
	// 读文件
	bool cat(FileDir* curDir, string fname, char* buff, userId_t uid);
	// 写文件，默认覆盖
	bool write(FileDir* curDir, const char* buff, string fname, userId_t uid);
	// 追加文件
	bool echo(FileDir* curDir, const char* buff, string fname, userId_t uid);
	// 关闭文件
	bool close(FileDir* curDir, string fname, userId_t uid);

	// 创建目录
	bool mkdir(FileDir* curDir, string fname, userId_t uid);
	// 删除目录
	bool rmdir(FileDir* curDir, string fname, userId_t uid);
	// 列出当前目录中内容
	bool ls(FileDir* curDir, userId_t uid);
	FileDir* cd(FileDir* curDir, string src);
	// 进入当前目录的上级目录
	FileDir* cd_parent(FileDir* curDir);
	// 进入主目录
	FileDir* cd_home();

	//重命名
	void rename(FileDir* curDir, string oldname, string newname);

	void find_Dir(FileDir* curDir, const char srcDir[]);

	FileDir* get_temp_dir(string src, int m);

	// 转换函数
	FileDir* str2filedir(FileDir* curDir, string path);
	// 软链接
	bool ln_soft(string sourceFile, string targetFile, userId_t uid);
	// 硬链接
	bool ln_hard(string sourceFile, string targetFile);
	// 拷贝函数
	bool cp(string sourceFile, string targetFile, userId_t uid);
	//格式化
	void format(FileDir* curDir, userId_t uid);
};

