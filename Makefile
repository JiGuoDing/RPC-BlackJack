# Makefile for BlackJack

CC = gcc
# CFLAGS = -g -fsanitize=address
LDFLAGS = -lnsl
CLIENT_SRC = ./src/xdr/blackJack_clnt.c ./src/c/blackJackClient.c ./src/xdr/blackJack_xdr.c
SERVER_SRC = ./src/xdr/blackJack_svc.c ./src/xdr/blackJack_xdr.c
BIN_DIR = ./src/bin
CLIENT_BIN = $(BIN_DIR)/client.o
SERVER_BIN = $(BIN_DIR)/server.o

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC) $(LDFLAGS)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRC) $(LDFLAGS)

# Create bin directory if not exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)

.PHONY: all clean
