OS = lin
LIB_DIR = -L ./lib

ifeq ($(OS), lin)
CC = gcc
LIB_NAME = libtilemapSF.a
LINKERS = $(LIB_DIR) -lraylib  /usr/lib/x86_64-linux-gnu/libGL.so -lm -lpthread -ldl -lrt
TARGET = demo
else
CC = x86_64-w64-mingw32-gcc
LIB_NAME = libtilemapSFwin.a
LINKERS = $(LIB_DIR) -lraylibwin -lraylibdll -lopengl32 -lgdi32 -lwinmm
TARGET = demo.exe
endif

OBJ_DIR = ./object_files/

C_FLAGS = -Wall -Wextra -Wconversion -std=c99 -fdiagnostics-color=always -pedantic
DEBUG_FLAG = -g
# -g

C_FILES = $(wildcard *.c)
OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)%.o, $(C_FILES))

build: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(DEBUG_FLAG) $(C_FLAGS) -o demo $(OBJ_DIR)*.o $(LINKERS)

lib: $(OBJ_FILES)
	ar -rc $(LIB_NAME) $(OBJ_DIR)tilemapSF.o $(OBJ_DIR)loadMapData.o $(OBJ_DIR)cJSON.o

$(OBJ_FILES): $(OBJ_DIR)%.o: %.c
	$(CC) -c $(DEBUG_FLAG) $< -o $@

clean: 
	rm -f $(OBJ_DIR)*.o demo demo.exe *.a

run: 
	./demo
