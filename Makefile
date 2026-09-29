CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -D_POSIX_C_SOURCE=200809L
SERVER = bin/server
CLIENT = bin/client

.PHONY: all server client clean

all: server client

server: $(SERVER)

client: $(CLIENT)

$(SERVER): src/server/server.c src/common/protocol.h
	mkdir -p bin
	$(CC) $(CFLAGS) src/server/server.c -o $(SERVER)

$(CLIENT): src/client/client.c src/common/protocol.h
	mkdir -p bin
	$(CC) $(CFLAGS) src/client/client.c -o $(CLIENT)

clean:
	rm -rf bin
