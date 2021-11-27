#include "ui.h"

string cur_dir = "home/user/file";


void window::showwelcome()          //欢迎界面
{
    system("cls");
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "***                                                        ***" << endl;
    cout << "***                  欢迎来到文件管理系统                  ***" << endl;
    cout << "***                                                        ***" << endl;
    cout << "--------------------------------------------------------------" << endl;
}

bool window::log()
{
    string name;
    string pwd1,pwd2;
    int flag = 0;
    cout << "请输入用户名："<<endl;
    cin >> name;
    cout << "请输入密码："<<endl;
    pwd1 = cin_password();
    cout << "请再次输入密码："<<endl;
    pwd2 = cin_password();
    if(pwd1 != pwd2)
    {
        cout << "两次输入密码不一致"<<endl;
        system("pause");
        start();
    }
   // flag = u.addUser(name,pwd1);
    if(!flag)
    {
        cout <<"两次输入密码不一致，请重新输入！"<<endl;
        system("pause");
        return false;
    }
    else
    {
        return true;
        //保存数据到磁盘块中。
    }
}

int window::show_choose()
{
    cout << "请选择操作：1.注册，2.登录，3.退出" << endl;
    int a;
    cin >> a;
    return a;
}

void window::show_login()       //登录界面
{
    int a;
    string name;
    char username[20];
    string password;
    string pwd1;
    string pwd2;
    int len=0;
    int flag = 1;                      //登录标志位
    char psw[8];
    cin >> a;
    switch(a)
    {
    case 1:
        if(log())
        {
            cout << "yes"<<endl;

            //用户名和密码保存到磁盘块
        }

        else
            cout << "no"<<endl;
        system("pause");
        start();
        break;
    case 2:
        cout << "请输入用户名："<<endl;
        cin >> username;
        cout << "请输入密码："<<endl;
        password = cin_password();
        strcpy_s(psw,password.c_str());
        cout <<password<<endl;
        //flag = check(username,password) //检查密码是否正确，正确输出1，错误输出0
     /*   if(flag = u.login(username,psw))
        {
            //open(home,username);
            //change_cur_dir(username);     //打开user文件，更改当前目录
            //改当前目录，当前用户。
            cout<<"yes"<<endl;
        }
        else
        {
            cout << "密码错误，请核对用户名和密码！"<<endl;
            system("pause");
            start();
        }
        */
        break;
    case 3:
        exit(0);
        break;
    }

}

void window::start()    //显示欢迎界面和登录界面
{
    showwelcome();
}

void window::show(string name)       //显示主界面（包括可进行的操作名称，欢迎语句）（中间参数为当前目录）
{
    showwelcome();
    cout <<"--------------------------------------------------------------------------------"<<endl;
    cout <<"***   操作提示：//好多操作名和输入格式信息       ***"<<endl;
    cout <<"--------------------------------------------------------------------------------"<<endl;
    string dir;
    dir = name;
    //showdir(dir)
    cout << dir << ">";
}

string window::cin_password()           //密码隐藏
{
    char a[8];
    int len = 0;
    char ch;
    while(1)
    {
        ch = _getch();
        if(ch == 8)
        {
            if(len!=0)
            {
                cout <<char(8)<<" "<<char(8);
                len--;
                continue;
            }
        }
        else if(ch == '\r')
        {
            cout <<endl;
            a[len]='\0';
            break;
        }
        else
        {
            cout << "*";
            a[len] = ch;
            len++;
        }
    }
    return a;
}

int window::checkopt(string str,int len)
{
    int i;
    string temp1,temp2;
    temp2 = str.substr(0,len);
    for(i=0;i<OPT_NUM;i++)
    {
        temp1 = opt[i].substr(0,len);
        //cout <<temp<<endl;
        if(temp2.compare(temp1)==0)
            return i+1;
    }
    return 0;
}

string window::getopt()
{
    char a[MAX];
    int i;
    int len=0;
    char ch;
    int result;
    while(1)
    {
        ch = _getch();
        if(ch == 9)     //输入tab，对命令进行自动补齐，若发现没有相似命令，无事发生
        {
            result = checkopt(a,len);
            if(result)
            {
                string temp = opt[result-1].substr(len,opt[result-1].length()-len);
                cout << temp;
                int t = 0;
                for(;t<temp.length();len++,t++)
                {
                    a[len] = temp[t];
                }
                a[len] = ' ';
                continue;
            }
        }
        else if(ch == '\r')
        {
            a[len] = '\0';
            cout <<endl;
            break;
        }
        else if(ch == 8)
        {
            len--;
            cout << char(8)<<' '<<char(8);
            continue;
        }
        else
        {
            a[len] = ch;
            cout<< ch;
        }
        len++;
    }
    return a;
}

void window::help()
{
    cout << "命令符\t参数\t\t\t\t\t\t\t命令作用" << endl;
    cout << "cd\t[目录名|路径]\t\t\t\t\t打开目录或打开指定路径" << endl;
    cout << "cd..\t[目录名|路径]\t\t\t\t\t打开上级目录" << endl;
    cout << "cd/\t[目录名|路径]\t\t\t\t\t打开根目录" << endl;
    cout << "close\t[文件名|路径]\t\t\t\t\t关闭当前目录中的文件或指定路径下的文件" << endl;
    cout << "cls\t\t\t\t\t\t\t清空界面" << endl;
    cout << "creat\t[文件名|路径]\t\t\t\t\t在当前目录中或指定路径下创建文件" << endl;
    cout << "delete\t[文件名|路径]\t\t\t\t\t删除当前目录中的文件或指定路径下的文件" << endl;
    cout << "dir\t['空'|目录名|路径]\t\t\t\t\t显示当前目录项或显示指定路径中的所有目录项" << endl;
    cout << "exit\t\t\t\t\t\t\t退出文件系统" << endl;
    cout << "find\t[文件名|目录名]\t\t\t\t\t查找并返回文件或目录所在路径" << endl;
    cout << "format\t\t\t\t\t\t\t删除所有文件" << endl;
    cout << "help\t\t\t\t\t\t\t显示所有命令说明" << endl;
    cout << "logout\t\t\t\t\t\t\t退出登录状态" << endl;
    cout << "mkdir\t[目录名|路径]\t\t\t\t\t在当前目录中或指定路径下创建目录" << endl;
    cout << "open\t[文件名|路径]\t\t\t\t\t打开当前目录中的文件或指定路径下的文件" << endl;
    cout << "rmdir\t[目录名|路径]\t\t\t\t\t删除目录或指定路径" << endl;
    cout << "rname\t[原文件名 新文件名|原目录名 新目录名]\t\t对文件或目录进行重命名" << endl;
    cout << "TAB\t\t\t\t\t\t\t补全命令符" << endl;
}
