In this assignment I have worked with, Weiyi Zhang, Oscar Wu, and Ye Yuan to create this log structured file system.
This file system is able to create, wirte and read files and create subdirectory. I was not able to finish the rest part due to the final exam period.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
Disk Structure
The first block of the disk is the super block, it stored information of the disk
The second block is the free block vector, all the avaliable block in the disk will be marked as '1'
The second block stores the mapping of inode
The third block is the root directory of the disk
Other subdirectory will be stored in the following blocks
All the data will be stored after block 10
-------------------------------------------------------------------------------------------------------------------------------------------------------------
How to run it?
After unzip the file, type the following command in the terminal
chmod +x ./script //this will compile the script
./script //this will run the script
The first test case is used for testing file/directory creation function and creating disk
The second test case is used to test mounting the previous vdisk
-------------------------------------------------------------------------------------------------------------------------------------------------------------
Thougts
When I worked on this assignment, I tried to keep it simple. I assume that there will be enough space for files, and all the data will be stack on the blocks.
However, if a file that takes two blocks was is deleted, and I try to store a three block file, there will be some problems. However, since I did not have
enough time to finish delete, this problem is avoided now at this moment.