#include "Users.h"

int pre_user;

Users::Users() {
	for (int i = 0; i < MAX_USER_NUM; i++)
		this->id[i] = -1;
	this->ID = 1;
};

Users::~Users() {};


bool Users::getUindex(int* index)
{
	for (int i = 0; i < MAX_USER_NUM; i++)
	{
		if (this->id[i] == -1)
		{
			*index = i;
			return true;
		}
	}
	return false;
}

void Users::addUser(string name,string pwd) {

		int index;
		this->getUindex(&index);
		strcpy_s(this->name[index], name.c_str());
		strcpy_s(this->pwd[index], pwd.c_str());
		this->id[index] = this->ID;
		this->ID++;
		//adduser success！
}

bool Users::delUser(string name)
{
	if (name == "") {
		throw "this command format is 'delUser name'";
	}
	for (int i = 0; i < MAX_USER_NUM; i++) {
		if (this->id[i] != -1 && !strcmp(name.c_str(), this->name[i])) {
			this->id[i] = -1;
			return true;
		}
	}
	return false;
}

bool Users::su(string name) {
	if (name == "") {
		throw "this command format is 'addUser name'";
	}
	cout << "input the password:";
	char pwd[USER_PWD_SIZE];
	cin >> pwd;
	for (int i = 0; i < MAX_USER_NUM; i++) {
		if (this->id[i] != -1 && !strcmp(name.c_str(), this->name[i])) {
			if (!strcmp(pwd, this->pwd[i])) {
				pre_user = this->id[i];
				//"Login success!"
				return true;
			}
			else {
				//"Login failed! Password wrong!"
				return false;
			}
		}
	}
	//Login failed! User does not exist
	return false;
}

bool Users::login(string name,string pwd) {
	for (int i = 0; i < MAX_USER_NUM; i++) {
		if (this->id[i] != -1 && !strcmp(name.c_str(), this->name[i])) {
			if (!strcmp(pwd.c_str(), this->pwd[i])) {
				pre_user = this->id[i];
				//Login success!
				return true;
			}
			else {
				//Login failed! Password wrong!
				return false;
			}
		}
	}
	//Login failed! User does not exist
	return false;
}

bool Users::save(VHD* vhd)
{
	//const int len = MAX_USER_NUM * (USER_NAME_SIZE + USER_PWD_SIZE + sizeof(this->id)) + sizeof(this->ID);
	const int len = sizeof(*this);
	char users[len];
	//char* p;
	//p = users;

	//强制类型转换 按序拷贝到users数组中 也可以申请一个长度为block的数组
	/*
	memcpy(p, &this->ID, sizeof(this->ID));
	p += sizeof(this->user_num);
	memcpy(p, this->name, sizeof(this->name));
	p += sizeof(this->name);
	memcpy(p, this->pwd, sizeof(this->pwd));
	vhd->writeBlock(users, Boot_BlockId, 0, len);*/
	vhd->writeBlock((char*)this, Boot_BlockId, 0, len);

	return true;
}

bool Users::load(VHD* vhd)
{
	//const int len = MAX_USER_NUM * (USER_NAME_SIZE + USER_PWD_SIZE + sizeof(this->id)) + sizeof(this->ID);
	const int len = sizeof(*this);
	char users[len];
	vhd->readBlock((char*)this, Boot_BlockId, 0, len);
	//char* p;
	//p = users;
	//强制类型转换 按序从users数组中拷贝到对应的类型中 也可以申请一个长度为block的数组


	//memcpy(&this->user_num, p, sizeof(this->user_num));
	//p += sizeof(this->user_num);
	//memcpy(this->name, p, sizeof(this->name));
	//p += sizeof(this->name);
	//memcpy(this->pwd, p, sizeof(this->pwd));

	return true;
}

bool Users::showUsers()
{
	for (int i = 0; i < MAX_USER_NUM; i++)
	{
		if (this->id[i] != -1)
			cout << this->name[i] << " " << this->pwd[i] << endl;
	}
	return true;
}

userId_t Users::find(string name)
{
	for (int i = 0; i < MAX_USER_NUM; i++)
	{
		if (strcmp(name.c_str(), this->name[i])==0&& this->id[i]!=-1)
		{
			//Login success!
			return i;
		}
	}
	return -1;
}