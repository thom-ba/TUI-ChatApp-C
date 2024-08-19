# Simple Chat Application in C

## Overview

This project is a basic client-server chat application implemented in C. It allows multiple clients to connect to a server and exchange messages in real-time. The application uses TCP/IP for network communication and supports multi-threading for handling concurrent message reception and user input.


## Requirements

- **Compiler:** GCC or any compatible C compiler.
- **Libraries:** Standard C libraries (`<pthread.h>`, `<netinet/in.h>`, `<poll.h>`, `<sys/socket.h>`, etc.)

## Setup and Building

### Cloning the Repository

```bash
git clone https://github.com/yourusername/simple-chat-app.git
cd simple-chat-app
```

### Building the Project

Use the provided `Makefile` to build the project. This will compile the source code and place the executables in the `bin/` directory.

```bash
make
```

### Cleaning Up

To clean the project by removing the compiled binaries and object files, use:

```bash
make clean
```

## Running the Application

### Starting the Server

Run the server program first to start listening for incoming client connections.

```bash
./bin/server
```

### Starting the Client

After starting the server, you can run multiple instances of the client program to connect to the server.

```bash
./bin/client
```

The client will prompt you to enter your username. After entering your username, you can start sending and receiving messages.

## Usage

- **Server:** Listens for incoming client connections, handles new connections, and broadcasts messages to all connected clients.
- **Client:** Connects to the server, allows the user to send and receive messages, and provides a basic terminal-based user interface.


## License

This project is dedicated to the public domain. You are free to use, modify, and distribute this software as you wish. For more details, see the [UNLICENSE](http://unilicense.org) file.

## Author

Thomas Baumeister

