# System Calls Used

| Call/API | Purpose | Project use |
|---|---|---|
| `socket()` | Creates a communication endpoint | Server and client TCP sockets |
| `bind()` | Assigns address/port to socket | Server |
| `listen()` | Puts server socket into listening mode | Server |
| `accept()` | Accepts a client connection | Server |
| `connect()` | Connects client to server | Client |
| `fork()` | Creates a child process | One child per client |
| `pipe()` | Creates a unidirectional IPC channel | Child sends events to parent |
| `read()` | Reads bytes from a descriptor | Server reads pipe events |
| `write()` | Writes bytes to a descriptor | Child sends events through pipe |
| `send()` | Sends data through a socket | Both sides |
| `recv()` | Receives data through a socket | Both sides |
| `close()` | Releases a descriptor | Cleanup |
| `waitpid()` | Waits for child processes | Zombie cleanup/shutdown |
| `sigaction()` | Installs signal handlers | SIGINT/SIGCHLD |
| `kill()` | Sends a signal to a process | Server shutdown |
| `getpid()` | Gets current process ID | Server demonstration/logging |
| `select()` | Waits for input on multiple descriptors | Client handles keyboard + socket |

## Why fork()?

When a client connects, the server calls `fork()`. The child handles that client's socket while the parent continues accepting new clients.

## Why pipe()?

After `fork()`, ordinary variables are separate copies. Therefore a child cannot update the parent's normal client list directly. The child sends structured events to the parent through a pipe. The parent owns the client list and performs broadcasts/private-message routing.

## Zombie processes

When a child terminates, the parent receives `SIGCHLD`. The handler calls `waitpid(..., WNOHANG)` to reap finished children.

## File descriptors

Linux represents sockets and pipes using file descriptors. This is why calls such as `read()`, `write()`, and `close()` can operate on them.
