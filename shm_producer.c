// shm_producer.c
// Producer: writes data into shared memory using reader-writer semaphores.
// Compile: gcc -o shm_producer shm_producer.c -pthread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_KEY 0x1234
#define SHM_SIZE 1024

#define SEM_MUTEX_NAME "/shm_mutex"
#define SEM_RW_NAME    "/shm_rw"

typedef struct {
    char data[256];
    int has_data;
    int read_count;
} shm_t;

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

    // Initialize shared memory
    shm->has_data = 0;
    shm->read_count = 0;

    // Open/create POSIX named semaphores
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

    printf("Producer: enter lines to write to shared memory (Ctrl+D to quit).\n");

    char buf[256];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        // Writer: acquire rw_mutex for exclusive access
        if (sem_wait(rw_mutex) == -1) {
            perror("sem_wait rw_mutex");
            break;
        }

        // Critical section: write data
        strncpy(shm->data, buf, sizeof(shm->data) - 1);
        shm->data[sizeof(shm->data) - 1] = '\0';
        shm->has_data = 1;

        printf("Producer wrote: %s", shm->data);

        // Release writer lock
        if (sem_post(rw_mutex) == -1) {
            perror("sem_post rw_mutex");
            break;
        }
    }

    // Cleanup
    if (shmdt(shm) == -1) {
        perror("shmdt");
    }

    // Mark shared memory for deletion (when last detach happens)
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
    }

    sem_close(mutex);
    sem_close(rw_mutex);
    sem_unlink(SEM_MUTEX_NAME);
    sem_unlink(SEM_RW_NAME);

    return 0;
}
