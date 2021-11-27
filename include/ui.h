#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

using namespace std;

#include <conio.h>
#include "Users.h"
#include <iostream>
#include <windows.h>
#include <string.h>
#define OPT_NUM 17
#define MAX 50




class window
{
private:
    string opt[OPT_NUM] = {"mkdir","create","cd","open","close","delete","dir","exit","logout","rmdir","format","rename","find","link","Tab","help","cls"};
    Users u;
    VHD v;
public:
    window() {}
    ~window() {}
    void showwelcome();          //欢迎界面
    string cin_password();           //密码隐藏
    void show_login();       //登录界面
    void start();    //显示欢迎界面和登录界面
    void show(string name);       //显示主界面（包括可进行的操作名称，欢迎语句，输入界面）
    string getopt();                //获取输入控制台的命令
    int checkopt(string str,int len);   //对比命令，自动补齐
    bool log();
    int show_choose();
    void help();
};

#endif // UI_H_INCLUDED
