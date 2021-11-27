#pragma once

#include "FSParams.h"
#include "INode.h"
#include <string>
#include <vector>

using namespace std;

typedef struct FileDir
{
	int len;			// 子目录个数
	char dirName[32];	// 目录名
	inodeId_t id;		// i节点的id
	vector<FileDir*> subDirs;	// 子目录
	FileDir* parent;	// 指向父节点

	FileDir(inodeId_t inode, FileDir* p = NULL) : id(inode), len(0), parent(p) 
	{
		memset(dirName, '\0', 32);
	}

	FileDir(const char* name, inodeId_t inode, FileDir* p = NULL) : id(inode), len(0), parent(p)
	{
		memcpy(dirName, name, 32);
	}

	~FileDir() {}

	FileDir* get_parent() {
		return parent;
	}

	string get_fullDir() {
		if (parent == NULL) return "/home";		// 返回根目录
		return parent->get_fullDir() + "/" + dirName;
	}
}FileDir;