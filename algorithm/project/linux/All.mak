
HOME = ../../..
CC = gcc
TARGET = test
CFLAGS = -Wall -O -g
INC_DIR = ../../include
INCPATH = -I$(INC_DIR) -I$(HOME)/platform/include
DEPINC = ../../include/cm_hash.h
STATIC_LIB = algorithm.a $(HOME)/platform/linux/project/platform.a $(HOME)/utility/project/linux/utility.a

SRC = main.c
OBJ = main.o

$(TARGET) : $(OBJ) $(STATIC_LIB)
	$(CC) -o $(TARGET) $(OBJ) $(STATIC_LIB)

$(OBJ) : $(SRC) $(DEPINC)
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@

$(STATIC_LIB):
	make -f Makefile
	make -C $(HOME)/utility/project/linux
	make -C $(HOME)/platform/linux/project

clean:
	rm -rf  $(TARGET) $(OBJ)
	make -C $(HOME)/utility/project/linux clean
	make -C $(HOME)/platform/linux/project clean
