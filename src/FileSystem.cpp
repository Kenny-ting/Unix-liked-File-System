/*

Description: 文件系统的相关操作

*/

#include "FileSystem.h"
#include <string.h>

/*
	FileSystem 的构造函数、析构函数
*/


FileSystem::~FileSystem()
{
	// 保存i节点的改变
	save_allINode();
	// 保存所有目录的改变
	write_dirToDisk(rootDir);
}

void my_strcat(char* dest, int size, char* src)
{
	int start = strlen(dest);
	for (int i = 0; i < size; ++i) {
		dest[start + i] = src[i];
	}
}

/*
	VHD 的操作
*/

void FileSystem::init_system()
{
	INode* inodeRoot = new INode();
	disk.readInode((char*)inodeRoot, 1);    // 读入根目录的i节点

	if (inodeRoot->direct_data[0] == 34) {  // 如果已经初始化过目录
		// 默认根目录在1号i节点和34号磁盘块
		char name[32] = "/home";
		rootDir = read_dirFromDisk(name, 1, NULL);
	}
	else {
		create_rootDir();
		Users user;
		user.save(&this->disk);
	}
	users.load(&this->disk);
}



/*
	INode 磁盘i节点的所有操作
*/


// 找到第k块的内容
blockId_t FileSystem::get_kBlock(INode& inode, blockId_t k)
{
	if ((long long)k > inode.blocks + 1) cout << "k is too large" << endl;
	if (k < INODE_DIRECT_CNT) {
		return inode.direct_data[k];
	}	
	else {
		k = k - INODE_DIRECT_CNT; // 减去直接寻址的块数
		blockId_t ans;

		if (k < INODE_INDIRECT_CNT1)
		{
			disk.readBlock((char*)&ans, inode.inDirect1, k * BID_LEN, BID_LEN);
			return ans;
		}
		k -= INODE_INDIRECT_CNT1;
		if (k < INODE_INDIRECT_CNT2)
		{
			blockId_t temp, ind1;
			temp = k / INODE_INDIRECT_CNT1; // K所在的一阶间址表在二阶间址表中的序号
			disk.readBlock((char*)&ind1, inode.inDirect2, temp * BID_LEN, BID_LEN); // K所在的一阶间址表地址
			k -= temp * (Block_Size / BID_LEN);
			disk.readBlock((char*)&ans, ind1, k * BID_LEN, BID_LEN);
			return ans;
		}
		else cout << "k is out of range!" << endl;

	}
	
}

// 设置某一个块到索引数组中, k = 当前已有块数 + 1
void FileSystem::set_kBlock(INode& inode, blockId_t k, blockId_t blockId)
{
	if (k < INODE_DIRECT_CNT)
	{
		inode.direct_data[k] = blockId;
		return;
	}
	k = k - INODE_DIRECT_CNT; // 减去直接寻址的块数
	if (k < INODE_INDIRECT_CNT1)
	{
		if (k == 0) superblock.AllocateBlock(&inode.inDirect1); // 需要先分配一块来寻址
		if (!disk.writeBlock((char*)&blockId, inode.inDirect1, k * BID_LEN, BID_LEN))
		{
			cout << "Failed to write in inDirect1 : " << to_string(k * BID_LEN) << endl;
		}
		return;
	}
	k = k - INODE_INDIRECT_CNT1;
	if (k < INODE_INDIRECT_CNT2)
	{
		if (k == 0) superblock.AllocateBlock(&inode.inDirect2);
		blockId_t tmp, ind1;
		tmp = k / (Block_Size / BID_LEN); // K所在的一阶间址表在二阶间址表中的序号
		k -= tmp * (Block_Size / BID_LEN);
		if (k == 0) // 需要在indirect2块中新增一个一阶间址表的blockID
		{
			superblock.AllocateBlock(&ind1);
			if (!disk.writeBlock((char*)&ind1, inode.inDirect2, tmp * BID_LEN, BID_LEN))
			{
				cout << "Failed to write to inDirect2 : " + std::to_string(tmp * BID_LEN) << endl;
			}
		}
		else
		{
			if (!disk.readBlock((char*)&ind1, inode.inDirect2, tmp * BID_LEN, BID_LEN)) // K所在的一阶间址表地址
			{
				cout << "Failed to Read from indirect2 : " + std::to_string(tmp * BID_LEN) << endl;
			}
		}
		disk.writeBlock((char*)&blockId, ind1, k * BID_LEN, BID_LEN);
	}
	else cout << "K is too large" << endl;
}


// 增加一块文件块
blockId_t FileSystem::append_Block(INode& inode)
{

	inode.atime = inode.mtime = time(NULL);
	blockId_t newBlock;
	superblock.AllocateBlock(&newBlock);

	set_kBlock(inode, inode.blocks, newBlock);

	++inode.blocks;
	inode.rem_bytes = 0;
	return newBlock;

}

// 回收所有的文件块
bool FileSystem::recycle_allBlock(INode& inode)
{
	int blocks = inode.blocks;
	
	
	++blocks;	// 无论那个rem_bytes是否为0，都还有一块残余要处理
	

	// 先清除直接索引
	for (int i = 0; inode.direct_data[i] != 0 && i < 10; ++i) {
		superblock.recycleBlock(inode.direct_data[i]); // 回收该块
		inode.direct_data[i] = 0;	// 指向0
		--blocks;	// 减去直接寻址的块数
	}

	// 处理一级间址
	if (blocks !=0) {
		int range = blocks;
		for (int i = 0; i < range && i < INODE_INDIRECT_CNT1; ++i) {
			blockId_t* bid = new blockId_t();
			disk.readBlock((char*)bid, inode.inDirect1, i * BID_LEN, BID_LEN);
			superblock.recycleBlock(*bid);	// 回收块
			--blocks;	//减少一块
		}
		superblock.recycleBlock(inode.inDirect1);	// 回收块
		inode.inDirect1 = 0;
	}
	
	// 处理二级间址
	if (blocks != 0) {

		blockId_t tmp = blocks / INODE_INDIRECT_CNT1; // INODE_INDIRECT_CNT1是一个块能间接寻址的块数
		blocks -= tmp * (Block_Size / BID_LEN);

		for (int i = 0; i < tmp + 1; ++i)
		{
			blockId_t* bid1 = new blockId_t();
			disk.readBlock((char*)bid1, inode.inDirect2, i * BID_LEN, BID_LEN);	// 读入一级间址的块号
			
			int range = INODE_INDIRECT_CNT1;
			if (i == tmp)
				range = blocks;	// 如果是最后一块只清空到一部分
			
			for (int j = 0; j < range; ++j) {
				blockId_t* bid2 = new blockId_t();
				disk.readBlock((char*)bid2, *bid1, j * BID_LEN, BID_LEN); // 读入具体的数据块
				superblock.recycleBlock(*bid2);	// 回收块
				//--inode.blocks;	//减少一块
			}
			superblock.recycleBlock(*bid1);	// 回收块
		}
		inode.inDirect2 = 0;

	}
	inode.blocks = 0;
	inode.rem_bytes = 0;
	inode.atime = inode.mtime = time(NULL);
	return true;
}

// 根据类型创建文件
inodeId_t FileSystem::create_file(FileDir* curDir, string fname, userId_t uid, fMode_t type)
{
	inodeId_t* id = new inodeId_t();		//申请空闲i节点

	superblock.AllocateInode(id);			// 给出一个空闲的i节点编号

	// 如果在当前目录下没找到i节点
	if (find_InodeId_inCurDir(curDir, fname, id)) {
		cout << "当前目录下有同名文件！请改名！" << endl;
		return 0;
	}

	INode* inode = new INode(*id, type, uid);	// 建立一个i节点
	HashINode* hinode = hashINodeTable.insert_INode(*inode);		// 插入哈希表中
	
	blockId_t bid = append_Block(hinode->inode);// 添加一块空闲块
	--hinode->inode.blocks;	                    // append_Block函数会默认加1
	hinode->changed = true;

	disk.writeInode((char*)inode, *id);	// 写回磁盘i节点

	return *id;
}


// 给定size大小的字节，读入到缓冲区中，例：buff大小1025，size大小1024，主要是方便输出
bool FileSystem::read_file(HashINode* hinode, char* buff, size_t size, userId_t uid)
{
	diskSize_t fileSize = hinode->inode.size();	// 文件大小

	if (size > fileSize) {	// 比文件大小还大
		cout << "Size is larger than the file itself!" << endl;
		return false;
	}

	if (uid == hinode->inode.owner || uid == USER_ROOT_UID) {

		int blocks = size / Block_Size;
		blockId_t k_logic = 0;	// 从第0块开始读
		blockId_t k_true = 0;	// 物理块号
		char blockbuff[Block_Size] = {};	// 缓冲区，放读入的内容

		// 按块读文件
		while (blocks) {
			k_true = get_kBlock(hinode->inode, k_logic);
			disk.readBlock(blockbuff, k_true);	// 读一块出来
			my_strcat(buff, Block_Size, blockbuff);
			size = size - Block_Size;		// size减去一块的长度
			++k_logic;
			--blocks;
		}
		if (size != 0) {
			k_true = get_kBlock(hinode->inode, k_logic);	//
			disk.readBlock(blockbuff, k_true, 0, size);		// 读size长度的字节出来
			my_strcat(buff, size, blockbuff);
		}

		// 修改 changed, 最后访问时间
		hinode->changed = 1;
		hinode->inode.atime = time(NULL);

	}
	else {
		cout << "Access denied!" << endl;
		return false;
	}

}


// 给定缓冲区域的内容，写入文件
bool FileSystem::write_file(HashINode* hinode, const char* buff, size_t buffSize, userId_t uid)
{

	if (uid != hinode->inode.owner && uid != USER_ROOT_UID) {				// 判断是否是当前用户
		cout << "Access denied!" << endl;
		return false;
	}

	hinode->changed = true;

	diskSize_t fileSize = hinode->inode.size();	// 文件大小

	if (recycle_allBlock(hinode->inode)) {			// 先将i节点索引数组中的所有块回收

		// 需要写多个块
		int blocks = buffSize / Block_Size;
		char buff_temp[Block_Size] = {};
		int i = 0;
		for (i = 0; i < blocks; ++i) {
			// 拷贝一块大小的内容
			memcpy(buff_temp, buff + i * Block_Size, Block_Size);
			// 分配块并写入
			blockId_t bid = append_Block(hinode->inode);
			disk.writeBlock(buff_temp, bid);
			// 减去已经写入的
			buffSize -= Block_Size;		// 减去已经写入的
		}

		// 拷贝余下大小的内容
		memcpy(buff_temp, buff + i * Block_Size, buffSize);
		// 分配块并写入
		blockId_t bid = append_Block(hinode->inode);
		hinode->inode.rem_bytes = buffSize;
		--hinode->inode.blocks;	// append_Block函数会默认加1
		disk.writeBlock(buff_temp, bid, 0, buffSize);
	}
	else {
		cout << "Recycling allBlock failed!" << endl;
		return false;
	}
	return true;

}

// 得到文件大小
diskSize_t FileSystem::get_fileSize(FileDir* curDir, string fname)
{
	inodeId_t* id = new inodeId_t();		//申请空闲i节点
	find_InodeId_inCurDir(curDir, fname, id);
	HashINode* hinode = insert_HashINode(*id);
	return hinode->inode.size();
}

/*
	HashINodeTable 哈希表
*/


// 插入一个哈希节点
HashINode* FileSystem::insert_HashINode(inodeId_t id)
{
	INode* inode = NULL;
	HashINode* hinode = hashINodeTable.find_HashINode(id);
	if (hinode == NULL) {	// 如果不在内存i节点队列中
		inode = new INode();
		disk.readInode((char*)inode, id);		// 读入一个i节点
		hinode = hashINodeTable.insert_INode(*inode);	// 插入哈希表中
	}

	return hinode;
}

// 保存一个HashINode节点中的INode内容到磁盘中
bool FileSystem::save_HashINode(HashINode* hinode)
{
	if (hinode->changed) {
		disk.writeInode((char*)&(hinode->inode), hinode->inode.iNodeId);
		return true;
	}
	else
		return false;
}

// 将内存组织的Hash队列中的一个HashINode根据changed更新外存
void FileSystem::save_allINode()
{
	for (int i = 0; i < 32; ++i) {
		int j = hashINodeTable.iNodeHashChain[i].num;
		HashINode* p = hashINodeTable.iNodeHashChain[i].start;

		while (j) {
			save_HashINode(p);
			p = p->next;
			--j;
		}
	}
	return;
}


/*
	Dir: 文件目录
*/

// 创建根目录
bool FileSystem::create_rootDir()
{
	// 默认根目录在1号i节点和34号磁盘块
	char name[32] = "/home";
	rootDir = new FileDir(name, 1, NULL);

	//初始化第一个i节点,空目录项指向第0个i节点，放到第一个磁盘i节点
	INode* inodeRoot = new INode(1, FILE_TYPE_DIR, 0);
	inodeRoot->rem_bytes = 0;
	inodeRoot->direct_data[0] = 34;
	inodeRoot->iNodeId = 1;

	// 插入哈希表中
	HashINode* hinode = hashINodeTable.insert_INode(*inodeRoot);
	hinode->changed = true;

	return true;
}

// 从磁盘中读出目录
FileDir* FileSystem::read_dirFromDisk(char name[], inodeId_t id, FileDir* parent)
{
	// 首先创立一个目录
	FileDir* curDir = new FileDir(name, id, parent);

	// 读入i节点的内容
	INode* inode = new INode();
	disk.readInode((char*)inode, id);

	// 如果不是目录，可以返回了
	if (!inode->isDir()) {
		return curDir;
	}

	// 插入哈希表中
	HashINode* hinode = hashINodeTable.insert_INode(*inode);

	// 读入目录的i节点号
	curDir->id = id;

	// 读入一块数据块
	char blockbuff[Block_Size] = {};
	blockId_t k_true = get_kBlock(hinode->inode, 0);	// 找到块的id
	int sfdsize = sizeof(int) + 32 + sizeof(inodeId_t);	// 一条sfd的长度
	disk.readBlock(blockbuff, k_true, 0, sfdsize);		// 读size长度的字节出来

	// 根据指针写入
	int sp = 0;
	memcpy((char*)&curDir->len, blockbuff + sp, sizeof(int));	// 读入 长度
	sp += sizeof(int);
	memcpy(curDir->dirName, blockbuff + sp, 32);	// 写入名字
	sp += 32;
	memcpy((char*)&curDir->id, blockbuff + sp, sizeof(inodeId_t));	// 写入i节点编号
	sp += sizeof(inodeId_t);

	memset(blockbuff, '\0', 512);
	int size = sfdsize * (curDir->len + 1);				// 读入的长度根据sfd的项数定
	disk.readBlock(blockbuff, k_true, 0, size);			// 读size长度的字节出来

	// 循环读入子目录
	for (int i = 0; i < curDir->len; ++i)
	{
		// 存的时候存整体，不按指针存
		int len = 0;
		memcpy((char*)&len, blockbuff + sp, sizeof(int));	// 写入长度
		sp += sizeof(len);

		char fname[32] = {};
		memcpy(fname, blockbuff + sp, 32);		// 写入名字
		sp += 32;

		inodeId_t id_temp = 0;
		memcpy((char*)&id_temp, blockbuff + sp, sizeof(inodeId_t));	// 写入i节点编号
		sp += sizeof(inodeId_t);

		FileDir* subDir = read_dirFromDisk(fname, id_temp, curDir);

		curDir->subDirs.push_back(subDir);	// 压栈

	}

	return curDir;

}

// 向磁盘中保存目录
bool FileSystem::write_dirToDisk(FileDir* curDir)
{

	// 写入一块数据块
	char blockbuff[Block_Size] = {};

	// 根据指针写入
	int sp = 0;

	memcpy(blockbuff + sp, (char*)&(curDir->len), sizeof(int));	// 写入长度
	sp += sizeof(int);

	memcpy(blockbuff + sp, curDir->dirName, 32);		// 写入名字
	sp += 32;

	memcpy(blockbuff + sp, (char*)&curDir->id, sizeof(inodeId_t));	// 写入i节点编号
	sp += sizeof(inodeId_t);

	// 循环写入子目录
	vector<FileDir*>::iterator iter = curDir->subDirs.begin();
	for (; iter != curDir->subDirs.end(); ++iter) {
		memcpy(blockbuff + sp, (char*)&(*iter)->len, sizeof(int));	// 写入长度
		sp += sizeof(int);

		memcpy(blockbuff + sp, (*iter)->dirName, 32);		// 写入名字
		sp += 32;

		memcpy(blockbuff + sp, (char*)&(*iter)->id, sizeof(inodeId_t));	// 写入i节点编号
		sp += sizeof(inodeId_t);

		// 递归调用
		HashINode* hinode = hashINodeTable.find_HashINode((*iter)->id);
		if (hinode != NULL) {
			if (hinode->inode.isDir())
				write_dirToDisk((*iter));
		}
		

	}

	HashINode* hinode = hashINodeTable.find_HashINode(curDir->id);
	write_file(hinode, blockbuff, sp, USER_ROOT_UID);

	return true;
}


// 将文件插入到当前目录下的部分
bool FileSystem::insert_InodeId_toCurDir(FileDir* curDir, string fname, inodeId_t id)
{
	FileDir* dir = new FileDir(fname.c_str(), id, curDir);
	dir->parent = curDir;
	dir->len = 0;

	curDir->subDirs.push_back(dir);
	curDir->len++;

	return true;
}

// 根据当前目录找到文件i节点的部分
bool FileSystem::find_InodeId_inCurDir(FileDir* curDir, string fname, inodeId_t* id)
{

	FileDir* mydir = find_srcDir_inCurDir(curDir, fname.c_str());
	if (mydir != NULL) {
		*id = mydir->id;
		return true;
	}
	return false;

}

// 根据当前目录名找到位置
FileDir* FileSystem::find_srcDir_inCurDir(FileDir* curDir, const char* srcDir)
{
	// 遍历子目录
	vector<FileDir*>::iterator iter = curDir->subDirs.begin();
	for (; iter != curDir->subDirs.end(); ++iter) {
		if (!strcmp((*iter)->dirName, srcDir)) {
			HashINode* hinode = hashINodeTable.find_HashINode((*iter)->id);
			if (hinode != NULL) {
				if (hinode->inode.isLink()) {
					//读取文件内容
					diskSize_t len = hinode->inode.size();
					char buff[512] = {};
					buff[len] = 0;
					this->read_file(hinode, buff, len, USER_ROOT_UID);
					string path(buff);
					FileDir* fdir = this->str2filedir(this->rootDir, path);
					return fdir;
				}
			}
			return (*iter);
		}
	}

	return NULL;
}

// 删除当前目录中指向文件i节点的部分
bool FileSystem::del_InodeId_inCurDir(FileDir* curDir, inodeId_t id)
{
	vector<FileDir*>::iterator iter = curDir->subDirs.begin();
	for (; iter != curDir->subDirs.end(); ++iter) {
		if ((*iter)->id == id) {
			curDir->subDirs.erase(iter);
			--curDir->len;
			return true;	// 删除该节点
		}
	}
	return false;
}

// 根据目录名找到位置
FileDir* FileSystem::find_srcDir(FileDir* curDir, const char* srcDir)
{
	FileDir* ret = NULL;

	if (!strcmp(curDir->dirName, srcDir)) {
		HashINode* hinode = this->hashINodeTable.find_HashINode(curDir->id);
		if (hinode->inode.isLink()) {
			//读取文件内容
			diskSize_t len = hinode->inode.size();
			char buff[512] = {};
			buff[len] = 0;
			this->read_file(hinode, buff, len, USER_ROOT_UID);
			string path(buff);
			FileDir* fdir = this->str2filedir(this->rootDir, path);
			return fdir;
		}
		return curDir;
	}

	// 遍历子目录
	vector<FileDir*>::iterator iter = curDir->subDirs.begin();
	for (; iter != curDir->subDirs.end(); ++iter) {
		ret = find_srcDir((*iter), srcDir);
		if (ret != NULL)
			return ret;
	}

	return NULL;

}


 /*
	 所有的CMD命令操作
 */


// 创建文件（当文件不存在时）create fild {fname} if not exist
bool FileSystem::touch(FileDir* curDir, string fname, userId_t uid)
{
	// 首先建立文件
	inodeId_t id = create_file(curDir, fname, uid, FILE_TYPE_FILE);
	// 将文件插入当前路径
	insert_InodeId_toCurDir(curDir, fname, id);
	return true;
}

// 删除文件（包括i节点和数据），需要判断是否有链接
bool FileSystem::del(FileDir* curDir, string fname, userId_t uid)
{
	inodeId_t* id = new inodeId_t();

	find_InodeId_inCurDir(curDir, fname, id);
	HashINode* hinode = hashINodeTable.find_HashINode(*id);

	if (hinode == NULL) {
		cout << "文件未打开" << endl;
		return false;
	}

	if (uid != hinode->inode.owner && uid != USER_ROOT_UID) {				// 判断是否是当前用户
		cout << "Access denied!" << endl;
		return false;
	}

	hinode->changed = true;

	// 需要判断是否有链接
	if (hinode->inode.hlinkNum > 1) {
		hinode->inode.hlinkNum--;
		del_InodeId_inCurDir(curDir, *id);
		return true;
	}

	recycle_allBlock(hinode->inode);	// 回收所有数据块

	if (!hashINodeTable.delete_HashINode(*id)) {	// 如果没删除成功
		cout << "Deleting inode inside memory is false!" << endl;
	}		

	superblock.recycleInode(*id);	// 回收i节点编号

	//删除当前目录中指向文件i节点的部分
	del_InodeId_inCurDir(curDir, *id);

	return true;

}

// 打开文件
bool FileSystem::open(FileDir* curDir, string fname, userId_t uid, HashINode* hinode)
{

	inodeId_t* id = new inodeId_t();
	find_srcDir_inCurDir(curDir, fname.c_str());
	hinode = insert_HashINode(*id);

	if (uid != hinode->inode.owner && uid != USER_ROOT_UID) {			// 判断是否是当前用户
		cout << "Access denied!" << endl;
		hashINodeTable.delete_HashINode(*id);	// 没有权限，删除该哈希节点
		return false;
	}

}

// 读文件
bool FileSystem::cat(FileDir* curDir, string fname, char* buff, userId_t uid)
{
	inodeId_t* id = new inodeId_t();

	find_InodeId_inCurDir(curDir, fname, id);
	HashINode* hinode = hashINodeTable.find_HashINode(*id);

	if (hinode == NULL) {
		cout << "文件未打开" << endl;
		return false;
	}


	return read_file(hinode, buff, hinode->inode.size(), uid);

}

// 写文件，默认覆盖
bool FileSystem::write(FileDir* curDir, const char* buff, string fname, userId_t uid)
{
	inodeId_t* id = new inodeId_t();

	find_InodeId_inCurDir(curDir, fname, id);
	HashINode* hinode = hashINodeTable.find_HashINode(*id);

	if (hinode == NULL) {
		cout << "文件未打开" << endl;
		return false;
	}


	diskSize_t buffSize = strlen(buff);	// 缓冲区大小
	return write_file(hinode, buff, buffSize, uid);
}

// 追加文件
bool FileSystem::echo(FileDir* curDir, const char* buff, string fname, userId_t uid)
{
	inodeId_t* id = new inodeId_t();

	find_InodeId_inCurDir(curDir, fname, id);
	HashINode* hinode = hashINodeTable.find_HashINode(*id);

	if (hinode == NULL) {
		cout << "文件未打开" << endl;
		return false;
	}

	hinode->changed = 1;

	diskSize_t buffSize = strlen(buff);	// 缓冲区大小
	char buff_temp[Block_Size] = {};		// 写入用“缓冲”
	int range = Block_Size - hinode->inode.rem_bytes;

	if (buffSize < range) {

		hinode->inode.atime = hinode->inode.mtime = time(NULL);

		blockId_t bid = get_kBlock(hinode->inode, hinode->inode.blocks);
		disk.writeBlock(buff, bid, hinode->inode.rem_bytes, buffSize);	// 直接写入
		hinode->inode.rem_bytes += buffSize;	// 如果要追加的少，那么就写一个buffSize的内容
		return true;
	}
	else {
		buffSize = buffSize - range;	// 减去已经写的部分
		blockId_t bid = get_kBlock(hinode->inode, hinode->inode.blocks);
		// 拷贝一块大小的内容
		memcpy(buff_temp, buff, range);
		disk.writeBlock(buff_temp, bid, hinode->inode.rem_bytes, range);	// 直接写入
		++hinode->inode.blocks;
		hinode->inode.rem_bytes = 0; // 如果要追加的多，那么就写满当前余下的块
	}
	
	int blocks = buffSize / Block_Size;	// 还需要添加的新块
	int remBytes = buffSize - blocks * Block_Size;	// 余下的字节
	
	// 需要写多个块
	int i = 0;
	for (i = 0; i < blocks; ++i) {
		// 拷贝一块大小的内容
		memcpy(buff_temp, buff + range + i * Block_Size, Block_Size);
		// 分配块并写入
		blockId_t bid = append_Block(hinode->inode);
		disk.writeBlock(buff_temp, bid);
		// 减去已经写入的
		buffSize -= Block_Size;		// 减去已经写入的
	}

	// 拷贝余下大小的内容
		
	memcpy(buff_temp, buff + range + i * Block_Size, remBytes);
	// 分配块并写入
	blockId_t bid = append_Block(hinode->inode);
	--hinode->inode.blocks;	// append_Block函数会默认加1
	disk.writeBlock(buff_temp, bid);
	hinode->inode.rem_bytes = remBytes; // 如果要追加的多，那么就写满当前余下的块
	cout << bid << "\t";
	cout << hinode->inode.blocks << "\t" << hinode->inode.size() << endl;
	return true;

}

// 创建目录
bool FileSystem::mkdir(FileDir* curDir, string fname, userId_t uid)
{
	// 首先建立目录文件
	inodeId_t id = create_file(curDir, fname, uid, FILE_TYPE_DIR);
	// 将文件插入当前路径
	insert_InodeId_toCurDir(curDir, fname, id);
	return true;
}

// 删除目录
bool FileSystem::rmdir(FileDir* curDir, string fname, userId_t uid)
{
	FileDir* src = find_srcDir(curDir, fname.c_str());
	if (src->len != 0)
	{
		while (1)
		{
			string xsname = src->dirName;
			cout << xsname << "目录下有文件，是否继续删除（y/n）:" << endl;
			string s;
			cin >> s;
			if (s == "n")
				return false;
			else if (s == "y")
			{
				for (int i = 0; i < src->subDirs.size(); i++)
				{
					//将文件名转换成string类型
					string name = src->subDirs[i]->dirName;
					HashINode* hinode = insert_HashINode(src->subDirs[i]->id);
					if (hinode->inode.isDir())
					{
						//文件类型是目录文件,调用删除目录函数
						if (rmdir(src, name, uid))
							--i;
					}
					else
					{
						//文件类型是普通文件,调用删除文件函数
						if (del(src, name, uid)) {
							//cout << name << "文件删除成功！" << endl;
							--i;
						}

					}
				}
				// 把目录下文件删除完毕后，在当前目录中删除目录项
				del_InodeId_inCurDir(curDir, src->id);
				//cout << xsname << "删除成功！" << endl;
				break;
			}
			else
			{
				cout << "输入有误，请重新输入！" << endl;
			}
		}
	}
	else
	{
		del_InodeId_inCurDir(curDir, src->id);
		//cout << fname << "删除成功！" << endl;
	}
	cout  << "删除成功！" << endl;
	return true;
}

// 列出当前目录中内容
bool FileSystem::ls(FileDir* curDir, userId_t uid)
{
	vector<FileDir*>::iterator iter = curDir->subDirs.begin();
	if (curDir->len == 0) {
		cout << "None!" << endl;
		return false;
	}

	for (; iter != curDir->subDirs.end(); ++iter) {
		cout << (*iter)->dirName << "\t";
	}
	cout << endl;
	return true;
}

// 进入当前目录的子目录
FileDir* FileSystem::cd(FileDir* curDir, string src)
{

	HashINode* hinode = insert_HashINode(curDir->id);
	if (hinode->inode.isDir()) {
		return find_srcDir_inCurDir(curDir, src.c_str());
	}
	else { 
		cout << "此文件非目录，无法进入！" << endl; 
		return NULL;
	};

}

FileDir* FileSystem::cd_parent(FileDir* curDir)
{
	if (curDir == rootDir)
	{
		cout << "当前为根目录，无法再返回上级目录！" << endl;
		return curDir;
	}
	else
	{
		return curDir->parent;
	}
}

FileDir* FileSystem::cd_home()
{
	return rootDir;
}

FileDir* FileSystem::get_temp_dir(string src, int m)
{
	string dir[20];
	char c_temp[MAX];
	strcpy_s(c_temp, src.c_str());
	char* tp = NULL;
	char* p = strtok_s(c_temp, "/", &tp);

	int j = 0;
	while (p != NULL)
	{
		dir[j] = p;
		j++;
		p = strtok_s(NULL, "/", &tp);
	}
	FileDir* temp = rootDir;
	for (int i = 1; i < j - m; i++)
	{
		strcpy_s(c_temp, dir[i].c_str());
		temp = find_srcDir(temp, c_temp);
	}
	return temp;

}

void FileSystem::find_Dir(FileDir* curDir, const char srcDir[])
{

	// 遍历子目录
	vector<FileDir*>::iterator iter = curDir->subDirs.begin();
	for (; iter != curDir->subDirs.end(); ++iter) {
		find_Dir((*iter), srcDir);
	}

	if (!strcmp(curDir->dirName, srcDir)) {
		cout << curDir->get_fullDir() << endl;
	}
}


//重命名
void FileSystem::rename(FileDir* curDir, string oldname, string newname)
{
	int flag = 0;
	for (int i = 0; i < curDir->subDirs.size(); i++)
	{
		if (strcmp(curDir->subDirs[i]->dirName, newname.c_str()) == 0)
		{
			cout << "命名冲突，请修改文件名称！" << endl;
			flag = 1;
			break;
		}
	}
	if (flag == 0)
	{
		for (int i = 0; i < curDir->subDirs.size(); i++)
		{
			if (strcmp(curDir->subDirs[i]->dirName, oldname.c_str()) == 0)
			{
				strcpy_s(curDir->subDirs[i]->dirName, newname.c_str());
				break;
			}
		}
	}

}


FileDir* FileSystem::str2filedir(FileDir* curDir, string path)
{
	// 截去根目录
	if (path[0] == '/') {
		path = path.substr(path.find_first_of('/', 1) + 1);
	}
	int index = path.find_first_of('/');
	string nextname = path.substr(0, index);
	string nextpath = "";
	if (index != -1)
		nextpath = path.substr(index + 1);
	vector<FileDir*>::iterator iter = curDir->subDirs.begin();
	for (; iter != curDir->subDirs.end(); ++iter) {
		if (!strcmp((*iter)->dirName, nextname.c_str()))
		{
			if (nextpath != "") {
				return this->str2filedir(*iter, nextpath);
			}
			else
				return *iter;
		}
	}
}


// 软链接
bool FileSystem::ln_soft(string sourceFile, string targetFile, userId_t uid)
{
	//切分目标文件夹和文件名
	string targetPath, targetName;
	int index = targetFile.find_last_of('/');
	targetPath = targetFile.substr(0, index); //  /home/wht
	targetName = targetFile.substr(index + 1);
	//找到目标文件夹目录
	//FileDir* targetI = this->find_srcDir(this->rootDir, targetPath.c_str());
	FileDir* targetI = str2filedir(this->rootDir, targetPath);
	//创建软连接文件
	inodeId_t targetIid = this->create_file(targetI, targetName, uid, FILE_TYPE_LINK);
	this->insert_InodeId_toCurDir(targetI, targetName, targetIid);
	//找到i节点,将原文件绝对路径写入
	HashINode* hinode = this->hashINodeTable.find_HashINode(targetIid);
	//cout << sizeof(sourceFile.c_str()) << endl;
	this->write_file(hinode, sourceFile.c_str(), sizeof(sourceFile) + 1, uid);

	return true;
}
// 硬链接
bool FileSystem::ln_hard(string sourceFile, string targetFile)
{
	//切分目标文件夹和文件名
	string targetPath, targetName;
	int index = targetFile.find_last_of('/');
	targetPath = targetFile.substr(0, index); //  /home/wht
	targetName = targetFile.substr(index + 1);
	// 找到目标文件夹i目录项
	//FileDir* targetI = find_srcDir(this->rootDir, targetPath.c_str());
	//FileDir* targetI = get_temp_dir(targetPath.c_str(), 0);
	FileDir* targetI = str2filedir(this->rootDir, targetPath);
	// 找到源文件i节点id
	//FileDir* sourceI = find_srcDir(this->rootDir, sourceFile.c_str());
	//FileDir* sourceI = get_temp_dir(sourceFile.c_str(), 0);
	FileDir* sourceI = str2filedir(this->rootDir, sourceFile);
	if (targetI == NULL || sourceI == NULL)
		return false;
	inodeId_t sourceIid = sourceI->id;
	//将目录项插入目标文件夹
	this->insert_InodeId_toCurDir(targetI, targetName, sourceIid);
	//源文件i节点id的link数+1
	HashINode* hinode = this->hashINodeTable.find_HashINode(sourceIid);
	hinode->changed = true;
	hinode->inode.hlinkNum++;

	return true;
}

bool FileSystem::cp(string sourceFile, string targetFile, userId_t uid)
{
	//切分目标文件夹和文件名
	string targetPath, targetName;
	int index = targetFile.find_last_of('/');
	targetPath = targetFile.substr(0, index); //  /home/wht
	targetName = targetFile.substr(index + 1);
	// 找到目标文件夹i目录项
	FileDir* targetI = str2filedir(this->rootDir, targetPath);
	// 创建文件
	touch(targetI, targetName, uid);
	// 找到新文件id
	FileDir* newI = str2filedir(targetI, targetName);
	inodeId_t newIid = newI->id;
	// 找到源文件i节点id
	FileDir* sourceI = str2filedir(this->rootDir, sourceFile);
	inodeId_t sourceIid = sourceI->id;
	HashINode* hinode = this->hashINodeTable.find_HashINode(sourceIid);
	diskSize_t len = hinode->inode.size();
	// 读取源文件内容
	char buff[512] = {};
	buff[len] = 0;
	this->read_file(hinode, buff, len, uid);
	//cout <<"len:"<<len <<" buff:" <<buff << endl;
	// 找到新文件hinode,并写入文件
	HashINode* hinode1 = this->hashINodeTable.find_HashINode(newIid);
	this->write_file(hinode1, buff, len, uid);
	return true;

}

//格式化
void FileSystem::format(FileDir* curDir, userId_t uid)
{
	string name = curDir->dirName;
	curDir = get_rootDir();
	rmdir(curDir, name, uid);
	mkdir(curDir, name, uid);
	cd(curDir, name);
}