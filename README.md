# Multi-User Chat Application Using Linux System Calls

## Team Members
- [Insert Team Member 1] - [ID 1]
- [Insert Team Member 2] - [ID 2]
- [Insert Team Member 3] - [ID 3]

## Supervisor
- [Insert Supervisor's Name]

## Abstract
A college-level Open Source Software Project (OSSP) implementing a robust TCP multi-user chat application in C on Ubuntu/WSL. The project demonstrates core operating system concepts including Linux system calls, process management with `fork()`, inter-process communication using pipes, and TCP socket programming. It supports multiple simultaneous clients with broadcasting and direct messaging capabilities.

## Current Phase Status
- **Phase**: [e.g., Development / Testing / Completed]

## Objectives

- Demonstrate Linux system calls.
- Demonstrate process creation using `fork()`.
- Demonstrate IPC using `pipe()`.
- Demonstrate TCP socket programming.
- Demonstrate signal handling and child cleanup.
- Support multiple simultaneous clients.

## Requirements

Ubuntu/WSL2 with:

- GCC
- Make

Install:

```bash
sudo apt update
sudo apt install build-essential
```

## Build

```bash
cd multi_user_chat
make
```

Or:

```bash
bash scripts/build.sh
```

## Run server

Terminal 1:

```bash
./bin/server 8080
```

## Run clients

Open three additional Ubuntu terminals:

```bash
./bin/client 127.0.0.1 8080
```

Use different usernames, for example:

- Bharat
- Rahul
- Anil

## Commands

```text
/help
/users
/msg <user> <message>
/broadcast <message>
/quit
```

Any normal text message is broadcast to all connected users.

## Demonstration

1. Start the server.
2. Connect three clients.
3. Give each client a different username.
4. Send a normal message.
5. Use `/users`.
6. Use `/msg Rahul hello`.
7. Use `/broadcast hello everyone`.
8. Disconnect one client with `/quit`.
9. Press Ctrl+C in the server terminal.

## Architecture

The parent server process owns the client table. Every client is handled by a child process created using `fork()`. Each child sends events to the parent using a pipe. The parent performs routing and broadcasting.

See `docs/ARCHITECTURE.md`.

## System calls

See `docs/SYSTEM_CALLS.md`.

## Viva

See `docs/VIVA.md`.

## Project structure

```text
multi_user_chat/
├── src/
│   ├── client/
│   │   └── client.c
│   ├── common/
│   │   └── protocol.h
│   └── server/
│       └── server.c
├── data/
├── results/
├── reports/
├── scripts/
│   ├── build.sh
│   └── run_demo.sh
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SYSTEM_CALLS.md
│   └── VIVA.md
├── Makefile
└── README.md
```

## Note

This is an educational project. It is deliberately focused on operating-system and system-programming concepts rather than production deployment.
