# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I./src
LDFLAGS =

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
DOCDIR = docs

# Source files
SERVER_SRC = $(SRCDIR)/server.c $(SRCDIR)/client_interface.c $(SRCDIR)/term_utils.c
CLIENT_SRC = $(SRCDIR)/client.c $(SRCDIR)/client_interface.c $(SRCDIR)/term_utils.c

# Object files
SERVER_OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CLIENT_SRC))

# Executables
SERVER_EXEC = $(BINDIR)/server
CLIENT_EXEC = $(BINDIR)/client

# Default target
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Rule to link the server executable
$(SERVER_EXEC): $(SERVER_OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to link the client executable
$(CLIENT_EXEC): $(CLIENT_OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to generate documentation (assuming docs.md needs to be processed in some way)
docs: $(DOCDIR)/docs.md
	cp $(DOCDIR)/docs.md $(DOCDIR)/docs.html # Placeholder for real doc processing

# Rule to clean the build artifacts
clean:
	rm -f $(OBJDIR)/*.o $(SERVER_EXEC) $(CLIENT_EXEC)
	rm -f $(DOCDIR)/docs.html # Clean up documentation output if necessary

# Phony targets
.PHONY: all clean docs

