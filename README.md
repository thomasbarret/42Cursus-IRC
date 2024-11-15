# 42 Cursus - IRC

**IRC** (Internet Relay Chat) is a project aimed at building a simple IRC server in C++ that complies with the RFC 2812 standard. This project will help you understand how real-time communication systems work by implementing a functional IRC server and client.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Installation and Usage](#installation-and-usage)
- [Technical Details](#technical-details)

---

## Overview

The goal of this project is to:

- Implement an IRC server supporting multiple clients.
- Handle basic IRC commands such as `JOIN`, `PART`, `PRIVMSG`, and `KICK`.
- Allow communication between clients using channels.
- Implement a bot that interacts with the server.

---

## Features

### Core Features
- **Client-Server Communication**: Supports multiple clients connected to the server.
- **Channel Management**: Clients can create and join channels, send messages, and manage users within the channels.
- **Command Parsing**: Implements key IRC commands based on the RFC 2812 specification.

### Bonus Features
- **Bot Integration**: A bot client interacts with the server, performing specific automated tasks.
- **Signal Handling**: Graceful handling of server shutdown with signals.
- **Utilities**: Includes tools for string parsing and command processing.

---

## Project Structure

The project is organized as follows:

```
.
├── Main.cpp                      # Entry point for the server application
├── Makefile                      # Build automation tool
├── bot                           # Bot implementation
│   ├── Main.cpp                  # Entry point for the bot application
│   ├── interface                 # Bot headers
│   │   ├── Class
│   │   │   └── Bot.hpp           # Bot class definition
│   │   └── IRCbot.hpp            # Bot interaction with the IRC protocol
│   └── src                       # Bot source files
│       └── Client
│           └── Bot.cpp           # Bot class implementation
├── interface                     # Headers for the core server and client
│   ├── Class
│   │   ├── Channel.hpp           # Channel class definition
│   │   ├── Client.hpp            # Client class definition
│   │   └── Server.hpp            # Server class definition
│   ├── Function
│   │   ├── Command.hpp           # Command parsing utilities
│   │   └── Utils.hpp             # Helper functions
│   └── IRC.hpp                   # Core IRC header file
└── src                           # Source code for server, client, and utilities
    ├── Client
    │   └── Client.cpp            # Client class implementation
    ├── Server
    │   ├── Channel.cpp           # Channel logic implementation
    │   ├── Command.cpp           # Command processing implementation
    │   ├── Mode.cpp              # IRC mode handling
    │   ├── Server.cpp            # Server class implementation
    │   └── Socket.cpp            # Socket handling
    └── Utility
        ├── Signal.cpp            # Signal handling
        └── String.cpp            # String utilities
```

---

## Installation and Usage

### Prerequisites
- **C++ Compiler**: Ensure you have a C++17-compatible compiler installed.
- **Make**: The project uses a Makefile for compilation.

### Build the Server and Bot
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/irc.git
   cd irc
   ```

2. Compile the project:
   ```bash
   make
   ```

3. Compile the bot (optional):
   ```bash
   make bot
   ```

### Run the Server
Start the IRC server:
```bash
./irc_server <port> <password>
```
- Replace `<port>` with the desired port number (e.g., `6667`).
- Replace `<password>` with the password for the server.

### Run the Bot
Run the bot client:
```bash
./irc_bot <server_host> <port> <nickname>
```
- Replace `<server_host>` with the server's address.
- Replace `<port>` with the port number of the server.
- Replace `<nickname>` with the bot's nickname.

### Clean the Project
To remove all compiled files:
```bash
make clean
```

---

## Technical Details

- **Protocol**: The project follows the IRC RFC 2812 specification for command handling.
- **Sockets**: Implements socket programming for client-server communication.
- **Object-Oriented Design**: Uses C++ classes to organize and encapsulate functionality.
- **Threading**: Manages multiple clients simultaneously using non-blocking I/O.