CC=gcc

# Компиляция объектных файлов
$(PRACTIKA)/%.o: $(PRACTIKA)/%.c
	$(CC) -c $< -o $@

# Исполняемые файлы из объектных
$(PRACTIKA)/%: $(PRACTIKA)/%.o
	$(CC) $^ -o $@

# Параметр t[N] - компиляция отдельного задания N
# Параметр d[N] - Удаление исполняемых файлов задания N
.PHONY: t5 d5 t6 d6 t7 d7 t8 d8 d9 t11 d11

t5:
	$(MAKE) -C Practika5

d5:
	$(MAKE) -C Practika5 clean

t6:
	$(MAKE) -C Practika6

d6:
	$(MAKE) -C Practika6 clean

t7:
	$(MAKE) -C Practika7

d7:
	$(MAKE) -C Practika7 clean

t8:
	$(MAKE) -C Practika8

d8:
	$(MAKE) -C Practika8 clean
t9:
	$(CC) Practika9/task1.c -o Practika9/task1
	$(CC) -o Practika9/file_manager Practika9/task2.c -lncurses
d9:
	rm -f Practika9/output.txt Practika9/file_manager
	$(MAKE) NUM=9 clean

t11:
	$(CC) -pthread -o Practika11/task1 Practika11/task1.c
	$(CC) -pthread -o Practika11/task2 Practika11/task2.c
	$(CC) -pthread -o Practika11/task3 Practika11/task3.c
d11:
	rm -f Practika11/life.log
	$(MAKE) NUM=11 clean

.PHONY: t% d%

t%:
	$(MAKE) NUM=$* all

d%:
	$(MAKE) NUM=$* clean

.PHONY: all clean

all:
	$(eval PRACTIKA=Practika$(NUM))
	$(eval SRC=$(wildcard $(PRACTIKA)/*.c))
	$(eval OBJ=$(SRC:.c=.o))
	$(eval EXE=$(SRC:.c=))
	@$(MAKE) -C $(PRACTIKA) $(notdir $(EXE))

clean:
	$(eval PRACTIKA=Practika$(NUM))
	$(eval SRC=$(wildcard $(PRACTIKA)/*.c))
	$(eval OBJ=$(SRC:.c=.o))
	$(eval EXE=$(SRC:.c=))
	@echo "Очистка в $(PRACTIKA)"
	rm -f $(OBJ) $(EXE)
