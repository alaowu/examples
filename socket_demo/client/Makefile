CURRENT_PATH = $(shell pwd)
CC  = gcc
CPP = g++
AR  = gcc-ar

OUT = $(CURRENT_PATH)/out

COM_FLAGS = -Wall -O2 -fPIC -m64 -g
C_FLAGS   = $(COM_FLAGS) -std=c11
CPP_FLAGS = $(COM_FLAGS) -std=c++11 

INCLUDES = -I. -I../include
LIB_PATH = -L.

SOURCE_PATH = $(CURRENT_PATH)

TARGET_NAME  = SocketClient

C_SRCS    = $(wildcard $(SOURCE_PATH)/*.c)
C_OBJS    = $(patsubst %.c, %.o, $(C_SRCS))

#CPP_SRCS  = $(wildcard $(CURRENT_PATH)/*.cpp)
#CPP_OBJS  = $(patsubst %.cpp, %.o, $(CPP_SRCS))

LIB_NAME  = -pthread -lm -ldl

.PHONY: all prepare clean

all: prepare $(TARGET_NAME) finish

prepare:
	@echo
	@echo ">>>>========================================================"
	@echo "TARGET_NAME = $(TARGET_NAME)"
	@mkdir -p $(OUT)

clean:
	@rm -rf $(C_OBJS)
	@rm -rf $(TARGET_NAME)
	@rm -rf $(OUT)

finish:
	@mv $(SOURCE_PATH)/*.o $(OUT)
	@echo "make done"
	@echo "<<<<========================================================"
	@echo

$(TARGET_NAME): $(C_OBJS) $(C_SRCS)
	@echo "generate $@"
	@$(CC) -o $@ $(C_OBJS) $(LIB_PATH) $(LIB_NAME)

%.o : %.c
	@echo "compile $@"
	@$(CC) $(C_FLAGS) $(INCLUDES) -c $< -o $@

