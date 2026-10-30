CFLAGS = -Wall -O3
CFLAGSO0 = -Wall -O0 -lpthread
LDFLAGS = -lpthread
CC = gcc
RM = rm -f
MAKEDEPEND = makedepend

INC = $(wildcard *.h)
SRC = $(wildcard *.c)

all: bench_malloc bench_huge

bench_malloc: main.o bench.o
	$(CC) $(LDFLAGS) -o $@ main.o bench.o

bench_huge: main_huge.o bench.o
	$(CC) $(LDFLAGS) -o $@ main_huge.o bench.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) *.o Makefile.bak
	$(RM) bench_malloc bench_huge
	$(RM) bench_hp_malloc

depend: $(INC)
	$(MAKEDEPEND) -Y./ -- $(CFLAGS) -- $(SRC) 2>/dev/null

# DO NOT DELETE

bench.o: bench.h
main.o: bench.h
main_huge.o: bench.h
