CC = gcc
CFLAGS = -std=c11 -g -pthread
TARGET = q5
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
RM = rm -f

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJS) $(TARGET)
