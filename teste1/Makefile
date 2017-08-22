CC = gcc
CFLAGS = -lm -Wall -g -pthread 

PROGRAMS = sequencial \
		multithread \
		processo

run: $(PROGRAMS)
	./sequencial 64
	./multithread 64 4
	./processo 64 4
