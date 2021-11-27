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
    void showwelcome();          //��ӭ����
    string cin_password();           //��������
    void show_login();       //��¼����
    void start();    //��ʾ��ӭ����͵�¼����
    void show(string name);       //��ʾ�����棨�����ɽ��еĲ������ƣ���ӭ��䣬������棩
    string getopt();                //��ȡ�������̨������
    int checkopt(string str,int len);   //�Ա�����Զ�����
    bool log();
    int show_choose();
    void help();
};

#endif // UI_H_INCLUDED
