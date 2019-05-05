#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "File.h"

void prompt(char* prompt){
	printf("%s is created\n", prompt);
}

int main(int argc, char*arg[]){
	createDisk();
	createFile("test1", 1024);
	prompt("test1");
	if(writeFile("test1", "This is Test 1") == 0){
		printf("Content loaded into test1\n");
	}
	if(createFile("test2", 1026) != 0){
		perror("Failed to Create File\n");
		exit(0);
	}
	prompt("test2");
	if(writeFile("test2", "This is Test 2") == 0){
		printf("Content loaded into test2\n");
	}
	printf("Reading content from test1:\n");
	readFile("test1");
	
	printf("Reading content from test15：\n");
	readFile("test5");
	
	createFile("test3", 512);
	prompt("test3");
	if(writeFile("test3", "This is Test 3") == 0){
		printf("Content loaded into test3\n");
	}
	if(createSubdirectory("d1") != 0){
		perror("Cannot find the directory\n");
		exit(0);
	}
	prompt("d1");
	if(changeCurrentDirectory("d1") == -1){
		perror("Failed to change the directory");
		exit(0);
	}
	printf("You are now in directory d1\n");
	if(createFile("test4", 256) != 0){
		perror("Failed to create test4\n");
		exit(0);
	}
	prompt("test4");
	if(writeFile("test4", "This is Test 4") == 0){
		printf("Content loaded into test4\n");
	}
	fwrite(systemAddress, DISK_SIZE*sizeof(char), 1, disk);
	exit(0);
}
