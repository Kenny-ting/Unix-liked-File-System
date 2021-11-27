#include "ui.h"

string cur_dir = "home/user/file";


void window::showwelcome()          //��ӭ����
{
    system("cls");
    cout << "\n--------------------------------------------------------------" << endl;
    cout << "***                                                        ***" << endl;
    cout << "***                  ��ӭ�����ļ�����ϵͳ                  ***" << endl;
    cout << "***                                                        ***" << endl;
    cout << "--------------------------------------------------------------" << endl;
}

bool window::log()
{
    string name;
    string pwd1,pwd2;
    int flag = 0;
    cout << "�������û�����"<<endl;
    cin >> name;
    cout << "���������룺"<<endl;
    pwd1 = cin_password();
    cout << "���ٴ��������룺"<<endl;
    pwd2 = cin_password();
    if(pwd1 != pwd2)
    {
        cout << "�����������벻һ��"<<endl;
        system("pause");
        start();
    }
   // flag = u.addUser(name,pwd1);
    if(!flag)
    {
        cout <<"�����������벻һ�£����������룡"<<endl;
        system("pause");
        return false;
    }
    else
    {
        return true;
        //�������ݵ����̿��С�
    }
}

int window::show_choose()
{
    cout << "��ѡ�������1.ע�ᣬ2.��¼��3.�˳�" << endl;
    int a;
    cin >> a;
    return a;
}

void window::show_login()       //��¼����
{
    int a;
    string name;
    char username[20];
    string password;
    string pwd1;
    string pwd2;
    int len=0;
    int flag = 1;                      //��¼��־λ
    char psw[8];
    cin >> a;
    switch(a)
    {
    case 1:
        if(log())
        {
            cout << "yes"<<endl;

            //�û��������뱣�浽���̿�
        }

        else
            cout << "no"<<endl;
        system("pause");
        start();
        break;
    case 2:
        cout << "�������û�����"<<endl;
        cin >> username;
        cout << "���������룺"<<endl;
        password = cin_password();
        strcpy_s(psw,password.c_str());
        cout <<password<<endl;
        //flag = check(username,password) //��������Ƿ���ȷ����ȷ���1���������0
     /*   if(flag = u.login(username,psw))
        {
            //open(home,username);
            //change_cur_dir(username);     //��user�ļ������ĵ�ǰĿ¼
            //�ĵ�ǰĿ¼����ǰ�û���
            cout<<"yes"<<endl;
        }
        else
        {
            cout << "���������˶��û��������룡"<<endl;
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

void window::start()    //��ʾ��ӭ����͵�¼����
{
    showwelcome();
}

void window::show(string name)       //��ʾ�����棨�����ɽ��еĲ������ƣ���ӭ��䣩���м����Ϊ��ǰĿ¼��
{
    showwelcome();
    cout <<"--------------------------------------------------------------------------------"<<endl;
    cout <<"***   ������ʾ��//�ö�������������ʽ��Ϣ       ***"<<endl;
    cout <<"--------------------------------------------------------------------------------"<<endl;
    string dir;
    dir = name;
    //showdir(dir)
    cout << dir << ">";
}

string window::cin_password()           //��������
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
        if(ch == 9)     //����tab������������Զ����룬������û������������·���
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
    cout << "�����\t����\t\t\t\t\t\t\t��������" << endl;
    cout << "cd\t[Ŀ¼��|·��]\t\t\t\t\t��Ŀ¼���ָ��·��" << endl;
    cout << "cd..\t[Ŀ¼��|·��]\t\t\t\t\t���ϼ�Ŀ¼" << endl;
    cout << "cd/\t[Ŀ¼��|·��]\t\t\t\t\t�򿪸�Ŀ¼" << endl;
    cout << "close\t[�ļ���|·��]\t\t\t\t\t�رյ�ǰĿ¼�е��ļ���ָ��·���µ��ļ�" << endl;
    cout << "cls\t\t\t\t\t\t\t��ս���" << endl;
    cout << "creat\t[�ļ���|·��]\t\t\t\t\t�ڵ�ǰĿ¼�л�ָ��·���´����ļ�" << endl;
    cout << "delete\t[�ļ���|·��]\t\t\t\t\tɾ����ǰĿ¼�е��ļ���ָ��·���µ��ļ�" << endl;
    cout << "dir\t['��'|Ŀ¼��|·��]\t\t\t\t\t��ʾ��ǰĿ¼�����ʾָ��·���е�����Ŀ¼��" << endl;
    cout << "exit\t\t\t\t\t\t\t�˳��ļ�ϵͳ" << endl;
    cout << "find\t[�ļ���|Ŀ¼��]\t\t\t\t\t���Ҳ������ļ���Ŀ¼����·��" << endl;
    cout << "format\t\t\t\t\t\t\tɾ�������ļ�" << endl;
    cout << "help\t\t\t\t\t\t\t��ʾ��������˵��" << endl;
    cout << "logout\t\t\t\t\t\t\t�˳���¼״̬" << endl;
    cout << "mkdir\t[Ŀ¼��|·��]\t\t\t\t\t�ڵ�ǰĿ¼�л�ָ��·���´���Ŀ¼" << endl;
    cout << "open\t[�ļ���|·��]\t\t\t\t\t�򿪵�ǰĿ¼�е��ļ���ָ��·���µ��ļ�" << endl;
    cout << "rmdir\t[Ŀ¼��|·��]\t\t\t\t\tɾ��Ŀ¼��ָ��·��" << endl;
    cout << "rname\t[ԭ�ļ��� ���ļ���|ԭĿ¼�� ��Ŀ¼��]\t\t���ļ���Ŀ¼����������" << endl;
    cout << "TAB\t\t\t\t\t\t\t��ȫ�����" << endl;
}
