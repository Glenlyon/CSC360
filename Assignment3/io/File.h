#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAGIC_NUMBER 0x53EF
#define DISK_SIZE NUM_BLOCKS*BLOCK_SIZE
#define NUM_BLOCKS 4096
#define BLOCK_SIZE 512
#define INODE_SIZE 32

typedef unsigned char   BYTE; 
typedef unsigned short  WORD;

typedef struct superBlock{
	u_int32_t magic_num;
	u_int32_t numberOfBlocks;
	u_int32_t numberofInodes;
}superBlock;

typedef struct freeBlockVector{
	u_int8_t freeBlock[BLOCK_SIZE];
}freeBlockVector;

typedef struct otherBlock{
	u_int8_t block[BLOCK_SIZE];
}otherBlock;

typedef struct inode{
	u_int32_t fileSize;
	u_int32_t flags;
	u_int16_t blocks[10];
	u_int16_t single_indirectBlock;
	u_int16_t double_indirectBlock;
}inode;

typedef struct entry{
	u_int8_t inodeNumber;
	u_int8_t fileName[31];
}entry;

typedef struct directory{
	struct entry map[16];
}directory;

extern int NUM_INODES;
extern struct directory *root;
extern struct directory *current;
extern char path[256];
extern u_int16_t subdirCount;
extern u_int8_t *systemAddress;
extern FILE *disk;

int32_t createInode(struct inode *node, u_int16_t blockNumber, u_int16_t fileSize, u_int32_t flags, u_int16_t numberOfBlock);
u_int8_t *getBlockAddress(u_int16_t blockNumber);
struct inode *getInodeAddress(u_int8_t inodeNumber);
u_int8_t getInodeNumber(struct inode *address);
int checkUsability(int blockNum);
u_int32_t findBlock(int numberOfBlock);
void reverseUsability(int blockNum);
struct inode* allocateInode();
u_int8_t findEmptyEntry(struct directory *dir);
u_int8_t findEntry(struct directory *dir, char* fileName);
void rootDirctory();
int32_t addToDirectory(char *fileName, struct directory *dir, u_int8_t inodeNumber);
int32_t createSubdirectory(char* directoryName);
int32_t createFile(char *fileName, u_int16_t size);
int32_t writeFile(char *fileName, char* data);
void createDisk();
int32_t readFile(char* fileName);
int32_t changeCurrentDirectory(char* directoryName);
