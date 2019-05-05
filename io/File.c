#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "File.h"

int NUM_INODES = 0;
struct directory *root;
struct directory *current;
char path[256] = {};
u_int16_t subdirCount = 4;
u_int8_t *systemAddress;
FILE *disk;

//set up inode for file and directory
int32_t createInode(struct inode *node, u_int16_t blockNumber, u_int16_t fileSize, u_int32_t flags, u_int16_t numberOfBlock){
	int i;
	node->fileSize = fileSize;
	node->flags = flags;
	for(i = 0; i < numberOfBlock; i++){
		node->blocks[i] = blockNumber+i;

	}
	node->single_indirectBlock = 0;
	node->double_indirectBlock = 0;
}

u_int8_t *getBlockAddress(u_int16_t blockNumber){
	return systemAddress + blockNumber*BLOCK_SIZE;
}	

struct inode *getInodeAddress(u_int8_t inodeNumber){
	struct otherBlock *inodeMap = (struct otherBlock *)getBlockAddress(2);
	struct inode *temp = (struct inode *)(&inodeMap[inodeNumber/16]) + (inodeNumber%16);
	return temp;
}

u_int8_t getInodeNumber(struct inode *address){
	struct block *inodeBlock = (struct block *)getBlockAddress(2);
	u_int16_t inodePosition = address - (struct inode*)inodeBlock;
	return inodePosition;
}

/*
int32_t findBlock(u_int16_t numberOfBlock){
	u_int16_t count = 0;
	u_int16_t firstBlock = 0;
	int i, j, k,bitNumInByte,byteNumInFBV;
	struct freeBlockVector *fbv = (struct freeBlockVector *)getBlockAddress(1);
	for(i = 10; i < NUM_BLOCKS; i++){
		bitNumInByte = i%8;
		if(bitNumInByte == 0){
			bitNumInByte = 8;
			byteNumInFBV = (i/8)-1;
		}else{
			byteNumInFBV = i/8;
		}
		if(((0b00000001 << (8-bitNumInByte)) & fbv->freeBlock[byteNumInFBV]) != 0){
			int flag=0b11111110;
			for(j = 0; j < 8 - bitNumInByte; j++){
				flag = (flag << 1) + 1;
			}			
			if(count == 0){
				firstBlock = i;
			}
			count++;
			if(numberOfBlock == count){
				printf("%d", firstBlock);
				for(k = 0; k < firstBlock + numberOfBlock; k++){
					fbv->freeBlock[byteNumInFBV] = fbv->freeBlock[byteNumInFBV] & flag;
				}
				return firstBlock;
			}
		}
	}
	return -1;
}
*/


//find a usable block through free block vector, and switch the corrospoding bit from 1 to 0
u_int32_t findBlock(int numberOfBlock){
	int i, j;
	int byteNumInVB = 0;
	int bitNumInByte = 0;
	struct freeBlockVector *fbv = (struct freeBlockVector*)getBlockAddress(1);
	for(i = 10; i < NUM_BLOCKS; i++){
		byteNumInVB = 0;
		bitNumInByte = i%8;
		if(bitNumInByte == 0){
			bitNumInByte = 8;
			byteNumInVB = (i/8)-1;		
		}else{
			byteNumInVB = i/8;	
		}
		//printf("%d\n", i); 
		if(((0b00000001 << (8-bitNumInByte)) & fbv->freeBlock[byteNumInVB]) != 0){
			u_int32_t firstBlock = i;
			for(j = 0; j < numberOfBlock; j++){
				byteNumInVB = 0;
				bitNumInByte = (i+j)%8;
				if(bitNumInByte == 0){
					bitNumInByte = 8;
					byteNumInVB = (i+j)/8-1;		
				}else{
					byteNumInVB = (i+j)/8;	
				}
				int flag = 0b11111110;
				int k;	
				for(k = 0; k < 8 - bitNumInByte; k++){
					flag = (flag << 1) + 1;
				}
				fbv->freeBlock[byteNumInVB] = fbv->freeBlock[byteNumInVB] & flag;
			}
			return firstBlock;	
		}
	}
}

//allocate an inode on the inode map and increase inode count
struct inode* allocateInode(){
	struct otherBlock *inodeMap = (struct otherBlock*)getBlockAddress(2);
	struct inode *temp;
	struct superBlock *sb = (struct superBlock *)getBlockAddress(0);
	if(sb->numberofInodes == 4086){	
		return NULL;
	}
	sb->numberofInodes++;
	temp = getInodeAddress(sb->numberofInodes);
	return temp;
}

//find an empty entry for new file
u_int8_t findEmptyEntry(struct directory *dir){
	int i;
	for(i = 0; i<16; i++){
		if(dir->map[i].inodeNumber ==0){
			return i;
		}
	}
	return -1;
}

//find an existing entry
u_int8_t findEntry(struct directory *dir, char* fileName){
	int i;
	for(i = 0; i<16; i++){
		if(!strcmp(dir->map[i].fileName, fileName)){
			return i;
		}
	}
	return -1;
}

//set the defult directory as the root
void rootDirctory(){
	struct inode *rootNode = allocateInode();
	root = (struct directory *)getBlockAddress(3);
	current = root;
	path[0] = '\\';
	path[1] = '\0';
}

//add file to the current directory
int32_t addToDirectory(char *fileName, struct directory *dir, u_int8_t inodeNumber){
	int8_t temp = findEmptyEntry(dir);
	dir->map[temp].inodeNumber = inodeNumber;
	strcpy(dir->map[temp].fileName, fileName);
	return 0;
}

//create subdirectory
int32_t createSubdirectory(char* directoryName){
	subdirCount++;
	struct inode *node = allocateInode();
	createInode(node, subdirCount, 0, 1, 1);
	u_int8_t inodeNumber = getInodeNumber(node);
	addToDirectory(directoryName, current, inodeNumber);
	return 0;
}

//allocate blocks with corrosponding size for an empty file
int32_t createFile(char *fileName, u_int16_t size){
	struct inode *node = allocateInode();
	u_int16_t numberOfBlock;
	if(size%512 == 0){
		numberOfBlock = size/512;
	}else{
		numberOfBlock = size/512 + 1;
	}
	u_int16_t fileBlock = findBlock(numberOfBlock);

	createInode(node, fileBlock, size, 0, numberOfBlock);
	u_int8_t inodeNumber = getInodeNumber(node);
	addToDirectory(fileName, current, inodeNumber);
	return 0;
}

//load content in to the file and its corrosponding blocks
int32_t writeFile(char *fileName, char *data){
	int8_t fileEntry = findEntry(current, fileName);
	if(fileEntry == -1){
		perror("File does not exist\n");
		return -1;
	}
	u_int8_t inodeNumber = current->map[fileEntry].inodeNumber;
	struct inode* node = getInodeAddress(inodeNumber);
	u_int8_t numberOfBlock = strlen(data)/512;
	u_int8_t nByte = strlen(data)%512;
	u_int16_t fileBlock = node->blocks[0];
	struct otherBlock *file =(struct otherBlock *)getBlockAddress(fileBlock);
	u_int8_t i;
	for(i = 0; i<numberOfBlock; i++){
		memcpy((void *)(file+i), (void *)(data + i*512), 512);
	}
	memcpy((void *)(file+i), (void *)(data + i*512), nByte);
	return 0;
}

//initiate disk;
void createDisk(){
	int i, j;
	disk = fopen("vdisk", "wb");
	systemAddress = (u_int8_t *)calloc(DISK_SIZE, sizeof(char));
	struct otherBlock *blk = (struct otherBlock *)systemAddress;
	struct superBlock *sb = (struct superBlock *)blk;
	sb->magic_num = MAGIC_NUMBER;
	sb->numberOfBlocks = NUM_BLOCKS;
	sb->numberofInodes = 0;
	blk += 1;
	struct freeBlockVector *fbv = (struct freeBlockVector*)blk;
	fbv->freeBlock[1] = 63;
	for(j = 2; j < BLOCK_SIZE; j++){
		fbv->freeBlock[j] = 255;
	}
	rootDirctory();
	
}

//read the content from the file
int32_t readFile(char* fileName){
	int i = 0;
	int8_t fileEntry = findEntry(current, fileName);
	if(fileEntry == -1){
		perror("File does not exist\n");
		return -1;
	}
	u_int8_t inodeNumber = current->map[fileEntry].inodeNumber;
	struct inode* node = getInodeAddress(inodeNumber);
	u_int16_t fileBlock = node->blocks[0];
	u_int32_t fileSize = node->fileSize;
	struct otherBlock *file = (struct otherBlock *)getBlockAddress(fileBlock);
	for(i = 0; i < fileSize; i++){
		printf("%c", *((char *)file+i));
	}
	printf("\n");
}

//swap to a subdirectory
int32_t changeCurrentDirectory(char* directoryName){
	u_int8_t entry = findEntry(current, directoryName);
	if(entry == -1){
		perror("Directory Does not exist\n");
		return -1;
	}
	u_int8_t inodeNumber = current->map[entry].inodeNumber;
	struct inode* node = getInodeAddress(inodeNumber);
	struct directory *dir = (struct directory*)getBlockAddress(node->blocks[0]);
	current = dir;
	return 0;
}
