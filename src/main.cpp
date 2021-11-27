
/*

Description:  此文件包含 "main" 函数。程序执行将在此处开始并结束

*/

#include "FileSystem.h"
#include "ui.h"

using namespace std;

// 登录函数
void log(window& w, FileSystem* fs, FileDir* dir, userId_t uid);
// 运行的主函数，为交互函数
void running(window& w, FileSystem* fs, FileDir* dir, userId_t uid);
// 测试函数
void test_blocks160(FileSystem* fs, FileDir* dir);

int main()
{
    // 定义一个系统
    FileSystem* fs = new FileSystem("try");
    fs->init_system();

    // 界面
    window w;

    // 当前目录
    FileDir* dir = fs->get_rootDir();
    
    // 登录
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
    cout << "请选择操作：1.注册，2.登录，3.退出" << endl;
    int choice;
    cin >> choice;
    switch (choice)
    {
    case 1:
        while (1)
        {
            cout << "请输入用户名：" << endl;
            cin >> name;
            //检查该用户名是否已存在
            userId_t userid = fs->users.find(name);
            if (userid != -1)
            {
                cout << "用户名已存在，请重新输入" << endl;
            }
            else
                break;
        }
        cout << "请输入密码：" << endl;
        pwd1 = w.cin_password();
        cout << "请再次输入密码：" << endl;
        pwd2 = w.cin_password();
        if (pwd1 != pwd2)
        {
            cout << "两次输入密码不一致" << endl;

        }
        else
        {
            cout << "用户创建成功" << endl;
            fs->mkdir(dir, name, uid);
            fs->users.addUser(name, pwd1);
        }
        fs->users.save(&fs->disk);
        system("pause");
        log(w, fs, dir, uid);
        break;

    case 2:
        cout << "请输入用户名：" << endl;
        cin >> name;
        cout << "请输入密码：" << endl;
        pwd1 = w.cin_password();

        if (fs->users.login(name, pwd1))
        {
            cout << "登录成功" << endl;
            system("pause");
            //打开用户根目录，目录名为用户名
            string username = name;
            //改变当前目录
            dir = fs->cd(dir, username);
            running(w, fs, dir, uid);
        }
        else
        {
            cout << "登录失败" << endl;
            system("pause");
            log(w, fs, dir, uid);
        }

    }

}

void cut_string(string opt, string* opt_str)
{
    char c_temp[MAX];   // 解决 const char 和 char 的矛盾
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
        string opt_str[3] = {}; // 分割字符串
        cout << dir->get_fullDir() << ">";
        string dir_temp[17] = {};//分隔路径
        FileDir* d_temp;
        d_temp = dir;
        string opt = w.getopt();
        cut_string(opt, opt_str);       // 分割字符串
        cut_dir(opt_str[1], dir_temp);
        for (t = 0; t < 17; t++)
        {
            if (dir_temp[t] == "")
                break;
        }
        string opt_head = opt_str[0];   // 命令
        if (opt_head == "cd")
        {
            if (dir_temp[1] == "")
            {
                if (fs->cd(dir, opt_str[1]))
                    dir = fs->cd(dir, opt_str[1]);
                else
                    cout << "当前目录下没有名为“" << opt_str[1] << "”的目录，请检查您输入的名称是否正确" << endl;
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 0))
                    dir = fs->get_temp_dir(opt_str[1], 0);
                else
                    cout << "无法查找到名为“" << opt_str[1] << "”的目录，请检查您输入的名称是否正确" << endl;
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
            // 刷新函数
            system("cls");
            w.showwelcome();
            running(w, fs, dir, uid);
        }
        else if (opt_head == "touch") {
            if (dir_temp[1] == "")
            {
                // 当前目录下创建文件
                fs->touch(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //根据路径创建文件
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    fs->touch(d_temp, dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }

        }
        else if (opt_head == "write") {
            if (dir_temp[1] == "")
            {
                // 当前目录下写文件
                fs->write(dir, opt_str[2].c_str(), opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //根据路径写入文件
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    fs->write(d_temp, opt_str[2].c_str(), dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "echo") {
            if (dir_temp[1] == "")
            {
                // 当前目录下写文件
                fs->echo(dir, opt_str[2].c_str(), opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //根据路径写入文件
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    fs->echo(d_temp, opt_str[2].c_str(), dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
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
                    //根据路径写入文件
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
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "del") {
            if (dir_temp[1] == "")
            {
                // 删除文件
                fs->del(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // 删除文件
                    fs->del(d_temp, dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "ls") {
            if (dir_temp[0] == "")
            {
                // 列出当前路径下的文件或目录
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
                    cout << "目录不存在，请确认您输入的目录名称是否正确" << endl;
                }

            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    d_temp = fs->get_temp_dir(opt_str[1], 0);
                    // 列出当前路径下的文件或目录
                    fs->ls(d_temp, uid);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "exit") {
            // 退出文件系统
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
                    //根据路径写入文件
                    d_temp = fs->get_temp_dir(opt_str[1], 0);
                    fs->find_Dir(d_temp, opt_str[2].c_str());
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "help") {
            // 帮助函数
            w.help();
        }
        else if (opt_head == "logout") {
            // 退出
            dir = fs->get_rootDir();
            log(w, fs, dir, USER_ROOT_UID);
        }
        else if (opt_head == "mkdir") {
            if (dir_temp[1] == "")
            {
                // 创建目录
                fs->mkdir(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //根据路径写入文件
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // 创建目录
                    fs->mkdir(d_temp, dir_temp[1], uid);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "open") {
            if (dir_temp[1] == "")
            {
                // 打开文件
                HashINode* hinode = NULL;
                fs->open(dir, opt_str[1], uid, hinode);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //根据路径写入文件
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // 打开文件
                    HashINode* hinode = NULL;
                    fs->open(d_temp, dir_temp[t - 1], uid, hinode);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "rmdir") {
            if (dir_temp[1] == "")
            {
                // 删除目录
                fs->rmdir(dir, opt_str[1], uid);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //根据路径写入文件
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // 删除目录
                    fs->rmdir(d_temp, dir_temp[t - 1], uid);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
                }
            }
        }
        else if (opt_head == "rename") {
            if (dir_temp[1] == "")
            {
                // 重命名
                fs->rename(dir, opt_str[1], opt_str[2]);
            }
            else
            {
                if (fs->get_temp_dir(opt_str[1], 1))
                {
                    //根据路径写入文件
                    d_temp = fs->get_temp_dir(opt_str[1], 1);
                    // 重命名
                    fs->rename(d_temp, dir_temp[t - 1], opt_str[2]);
                }
                else
                {
                    cout << "路径不存在，请确认您的路径输入是否正确" << endl;
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
    char buff[1025] = {};    // 申请的时候要流出'\0'

    memset(str, 'a', 1024);
    //write;
    fs->write(dir, str, "a.txt", 0);

    for (int i = 1; i < 10; ++i) {      // 测试写20个块
        memset(str, '0' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }


    for (int i = 0; i < 10; ++i) {      // 测试写40个块
        memset(str, 'a' + i, 1024);
        //write;

        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // 测试写60个块
        memset(str, '0' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // 测试写80个块
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // 测试写100个块
        memset(str, '0' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // 测试写120个块
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // 测试写140个块
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    for (int i = 0; i < 10; ++i) {      // 测试写160个块
        memset(str, 'a' + i, 1024);
        //write;
        fs->echo(dir, str, "a.txt", 0);
    }

    // 多写一个头, 测试rem_bytes
    char strhello[13] = "hello world!";
    fs->echo(dir, strhello, "a.txt", 0);

    char buffLarge[81933] = {};    // 申请的时候要流出'\0'
    fs->cat(dir, "a.txt", buffLarge, 0);

    for (int i = 0; i < 81932; ++i) {
        if (i % 512 == 0)
            cout << endl;
        cout << buffLarge[i];
        if (i < 81920)
            cout << " ";
    }
    cout << endl;
    fs->del(dir, "a.txt", 0); // 删除文件

}