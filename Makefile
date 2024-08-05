# Variables
CC = gcc
CCFLAGS = -Wall -g
LDFLAGS = 
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source and Object files
CLIENT_SRC = $(SRC_DIR)/client.c
SERVER_SRC = $(SRC_DIR)/server.c
CLIENT_OBJ = $(OBJ_DIR)/client.o
SERVER_OBJ = $(OBJ_DIR)/server.o
CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server

# Targets
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Link the client executable
$(CLIENT_EXEC): $(CLIENT_OBJ) $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $(CLIENT_OBJ) $(MAIN_OBJ)

# Link the server executable
$(SERVER_EXEC): $(SERVER_OBJ) $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $(SERVER_OBJ) $(MAIN_OBJ)

# Compile client source file to object file
$(OBJ_DIR)/client.o: $(CLIENT_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c -o $@ $<

# Compile server source file to object file
$(OBJ_DIR)/server.o: $(SERVER_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c -o $@ $<

# Compile main source file to object file
$(OBJ_DIR)/main.o: $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CCFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean

