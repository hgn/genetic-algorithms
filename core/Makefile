
OBJ= cfga.o mutation.o rbtree.o crossover.o population.o fitness.o

.SUFFIXES:
.SUFFIXES: .c .o

CC = gcc
CFLAGS = -Wall -W -pipe -O3 -Wwrite-strings -Wsign-compare -Wshadow -Wformat=2
LIBS = -lm

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

all: $(OBJ)
	$(CC) $(CFLAGS) -o cfga $(OBJ)

clean:
	-rm -f *.o *~ core cfga
