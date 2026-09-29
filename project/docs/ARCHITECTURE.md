# Architecture

The server uses a parent process plus one child process per connected client.

```text
                    SERVER PARENT
                         |
               accept() new connection
                         |
                       fork()
                    /          \
             Parent              Child
               |                   |
      stores client list      handles client
               |                   |
               |<---- pipe -------|
               |
       routes messages
       broadcasts
       private messages
```

## Important design point

A forked child gets a copy of the parent's memory, not shared normal memory. Therefore the child does not directly modify the parent's `clients[]` array.

Instead:

1. Parent accepts a client.
2. Parent creates a pipe.
3. Parent calls `fork()`.
4. Child receives the socket and pipe write end.
5. Child handles network input.
6. Child sends a `server_event_t` to the parent through the pipe.
7. Parent updates its client table and sends messages using the client sockets it owns.

This is a practical example of **process management + IPC + sockets**.
