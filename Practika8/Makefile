CC = gcc

SRC_LIB = calc_operations.c input_values.c
OBJ_LIB = calc_operations.o input_values.o
LIB = libfiremaxlib.so

SRC_MAIN = task1.c
OBJ_MAIN = task1.o

TARGET = calculator

.PHONY: all clean

all: $(TARGET)

$(OBJ_LIB): %.o : %.c calculator.h
	$(CC) -fPIC -I. -c $< -o $@

$(LIB): $(OBJ_LIB)
	$(CC) -shared -o $@ $^

$(OBJ_MAIN): $(SRC_MAIN) calculator.h
	$(CC) -fPIC -I. -c $< -o $@

$(TARGET): $(OBJ_MAIN) $(LIB)
	$(CC) -o $@ $(OBJ_MAIN) -L. -lfiremaxlib

clean:
	rm -f $(OBJ_LIB) $(OBJ_MAIN) $(LIB) $(TARGET)
