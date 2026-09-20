# Assignment: Hybrid IPC & Networking Utility

## Introduction

In this assignment, you will apply your knowledge of Inter-Process Communication (IPC) and network programming to build a multi-component system that demonstrates efficient data sharing and communication. You will work with shared memory for high speed IPC, implement synchronization using semaphores, and extend the system to include network communication via TCP sockets. This assignment will test your ability to integrate these concepts into a cohesive solution while ensuring robustness and correctness.

### Starter Files

There is no starter code for this assignment.

### Requirements

#### Part 1: Shared Memory and Semaphores

1. **Shared Memory Setup**:
   
   - Use `shmget()` to create a shared memory segment.
   - Implement a **producer-consumer** model where one process writes data to the shared memory, and another reads it.
   - Ensure proper cleanup using `shmctl()` and `shmdt()`.

2. **Synchronization**:
   
   - Use semaphores to synchronize access to the shared memory segment, preventing race conditions.
   - Implement a **reader-writer** pattern where multiple readers can access the data simultaneously, but writers have exclusive access.

#### Part 2: Network Communication

1. **TCP Socket Programming**:
   
   - Develop a **client-server application** where the server listens for incoming connections and the client sends data.
   - Handle partial reads/writes gracefully and implement timeouts for socket operations.
   - Use `select()` or threads to manage multiple clients concurrently.

2. **Integration**:
   
   - Extend the shared memory system to include a network component. For example, the server could write data received from clients into shared memory, and another process could read and display it.

## Deliverables

- A well-commented C program for the shared memory producer-consumer system with semaphores.

- A separate C program for the TCP client-server application.

- An integrated version combining both components (optional for bonus points).

## Expected source code files:

- `shm_producer.c` (Writes to shared memory)

- `shm_consumer.c` (Reads from shared memory)

- `server.c` (TCP server that updates shared memory)

- `client.c` (Sends data to the server)

----------

## Conclusion

This assignment challenges you to combine IPC and network programming techniques to build a functional and efficient system. By completing it, you will gain hands-on experience with shared memory, semaphores, and TCP sockets, all of which are critical skills for systems programming. Pay close attention to synchronization and error handling to ensure your solution is robust and scalable. Submit your work by the deadline and be prepared to discuss your design choices during the evaluation. Good luck!