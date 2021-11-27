
/*

Description:  ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ������

*/

#include "FileSystem.h"
#include "ui.h"

using namespace std;

// ��¼����
void log(window& w, FileSystem* fs, FileDir* dir, userId_t uid);
// ���е���������Ϊ��������
void running(window& w, FileSystem* fs, FileDir* dir, userId_t uid);
// ���Ժ���
void test_blocks160(FileSystem* fs, FileDir* dir);

int main()
{
    // ����һ��ϵͳ
    FileSystem* fs = new FileSystem("try");
    fs->init_system();

    // ����
    window w;

    // ��ǰĿ¼
    FileDir* dir = fs->get_rootDir();
    
    // ��¼
    log(w, fs, dir, USER_ROOT_UID);
    
    //running(w, fs, dir, USER_ROOT_UID);

    delete fs;
    return 0;
}


void log(window& w, FileSystem* fs, FileDir* dir, userId_t uid)
{
    w.showwelcome();
    string name;
    string pwd1, pwd2;
    int flag = 0;
    cout << "��ѡ�������1.ע�ᣬ2.��¼��3.�˳�" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
        while (1)
        {
            cout << "�������û�����" << endl;
            cin >> name;
            //�����û����Ƿ��Ѵ���
            userId_t userid = fs->users.find(name);
            if (userid != -1)
            {
                cout << "�û����Ѵ��ڣ�����������" << endl;
            }
            else
                break;
        }
        cout << "���������룺" << endl;
        pwd1 = w.cin_password();
        cout << "���ٴ��������룺" << endl;
        pwd2 = w.cin_password();
        if (pwd1 != pwd2)
        {
            cout << "�����������벻һ��" << endl;

        }
        else
        {
            cout << "�û������ɹ�" << endl;
            fs->mkdir(dir, name, uid);
            fs->users.addUser(name, pwd1);
        }
        fs->users.save(&fs->disk);
        system("pause");
        log(w, fs, dir, uid);
        break;

    case 2:
        cout << "�������û�����" << endl;
        cin >> name;
        cout << "���������룺" << endl;
        pwd1 = w.cin_password();

        if (fs->users.login(name, pwd1))
        {
            cout << "��¼�ɹ�" << endl;
            system("pause");
            //���û���Ŀ¼��Ŀ¼��Ϊ�û���
            string username = name;
            //�ı䵱ǰĿ¼
            dir = fs->cd(dir, username);
            running(w, fs, dir, uid);
        }
        else
        {
            cout << "��¼ʧ��" << endl;
            system("pause");
            log(w, fs, dir, uid);
        }

    }

}

void cut_string(string opt, string* opt_str)
{
    char c_temp[MAX];   // ��� const char �� char ��ì��
    strcpy_s(c_temp, opt.c_str());
    char* tp = NULL;
    char* p = strtok_s(c_temp, " ", &tp);

    int j = 0;
    while (p != NULL)
    {
        opt_str[j] = p;
        j++;
        p = strtok_s(NULL, " ", &tp);
    }
}

void cut_dir(string str, string* name)
{
    char c_temp[MAX];
    strcpy_s(c_temp, str.c_str());
    char* tp = NULL;
    char* p = strtok_s(c_temp, "/", &tp);

    int j = 0;
    while (p != NULL)
    {
        name[j] = p;
        j++;
        p = strtok_s(NULL, "/", &tp);
    }
}

void running(window& w, FileSystem* fs, FileDir* dir, userId_t uid)
{
    int t;
    while (true) {
        string opt_str[3] = {}; // �ָ��ַ���
        cout << dir->get_fullDir() << ">";
        string dir_temp[17] = {};//�ָ�·��
        FileDir* d_temp;
        d_temp = dir;
        string opt = w.getopt();
        cut_string(opt, opt_str);       // �ָ��ַ���
        cut_dir(opt_str[1], dir_temp);
        for (t = 0; t < 17; t++)
        {
            if (dir_temp[t] == "")
                break;
        }
        string opt_head = opt_str[0];   // ����
        if (opt_head == "cd")
        {
            if (dir_temp[1] == "")
            {
                if (fs->cd(dir, opt_str[1]))
                    dir = fs->cd(dir, opt_str[1]);
                else
                    cout << "��ǰĿ¼��û����Ϊ��" << opt_str[1] << "����Ŀ¼������������������Ƿ���ȷ" << endl;
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 0))
                    dir = fs->get_temp_dir(opt_str[1], 0);
                else
                    cout << "�޷����ҵ���Ϊ��" << opt_str[1] << "����Ŀ¼������������������Ƿ���ȷ" << endl;
            }
        }
        else if (opt_head == "cd..") {
            dir = fs->cd_parent(dir);
            //cout << opt_head << endl;
        }
        else if (opt_head == "cd/") {
            dir = fs->cd_home();
            //cout << opt_head << endl;
        }
        else if (opt_head == "close") {

            cout << opt_head << endl;
        }
        else if (opt_head == "cls") {
            // ˢ�º���
            system("cls");
            w.showwelcome();
            running(w, fs, dir, uid);
        }
        else if (opt_head == "touch") {
            if (dir_temp[1] == "")
            {
                // ��ǰĿ¼�´����ļ�
                fs->touch(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·�������ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    fs->touch(d_temp, dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }

        }
        else if (opt_head == "write") {
            if (dir_temp[1] == "")
            {
                // ��ǰĿ¼��д�ļ�
                fs->write(dir, opt_str[2].c_str(), opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    fs->write(d_temp, opt_str[2].c_str(), dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "echo") {
            if (dir_temp[1] == "")
            {
                // ��ǰĿ¼��д�ļ�
                fs->echo(dir, opt_str[2].c_str(), opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    fs->echo(d_temp, opt_str[2].c_str(), dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "cat") {
            if (dir_temp[1] == "")
            {
                diskSize_t fileSize = fs->get_fileSize(dir, opt_str[1]);
                cout << fileSize << endl;
                char* buff = (char*)malloc(fileSize + 1);
                memset(buff, '\0', fileSize + 1);

                fs->cat(dir, opt_str[1], buff, uid);
                cout << buff << endl;
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);

                    diskSize_t fileSize = fs->get_fileSize(d_temp, opt_str[1]);
                    cout << fileSize << endl;
                    char* buff = (char*)malloc(fileSize + 1);
                    memset(buff, '\0', fileSize + 1);

                    fs->cat(d_temp, dir_temp[t - 1], buff, uid);
                    cout << buff << endl;
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "del") {
            if (dir_temp[1] == "")
            {
                // ɾ���ļ�
                fs->del(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // ɾ���ļ�
                    fs->del(d_temp, dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "ls") {
            if (dir_temp[0] == "")
            {
                // �г���ǰ·���µ��ļ���Ŀ¼
                fs->ls(dir, uid);
            }
            else if (dir_temp[1] == "")
            {
                if (fs->find_srcDir(dir, dir_temp[0].c_str()))
                {
                    d_temp = fs->find_srcDir(dir, dir_temp[0].c_str());
                    fs->ls(d_temp, uid);
                    dir_temp[0] = "";
                }
                else
                {
                    cout << "Ŀ¼�����ڣ���ȷ���������Ŀ¼�����Ƿ���ȷ" << endl;
                }

            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    d_temp = fs->get_temp_dir(opt_str[1], 0);
                    // �г���ǰ·���µ��ļ���Ŀ¼
                    fs->ls(d_temp, uid);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "exit") {
            // �˳��ļ�ϵͳ
            cout << "Bye bye~" << endl;
            break;
        }
        else if (opt_head == "find") {
            if (dir_temp[1] == "")
            {
                fs->find_Dir(dir, opt_str[1].c_str());
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 0))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 0);
                    fs->find_Dir(d_temp, opt_str[2].c_str());
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "help") {
            // ��������
            w.help();
        }
        else if (opt_head == "logout") {
            // �˳�
            dir = fs->get_rootDir();
            log(w, fs, dir, USER_ROOT_UID);
        }
        else if (opt_head == "mkdir") {
            if (dir_temp[1] == "")
            {
                // ����Ŀ¼
                fs->mkdir(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // ����Ŀ¼
                    fs->mkdir(d_temp, dir_temp[1], uid);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "open") {
            if (dir_temp[1] == "")
            {
                // ���ļ�
                HashINode* hinode = NULL;
                fs->open(dir, opt_str[1], uid, hinode);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // ���ļ�
                    HashINode* hinode = NULL;
                    fs->open(d_temp, dir_temp[t - 1], uid, hinode);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "rmdir") {
            if (dir_temp[1] == "")
            {
                // ɾ��Ŀ¼
                fs->rmdir(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // ɾ��Ŀ¼
                    fs->rmdir(d_temp, dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "rename") {
            if (dir_temp[1] == "")
            {
                // ������
                fs->rename(dir, opt_str[1], opt_str[2]);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //����·��д���ļ�
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // ������
                    fs->rename(d_temp, dir_temp[t - 1], opt_str[2]);
                }
                else
                {
                    cout << "·�������ڣ���ȷ������·�������Ƿ���ȷ" << endl;
                }
            }
        }
        else if (opt_head == "ln") {
            string sourceFile(opt_str[1]);
            string targetFile(opt_str[2]);
            if (sourceFile[0] != '/')
                sourceFile = dir->get_fullDir() + "/" + sourceFile.substr(sourceFile.find_last_of('/') + 1);
            if (targetFile[0] != '/')
                targetFile = dir->get_fullDir() + "/" + targetFile.substr(targetFile.find_last_of('/') + 1);
            //cout << targetFile << endl;
            fs->ln_hard(sourceFile, targetFile);

        }
        else if (opt_head == "lns") {
            string sourceFile(opt_str[1]);
            string targetFile(opt_str[2]);
            if (sourceFile[0] != '/')
                sourceFile = dir->get_fullDir() + "/" + sourceFile.substr(sourceFile.find_last_of('/') + 1);
            if (targetFile[0] != '/')
                targetFile = dir->get_fullDir() + "/" + targetFile.substr(targetFile.find_last_of('/') + 1);
            fs->ln_soft(sourceFile, targetFile, uid);

        }
        else if (opt_head == "cp") {
            string sourceFile(opt_str[1]);
            string targetFile(opt_str[2]);
            if (sourceFile[0] != '/')
                sourceFile = dir->get_fullDir() + "/" + sourceFile.substr(sourceFile.find_last_of('/') + 1);
            if (targetFile[0] != '/')
                targetFile = dir->get_fullDir() + "/" + targetFile.substr(targetFile.find_last_of('/') + 1);
            fs->cp(sourceFile, targetFile, uid);

        }
        else if (opt_head == "format") {
            fs->format(dir, uid);
        }
        else if (opt_head == "test") {
            test_blocks160(fs, dir);

        }
    }

}

void test_blocks160(FileSystem* fs, FileDir* dir)
{
    fs->touch(dir, "a.txt", 0);

    HashINode* hinode = NULL;
    fs->open(dir, "a.txt", 0, hinode);

    char str[1025] = {};
    char buff[1025] = {};    // �����ʱ��Ҫ����'\0'

    memset(str, 'a', 1024);
    //write;
    fs->write(dir, str, "a.txt", 0);

    for (int i = 1; i < 10; ++i) {      // ����д20����
        memset(str, '0' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }


    for (int i = 0; i < 10; ++i) {      // ����д40����
        memset(str, 'a' + i, 1024);
        //write;

        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // ����д60����
        memset(str, '0' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // ����д80����
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // ����д100����
        memset(str, '0' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // ����д120����
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // ����д140����
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // ����д160����
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    // ��дһ��ͷ, ����rem_bytes
    char strhello[13] = "hello world!";
    fs->echo(dir, strhello, "a.txt", 0);

    char buffLarge[81933] = {};    // �����ʱ��Ҫ����'\0'
    fs->cat(dir, "a.txt", buffLarge, 0);

    for (int i = 0; i < 81932; ++i) {
        if (i % 512 == 0)
            cout << endl;
        cout << buffLarge[i];
        if (i < 81920)
            cout << " ";
    }
    cout << endl;
    fs->del(dir, "a.txt", 0); // ɾ���ļ�

}