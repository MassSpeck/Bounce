CC=gcc
CFLAGS=-Wall -pedantic
PROG=bounce

all: $(PROG)

$(PROG): bounce.o
	$(CC) -o $@ $^

.PHONY: clean
clean:
	$(RM) *.o $(PROG)