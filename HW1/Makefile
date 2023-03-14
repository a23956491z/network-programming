CC = gcc
CFLAGS = -g -MD
# CFLAGS = -lreadline
INC = -Iinclude

SRC_DIR = src
OBJ_DIR = object
OBJS = $(OBJ_DIR)/shell.o $(OBJ_DIR)/container.o


all: $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# shell: ./src/shell.c
# 	gcc $(CFLAGS) $(INC) ./src/shell.c -o shell 


