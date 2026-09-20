// server.c
// TCP server: accepts multiple clients, receives data, writes latest message into shared memory.
// Uses select() for concurrency and integrates with shared memory writer logic.
// Compile: gcc -o server server.c -pthread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <fcntl.h>

#define PORT 5000
#define MAX_CLIENTS  FD_SETSIZE
#define BUF_SIZE     256

#define SHM_KEY 0x1234
#define SHM_SIZE 1024

#define SEM_MUTEX_NAME "/shm_mutex"
#define SEM_RW_NAME    "/shm_rw"

typedef struct {
    char data[256];
    int has_data;
    int read_count;
} shm_t;

ssize_t read_all(int fd, char *buf, size_t maxlen) {
    // Simple line-based read: stop at '\n' or maxlen
    size_t total = 0;
    while (total < maxlen - 1) {
        char c;
        ssize_t n = recv(fd, &c, 1, 0);
        if (n == 0) break;      // connection closed
        if (n < 0) return -1;   // error
        buf[total++] = c;
        if (c == '\n') break;
    }
    buf[total] = '\0';
    return (ssize_t)total;
}

int main(void) {
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shm_t *shm = (shm_t *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    shm->has_data = 0;
    shm->read_count = 0;

    sem_t *mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open mutex");
        exit(EXIT_FAILURE);
    }

    sem_t *rw_mutex = sem_open(SEM_RW_NAME, O_CREAT, 0666, 1);
    if (rw_mutex == SEM_FAILED) {
        perror("sem_open rw_mutex");
        exit(EXIT_FAILURE);
    }

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 16) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);
    int max_fd = listen_fd;

    while (1) {
        read_set = master_set;

        struct timeval tv;
        tv.tv_sec = 5;   // timeout for select
        tv.tv_usec = 0;

        int ready = select(max_fd + 1, &read_set, NULL, NULL, &tv);
        if (ready < 0) {
            perror("select");
            break;
        } else if (ready == 0) {
            // Timeout: could do housekeeping here
            continue;
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            if (!FD_ISSET(fd, &read_set)) continue;

            if (fd == listen_fd) {
                // New connection
                struct sockaddr_in cli_addr;
                socklen_t cli_len = sizeof(cli_addr);
                int cli_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
                if (cli_fd < 0) {
                    perror("accept");
                    continue;
                }
                FD_SET(cli_fd, &master_set);
                if (cli_fd > max_fd) max_fd = cli_fd;
                printf("New client connected: fd=%d\n", cli_fd);
            } else {
                // Existing client: read data
                char buf[BUF_SIZE];
                ssize_t n = read_all(fd, buf, sizeof(buf));
                if (n <= 0) {
                    printf("Client fd=%d disconnected\n", fd);
                    close(fd);
                    FD_CLR(fd, &master_set);
                    continue;
                }

                printf("Server received from fd=%d: %s", fd, buf);

                // Writer: lock rw_mutex, update shared memory
                if (sem_wait(rw_mutex) == -1) {
                    perror("sem_wait rw_mutex");
                    continue;
                }

                strncpy(shm->data, buf, sizeof(shm->data) - 1);
                shm->data[sizeof(shm->data) - 1] = '\0';
                shm->has_data = 1;

                if (sem_post(rw_mutex) == -1) {
                    perror("sem_post rw_mutex");
                }
            }
        }
    }

    // Cleanup
    close(listen_fd);
    if (shmdt(shm) == -1) {
        perror("shmdt");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
    }

    sem_close(mutex);
    sem_close(rw_mutex);
    sem_unlink(SEM_MUTEX_NAME);
    sem_unlink(SEM_RW_NAME);

    return 0;
}
