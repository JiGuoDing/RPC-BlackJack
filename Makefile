# Makefile for BlackJack

CC = gcc
# CFLAGS = -g -fsanitize=address # 内存调试用
# LDFLAGS = -lrt -ltirpc # 在ubuntu22及以上使用
LDFLAGS = -lnsl # 在ubuntu20下使用
CLIENT_SRC = ./src/xdr/blackJack_clnt.c ./src/c/blackJackClient.c ./src/xdr/blackJack_xdr.c
SERVER_SRC = ./src/xdr/blackJack_svc.c ./src/xdr/blackJack_xdr.c
BIN_DIR = ./src/bin
CLIENT_BIN = $(BIN_DIR)/client.o
SERVER_BIN = $(BIN_DIR)/server.o

# 检查所需库是否存在
REQUIRED_LIBS = libnsl.so

all: check_libs $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC) | $(BIN_DIR) # 确保bin目录存在
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC) $(LDFLAGS)

$(SERVER_BIN): $(SERVER_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRC) $(LDFLAGS)

# Create bin directory if not exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# 检查所需的库
check_libs:
	@for lib in $(REQUIRED_LIBS); do \
		if ! ldconfig -p | grep -q $$lib; then \
			echo "缺少库: $$lib。请先安装所需的库再编译。"; \
			exit 1; \
		fi \
	done

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)

.PHONY: all clean
