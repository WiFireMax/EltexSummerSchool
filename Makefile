CC=gcc

# Компиляция объектных файлов
$(PRACTIKA)/%.o: $(PRACTIKA)/%.c
	$(CC) -c $< -o $@

# Исполняемые файлы из объектных
$(PRACTIKA)/%: $(PRACTIKA)/%.o
	$(CC) $^ -o $@

# Параметр t[N] - компиляция отдельного задания N
# Параметр d[N] - Удаление исполняемых файлов задания N
.PHONY: t5 d5 t6 d6 t7 d7 t8 d8

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
	@rm -f $(OBJ) $(EXE)

