# Test Results

This document contains the execution results and observations from testing the Multi-User Chat Application.

## 1. Server Initialization
- **Action:** Start server on port 8080.
- **Expected Result:** Server starts listening on port 8080 and waits for incoming connections.
- **Actual Result:** Passed.

## 2. Client Connection
- **Action:** Connect three concurrent clients (e.g., Bharat, Rahul, Anil) to `127.0.0.1:8080`.
- **Expected Result:** Server accepts connections, forks child processes for each client, and logs their connection.
- **Actual Result:** Passed.

## 3. Message Broadcasting
- **Action:** A client sends a message using `/broadcast Hello Everyone`.
- **Expected Result:** The message is routed through the server's pipe and broadcasted to all connected clients.
- **Actual Result:** Passed.

## 4. Direct Messaging
- **Action:** A client sends a private message using `/msg Rahul Hi Rahul!`.
- **Expected Result:** Only Rahul receives the message; other clients do not see it.
- **Actual Result:** Passed.

## 5. Client Disconnection
- **Action:** A client sends the `/quit` command.
- **Expected Result:** The server gracefully closes the connection, cleans up the child process, and notifies other users.
- **Actual Result:** Passed.
