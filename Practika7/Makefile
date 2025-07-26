CC = gcc

SRC_LIB = calc_operations.c input_values.c
OBJ_LIB = calc_operations.o input_values.o
LIB = libfiremaxlib.a

SRC_MAIN = task1.c
OBJ_MAIN = task1.o

TARGET = calculator

.PHONY: all clean

all: $(TARGET)

$(OBJ_LIB): %.o : %.c calculator.h
	$(CC) -I. -c $< -o $@

$(LIB): $(OBJ_LIB)
	ar rcs $@ $^

$(OBJ_MAIN): $(SRC_MAIN) calculator.h
	$(CC) -I. -c $< -o $@

$(TARGET): $(OBJ_MAIN) $(LIB)
	$(CC) -I. $^ -o $@

clean:
	rm -f $(OBJ_LIB) $(OBJ_MAIN) $(LIB) $(TARGET)
