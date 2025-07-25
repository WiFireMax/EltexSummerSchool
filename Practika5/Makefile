CC=gcc

OBJS=main.o abonent.o menu.o

all: task1

task1: $(OBJS)
	$(CC) -o task1 $(OBJS)

main.o: main.c abonent.h menu.h
	$(CC) -c main.c

abonent.o: abonent.c abonent.h
	$(CC) -c abonent.c

menu.o: menu.c menu.h
	$(CC) -c menu.c

clean:
	rm -f *.o task1
