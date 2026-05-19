CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: aethel8

aethel8: src/main.c src/aethel8.h
	$(CC) $(CFLAGS) src/main.c -o aethel8

clean:
	rm -f aethel8