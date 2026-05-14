CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: server client

server: minor4svr.c
	$(CC) $(CFLAGS) -o server minor4svr.c

client: minor4cli.c
	$(CC) $(CFLAGS) -o client minor4cli.c

clean:
	rm -f server client
