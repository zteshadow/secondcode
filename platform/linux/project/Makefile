
AR = ar
#CC =
CC = gcc
TARGET = platform.a
CFLAGS = -Wall -O -g
SRC_DIR = ..
INC_DIR = ../../include
INCPATH = -I$(INC_DIR)
DEPINC =

%.o : $(SRC_DIR)/%.c $(INC_DIR)/%.h $(DEPINC)
	$(CC) $(CFLAGS) $(INCPATH) -c $< -o $@

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst %.c, %.o, $(notdir $(SRC)))

$(TARGET) : $(OBJ)
	$(AR) -r $(TARGET) $(OBJ)

clean:
	rm -rf  $(TARGET) $(OBJ)
