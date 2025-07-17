CC=gcc
SRC4=task1.c task2.c task3.c task4.c
EXE4=task1 task2 task3 task4

# Параметр t[N] - компиляция задач в отдельном задании N
# Параметр d[N] - удаление исполняемых файлов задач в отдельном задании N
.PHONY: all t% d%

PRACTIKA=Practika$*

# Компиляция
t%:
	@mkdir -p $(PRACTIKA)
	@for src in $(SRC4); do \
		exe=$${src%.c}; \
		echo "$(CC) $$src -o $$exe"; \
		$(CC) $(PRACTIKA)/$$src -o $(PRACTIKA)/$$exe; \
	done

# Удаление
d%:
	@for exe in $(EXE4); do \
		rm -f $(PRACTIKA)/$$exe; \
	done
	@echo "Удаление исполняемых файлов прошло успешно"
