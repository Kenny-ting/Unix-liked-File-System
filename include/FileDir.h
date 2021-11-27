#pragma once

#include "FSParams.h"
#include "INode.h"
#include <string>
#include <vector>

using namespace std;

typedef struct FileDir
{
	int len;			// ��Ŀ¼����
	char dirName[32];	// Ŀ¼��
	inodeId_t id;		// i�ڵ��id
	vector<FileDir*> subDirs;	// ��Ŀ¼
	FileDir* parent;	// ָ�򸸽ڵ�

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
		if (parent == NULL) return "/home";		// ���ظ�Ŀ¼
		return parent->get_fullDir() + "/" + dirName;
	}
}FileDir;