# OSSP Viva Questions

1. **What is a system call?**  
A system call is the interface through which a user program requests services from the operating system kernel.

2. **Why is fork() used?**  
It creates a new child process so a client can be handled concurrently.

3. **What does fork() return?**  
Zero in the child, the child's PID in the parent, and -1 on failure.

4. **What is a process?**  
A running instance of a program.

5. **What is a zombie process?**  
A terminated child whose exit status has not yet been collected by its parent.

6. **Why use waitpid()?**  
To collect terminated child processes and prevent zombies.

7. **What is a socket?**  
A communication endpoint used for network communication.

8. **Why TCP?**  
TCP provides reliable, ordered byte-stream communication.

9. **What does bind() do?**  
It associates a socket with a local IP address and port.

10. **What does listen() do?**  
It makes a TCP socket wait for incoming connection requests.

11. **What does accept() do?**  
It accepts a pending client connection and returns a new connected socket.

12. **What does connect() do?**  
It requests a connection from the client to the server.

13. **What is a file descriptor?**  
A small integer used by Linux to identify an open file, socket, pipe, or similar resource.

14. **Why are sockets file descriptors?**  
Linux provides a common descriptor-based interface for many I/O resources.

15. **What is IPC?**  
Inter-Process Communication: mechanisms that let processes exchange data.

16. **Why is pipe() used?**  
It lets each child send structured events to the parent.

17. **Why can't children simply change the parent's client array?**  
After fork(), each process has its own memory copy.

18. **How does the server broadcast?**  
The parent receives a message event from a child and sends the message to the connected client sockets.

19. **How does private messaging work?**  
The parent searches its client table for the target username and sends the message to that socket.

20. **What is SIGCHLD?**  
A signal delivered to a parent when a child process terminates or stops.

21. **Why handle SIGINT?**  
To perform a clean shutdown when Ctrl+C is pressed.

22. **What happens when a client disconnects?**  
The child exits or reports a quit event, and the parent closes the resources and removes the client.

23. **What is concurrency?**  
Making progress on multiple tasks during overlapping time periods.

24. **Why use select() in the client?**  
It lets the client wait for either keyboard input or server messages.

25. **What is a TCP port?**  
A number used to identify a network service endpoint.

26. **Why check system-call return values?**  
Failures must be detected so the program can handle errors safely.

27. **Why use close()?**  
To release file descriptors and network resources.

28. **What is a parent process?**  
A process that creates another process, such as through fork().

29. **What is the main limitation?**  
It is intended for local/college demonstrations, not production-scale deployment.

30. **How could it be improved?**  
Authentication, encryption, persistent chat history, a GUI, better protocol framing, and scalable event-driven I/O could be added.
