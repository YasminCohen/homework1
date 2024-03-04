CC = gcc
CFLAGS = -std=c11 -g
RM = rm -f

.PHONY: all default clean

all: q5

default: all

q5: q5.o
	$(CC) $(CFLAGS) -pthread -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	$(RM) *.o *.so q5