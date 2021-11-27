#pragma once

/*

Description: the Parameters of File System
			 ���ļ��������ǽ������ļ�ϵͳ�ı�Ҫ����

*/

struct INode;

// Disk ���̵Ĳ���
typedef		unsigned long long	diskSize_t;			// ���̴�С�������������
typedef		diskSize_t		diskAddr_t;
const		unsigned int	data_blockNum = 512;	// ���ݿ�һ��512��

// Block ��Ĳ���
typedef		unsigned int	blockId_t;				// ������������� unsigned int
typedef		unsigned short	blockSize_t;			// ���С����������
typedef		blockSize_t		blockAddr_t;			// ���ַ����������

const		blockSize_t		Block_Size = 512;		// һ��Ĵ�С��512�ֽ�
const		blockId_t		Boot_BlockId = 0;		// 0# ��������
const		blockId_t		Super_BlcokId = 1;		// 1# �ǳ�����
const		blockId_t		Root_BlcokId = 2;		// 2# ��ʼ�Ǵ��������ڵ��

const		unsigned short	BID_LEN = sizeof(blockId_t);	 // ��ŵĴ�С
const		unsigned short	BSIZE_LEN = sizeof(blockSize_t); // ��¼���п���ٵı�����С
const		unsigned int	GROUP_SIZE = 49; //�������Ӽ�����


// INode i�ڵ�Ĳ���
typedef 	unsigned short	inodeId_t;				// i�ڵ�����������
typedef		long long		diskOff_t;				// wht�����
typedef		unsigned short	fMode_t;
const		blockId_t		index_blockNum = 32;	// �洢i�ڵ������鶨��Ϊ32��

#define		INODE_SIZE			sizeof(INode)
#define		INDEX_ARRAYNUM		12					// ��������һ��12��
#define		INODE_DIRECT_CNT	10					// ֱ������10��	
#define		INODE_PER_BLOCK		(Block_Size / INODE_SIZE) // һ������Դ�ŵ�iNode��Ŀ
#define		INODE_INDIRECT_CNT1	(Block_Size / BID_LEN)  // һ����ַ�Ŀ��ַ��Ŀ
#define		INODE_INDIRECT_CNT2	(diskAddr_t)INODE_INDIRECT_CNT1 * INODE_INDIRECT_CNT1 
														  // ������ַ�Ŀ��ַ��Ŀ
const		unsigned int		InodeNum = index_blockNum * 4;
const		unsigned int		Bitmap_offset = 440;


// File Mode  --- �ò�ͬ��ʵ�������ļ�������
const		blockId_t		BlockNum = 2 + index_blockNum + data_blockNum;

#define		OWNER_R			1<<5		// �����ߵĶ�Ȩ��
#define		OWNER_W			1<<4		// �����ߵ�дȨ��
#define		OWNER_X			1<<3		// X��ʾ����ִ��Ȩ��
#define		PUBLIC_R		1<<2		// �����ɶ�
#define		PUBLIC_W		1<<1		// ������д
#define		PUBLIC_X		1<<0

#define		OWNER_ALL		(OWNER_R | OWNER_W | OWNER_X)
#define		PUBLIC_ALL		(PUBLIC_R | PUBLIC_W | PUBLIC_X)
#define		DIR_DEFAULT		(OWNER_ALL | PUBLIC_R | PUBLIC_X)
#define		FILE_DEFAULT	(OWNER_R | OWNER_W | PUBLIC_R)

#define		FILE_TYPE_ALL	(3 << 6)	// �ļ����͵����ж���
#define		FILE_TYPE_FILE	(0 << 6)	// ��ͨ�ļ�����
#define		FILE_TYPE_DIR	(1 << 6)	// Ŀ¼�ļ�����
#define		FILE_TYPE_LINK	(2 << 6)	// �����ļ�����

// User �û�
typedef 	 short	userId_t;
const		userId_t		USER_ROOT_UID = 0;

#define		USER_HOME_DIR	"/home"
# define	USER_NAME_SIZE	20
# define	USER_PWD_SIZE	20
# define	MAX_USER_NUM	10

// UI
#define OPT_NUM 17
#define MAX 50



