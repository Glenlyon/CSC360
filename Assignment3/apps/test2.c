#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "File.h"

int mountDisk(char* diskName){
	disk = fopen("vdisk", "r+b");
	if(!disk){
		printf("Failed to mount the disk\n");
		return -1;
	}
	systemAddress = (u_int8_t*)calloc(DISK_SIZE, sizeof(char));
	fread(systemAddress, sizeof(struct otherBlock), 4096, disk);
	struct superBlock *sb = (struct superBlock*)systemAddress;
	if(sb->magic_num != 0x53EF){
		printf("File type unknown");
		return -1;
	}
    root = (struct directory *)getBlockAddress(3);
    current = root;
    path[0] = '\\';
    path[1] = '\0';
	return 0;
}

int main(int argc, char*arg[]){
	if(mountDisk("vdisk") == -1){
		perror("vdisk does not exist, test2 is terminated\n");
		exit(0);
	}
	printf("Disk mounted\n");
}