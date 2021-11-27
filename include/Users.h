#pragma once
#ifndef Users_H
#define Users_H

#include <string>
#include <string.h>
#include <cstring>
#include <iostream>
#include "VHD.h"




extern int pre_user;

using namespace std;

class Users
{
private:
	short id[MAX_USER_NUM];
	char name[MAX_USER_NUM][USER_NAME_SIZE];
	char pwd[MAX_USER_NUM][USER_PWD_SIZE];
	short ID;
public:
	Users();
	~Users();
	void addUser(string name,string pwd);
	bool delUser(string name = "");
	bool login(string name, string pwd);
	bool su(string name = "");
	bool load(VHD* vhd);   //这里就存在0号块中
	bool save(VHD* vhd);
	bool showUsers();		//测试用
	bool getUindex(int* index);//获取空的记录行数
	userId_t find(string name);
};

#endif // !Users_H
