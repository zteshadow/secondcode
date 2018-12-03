
AR = ar
#CC =
CC = gcc
TARGET = algorithm.a
CFLAGS = -Wall -O -g
SRC_DIR = ../../source
INC_DIR = ../../include
DEP_DIR = ../../../platform/include
INCPATH = -I$(INC_DIR) -I$(DEP_DIR)
DEPINC =

%.o : $(SRC_DIR)/%.c $(DEPINC)
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@

SRC = $(wildcard $(SRC_DIR)/*.c)
#SRC = $(SRC_DIR)/cm_binary_tree.c $(SRC_DIR)/cm_list_hash.c
OBJ = $(patsubst %.c, %.o, $(notdir $(SRC)))

$(TARGET) : $(OBJ)
	$(AR) -r $(TARGET) $(OBJ)

clean:
	rm -rf  $(TARGET) $(OBJ)
