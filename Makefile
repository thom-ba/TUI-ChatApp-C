# Variables
CC = gcc
CCFLAGS = -Wall -g
LDFLAGS = 
SOURCES = client.c server.c main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = ca

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
