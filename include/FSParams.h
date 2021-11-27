#pragma once

/*

Description: the Parameters of File System
			 该文件包括我们建立的文件系统的必要参数

*/

struct INode;

// Disk 磁盘的参数
typedef		unsigned long long	diskSize_t;			// 磁盘大小定义的数据类型
typedef		diskSize_t		diskAddr_t;
const		unsigned int	data_blockNum = 512;	// 数据块一共512块

// Block 块的参数
typedef		unsigned int	blockId_t;				// 块号数据类型是 unsigned int
typedef		unsigned short	blockSize_t;			// 块大小的数据类型
typedef		blockSize_t		blockAddr_t;			// 块地址的数据类型

const		blockSize_t		Block_Size = 512;		// 一块的大小是512字节
const		blockId_t		Boot_BlockId = 0;		// 0# 是引导块
const		blockId_t		Super_BlcokId = 1;		// 1# 是超级块
const		blockId_t		Root_BlcokId = 2;		// 2# 开始是磁盘索引节点块

const		unsigned short	BID_LEN = sizeof(blockId_t);	 // 块号的大小
const		unsigned short	BSIZE_LEN = sizeof(blockSize_t); // 记录空闲块多少的变量大小
const		unsigned int	GROUP_SIZE = 49; //成组链接技术用


// INode i节点的参数
typedef 	unsigned short	inodeId_t;				// i节点编号数据类型
typedef		long long		diskOff_t;				// wht定义的
typedef		unsigned short	fMode_t;
const		blockId_t		index_blockNum = 32;	// 存储i节点的物理块定义为32块

#define		INODE_SIZE			sizeof(INode)
#define		INDEX_ARRAYNUM		12					// 索引数组一共12项
#define		INODE_DIRECT_CNT	10					// 直接索引10项	
#define		INODE_PER_BLOCK		(Block_Size / INODE_SIZE) // 一个块可以存放的iNode数目
#define		INODE_INDIRECT_CNT1	(Block_Size / BID_LEN)  // 一级间址的块地址数目
#define		INODE_INDIRECT_CNT2	(diskAddr_t)INODE_INDIRECT_CNT1 * INODE_INDIRECT_CNT1 
														  // 二级间址的块地址数目
const		unsigned int		InodeNum = index_blockNum * 4;
const		unsigned int		Bitmap_offset = 440;


// File Mode  --- 用不同的实数区分文件的类型
const		blockId_t		BlockNum = 2 + index_blockNum + data_blockNum;

#define		OWNER_R			1<<5		// 所有者的读权限
#define		OWNER_W			1<<4		// 所有者的写权限
#define		OWNER_X			1<<3		// X表示允许执行权限
#define		PUBLIC_R		1<<2		// 公共可读
#define		PUBLIC_W		1<<1		// 公共可写
#define		PUBLIC_X		1<<0

#define		OWNER_ALL		(OWNER_R | OWNER_W | OWNER_X)
#define		PUBLIC_ALL		(PUBLIC_R | PUBLIC_W | PUBLIC_X)
#define		DIR_DEFAULT		(OWNER_ALL | PUBLIC_R | PUBLIC_X)
#define		FILE_DEFAULT	(OWNER_R | OWNER_W | PUBLIC_R)

#define		FILE_TYPE_ALL	(3 << 6)	// 文件类型的所有定义
#define		FILE_TYPE_FILE	(0 << 6)	// 普通文件类型
#define		FILE_TYPE_DIR	(1 << 6)	// 目录文件类型
#define		FILE_TYPE_LINK	(2 << 6)	// 链接文件类型

// User 用户
typedef 	 short	userId_t;
const		userId_t		USER_ROOT_UID = 0;

#define		USER_HOME_DIR	"/home"
# define	USER_NAME_SIZE	20
# define	USER_PWD_SIZE	20
# define	MAX_USER_NUM	10

// UI
#define OPT_NUM 17
#define MAX 50



