# Project Report: Multi-User Chat Application

## 1. Introduction
The objective of this Open Source Software Project (OSSP) is to design and develop a robust TCP multi-user chat application in C for Ubuntu/WSL. The project serves to demonstrate the practical application of core operating system concepts.

## 2. System Architecture
The application uses a multi-process architecture:
- **Server:** Acts as the central hub. It maintains a client table and handles incoming connections.
- **Child Processes:** The server uses `fork()` to spawn a new process for each connected client, ensuring that the main server thread is never blocked.
- **Inter-Process Communication (IPC):** Child processes communicate with the parent server using Linux `pipe()` to route messages.

## 3. System Calls Utilized
- `socket()`, `bind()`, `listen()`, `accept()`, `connect()`: For TCP network communication.
- `fork()`: For creating client-handler processes.
- `pipe()`: For IPC between child and parent processes.
- `read()`, `write()`, `recv()`, `send()`: For data transmission.
- `signal()`: For handling zombie processes and graceful shutdowns.

## 4. Implementation Details
The codebase is divided into `client`, `server`, and `common` components located in the `src/` directory. The Makefile automates the build process, generating the executables in the `bin/` folder.

## 5. Conclusion
This project successfully demonstrates the integration of network programming and operating system fundamentals. The resulting chat application is capable of handling multiple concurrent users with both broadcast and private messaging features.
