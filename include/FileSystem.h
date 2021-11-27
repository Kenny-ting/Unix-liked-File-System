#pragma once

/*

Description: �ļ�ϵͳ����ز���

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
	
	
	FileDir* rootDir;				// ָ���Ŀ¼��FileDir
	FileDir* curDir;				// ��ǰĿ¼
	HashINodeTable hashINodeTable;	// ��ϣ����֯�����ڴ��i�ڵ�

public:
	FileSystem(string name) : rootDir(NULL), disk(name), superblock(&disk){}
	~FileSystem();

	Users users;
	VHD disk;						// ģ��һ�����
	SuperBlock superblock;			// ������
	/* 
		VHD �Ĳ���
	*/
	
	// ��ʼ���ļ�ϵͳ
	void init_system();

	/*
		INode ����i�ڵ�����в���
	*/

	// �õ���k��Ŀ��
	blockId_t get_kBlock(INode& inode, blockId_t k);
	// ����ĳһ���鵽����������, k = ��ǰ���п��� + 1
	void set_kBlock(INode& inode, blockId_t k, blockId_t blockID);
	// ����һ���ļ���
	blockId_t append_Block(INode& inode);
	// �������е��ļ���
	bool recycle_allBlock(INode& inode);
	// ����һ���ļ�
	inodeId_t create_file(FileDir* curDir, string fname, userId_t uid, fMode_t type);
	// ����size��С���ֽڣ����뵽�������У�����buff��С1025��size��С1024����Ҫ�Ƿ������
	bool read_file(HashINode* hinode, char* buff, size_t size, userId_t uid);
	// ����������������ݣ�д���ļ�
	bool write_file(HashINode* hinode, const char* buff, size_t size, userId_t uid);
	// �õ��ļ���С
	diskSize_t get_fileSize(FileDir* curDir, string fname);

	/*
		HashINodeTable ��ϣ��
	*/

	// �����ǰ�����ڴ棬������i�ڵ�����ڴ�
	HashINode* insert_HashINode(inodeId_t id);
	// ����һ��HashINode�ڵ��е�INode���ݵ�������
	bool save_HashINode(HashINode* hinode);
	// ���ڴ���֯��Hash�����е�һ��HashINode����changed�������
	void save_allINode();


	/*
		Dir: �ļ�Ŀ¼
	*/

	// ���ظ�Ŀ¼
	FileDir* get_rootDir() { return rootDir; };
	// ������Ŀ¼
	bool create_rootDir();
	// �Ӵ����ж�������Ŀ¼
	FileDir* read_dirFromDisk(char name[], inodeId_t id, FileDir* parent = NULL);
	// ������б�������Ŀ¼
	bool write_dirToDisk(FileDir* curDir);
	// ���ļ����뵽��ǰĿ¼�µĲ���
	bool insert_InodeId_toCurDir(FileDir* curDir, string fname, inodeId_t id);
	// ���ݵ�ǰĿ¼�ҵ��ļ�i�ڵ�Ĳ���
	bool find_InodeId_inCurDir(FileDir* curDir, string fname, inodeId_t* id);
	// ���ݵ�ǰĿ¼���ҵ�λ��
	FileDir* find_srcDir_inCurDir(FileDir* curDir, const char* srcDir);
	// ɾ����ǰĿ¼��ָ���ļ�i�ڵ�Ĳ���
	bool del_InodeId_inCurDir(FileDir* curDir, inodeId_t id);
	// ����Ŀ¼���ҵ�λ��
	FileDir* find_srcDir(FileDir* curDir, const char* srcDir);


	/* 
		���е�CMD�������
	*/

	// ��¼
	bool login();
	// �˳�
	bool logout();

	// �����ļ������ļ�������ʱ��create fild {fname} if not exist
	bool touch(FileDir* curDir, string fname, userId_t uid);
	// ɾ���ļ�������i�ڵ�����ݣ�����Ҫ�ж��Ƿ�������
	bool del(FileDir* curDir, string fname, userId_t uid);
	// ���ļ�
	bool open(FileDir* curDir, string fname, userId_t uid, HashINode* hinode);
	// ���ļ�
	bool cat(FileDir* curDir, string fname, char* buff, userId_t uid);
	// д�ļ���Ĭ�ϸ���
	bool write(FileDir* curDir, const char* buff, string fname, userId_t uid);
	// ׷���ļ�
	bool echo(FileDir* curDir, const char* buff, string fname, userId_t uid);
	// �ر��ļ�
	bool close(FileDir* curDir, string fname, userId_t uid);

	// ����Ŀ¼
	bool mkdir(FileDir* curDir, string fname, userId_t uid);
	// ɾ��Ŀ¼
	bool rmdir(FileDir* curDir, string fname, userId_t uid);
	// �г���ǰĿ¼������
	bool ls(FileDir* curDir, userId_t uid);
	FileDir* cd(FileDir* curDir, string src);
	// ���뵱ǰĿ¼���ϼ�Ŀ¼
	FileDir* cd_parent(FileDir* curDir);
	// ������Ŀ¼
	FileDir* cd_home();

	//������
	void rename(FileDir* curDir, string oldname, string newname);

	void find_Dir(FileDir* curDir, const char srcDir[]);

	FileDir* get_temp_dir(string src, int m);

	// ת������
	FileDir* str2filedir(FileDir* curDir, string path);
	// ������
	bool ln_soft(string sourceFile, string targetFile, userId_t uid);
	// Ӳ����
	bool ln_hard(string sourceFile, string targetFile);
	// ��������
	bool cp(string sourceFile, string targetFile, userId_t uid);
	//��ʽ��
	void format(FileDir* curDir, userId_t uid);
};

