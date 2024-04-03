CC = gcc
OBJ_DIR = ./object_files/

C_FLAGS = -Wall -Wextra -Wconversion -std=c99 -fdiagnostics-color=always -pedantic
DEBUG_FLAG = -g

LIB_DIR = -L ./lib

LINKERS = $(LIB_DIR) -lraylib /usr/lib/x86_64-linux-gnu/libGL.so -lm -lpthread -ldl -lrt

C_FILES = $(wildcard *.c)
OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)%.o, $(C_FILES))

build: main

main: $(OBJ_FILES)
	$(CC) $(DEBUG_FLAG) $(C_FLAGS) -o main $(OBJ_DIR)*.o $(LINKERS)

$(OBJ_FILES): $(OBJ_DIR)%.o: %.c
	$(CC) -c $(DEBUG_FLAG) $< -o $@

clean: 
	rm -f $(OBJ_DIR)*.o main libanim.a

run: 
	./main
