

.SUFFIXES:
.SUFFIXES: .c .o

CC = gcc
CFLAGS = -Wall -W -pipe -O3 -Wwrite-strings -Wsign-compare -Wshadow -Wformat=2
LIBS = -lm

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

all: cfga.o
	$(CC) $(CFLAGS) -o cfga cfga.o

clean:
	-rm -f *.o *~ core cfga
