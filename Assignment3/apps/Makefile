CC = gcc
SRC = ./../io/File.c
CFALGS = -Wall -g 

.PHONY: all
all:
	 $(CC) $(CFLAGS) -I./../io/ $(SRC) test1.c -o test1
	 $(CC) $(CFLAGS) -I./../io/ $(SRC) test2.c -o test2
.PHONY: clean 
clean:
	rm -rf test1 test2

