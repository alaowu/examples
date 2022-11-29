CC = gcc

SRC = main_one.c
OBJ = main_one.o

INC = -I../kernel
LIBS = 

TARGET = main_one

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(INC) $(LIBS)

$(OBJ): $(SRC)
	$(CC) -c $< $(INC) -o $@
