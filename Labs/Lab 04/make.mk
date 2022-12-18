#make file - this is a comment section
 
CC=gcc  #compiler
TARGET=main #target file name
 
all:
    $(CC) main.c alloc.c mult.c print.c read.c -o $(TARGET)
 
clean:
    rm $(TARGET)