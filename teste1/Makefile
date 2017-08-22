CC = gcc
CFLAGS = -Wall -pthread -lm

PROGRAMS = sequencial \
		multithread \
		processo

run: $(PROGRAMS)
	./sequencial 64
	./multithread 64 4
	./processo 64 4

processo: processo.c
	$(CC) $(CFLAGS) processo.c -o processo -lrt
