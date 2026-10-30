INCLUDE=/home/ensiie/TD2/hp_allocator/hp_allocator_lib/include
LIB=/home/Documents/Cours/ENSIIE/S3/ARSE/ASE-TD2-ETUDIANT/ETUDIANT/hp_allocator/hp_allocator_lib/lib
CFLAGS = -Wall -O3 -I$(INCLUDE)
CFLAGSO0 = -Wall -O0
LDFLAGS = -lpthread
CC = gcc
RM = rm -f
MAKEDEPEND = makedepend

INC = $(wildcard *.h)
SRC = $(wildcard *.c)

all: bench_hp_malloc

bench_hp_malloc: main.o bench.o
	$(CC) $(LDFLAGS) $(CFLAGS)  $^ -o $@  -L$(LIB) -lhp_allocator

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) *.o Makefile.bak
	$(RM) bench_hp_malloc

depend: $(INC)
	$(MAKEDEPEND) -Y./ -- $(CFLAGS) -- $(SRC) 2>/dev/null

# DO NOT DELETE

bench.o: bench.h
main.o: bench.h
