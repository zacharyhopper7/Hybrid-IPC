// shm_consumer.c
// Consumer: reads data from shared memory using reader-writer semaphores.
// Compile: gcc -o shm_consumer shm_consumer.c -pthread

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
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shm_t *shm = (shm_t *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    sem_t *mutex = sem_open(SEM_MUTEX_NAME, 0);
    if (mutex == SEM_FAILED) {
        perror("sem_open mutex");
        exit(EXIT_FAILURE);
    }

    sem_t *rw_mutex = sem_open(SEM_RW_NAME, 0);
    if (rw_mutex == SEM_FAILED) {
        perror("sem_open rw_mutex");
        exit(EXIT_FAILURE);
    }

    printf("Consumer: waiting for data in shared memory...\n");

    while (1) {
        if (!shm->has_data) {
            usleep(100000); // 100ms poll
            continue;
        }

        // Reader: acquire mutex to update read_count
        if (sem_wait(mutex) == -1) {
            perror("sem_wait mutex");
            break;
        }

        shm->read_count++;
        if (shm->read_count == 1) {
            // First reader locks rw_mutex
            if (sem_wait(rw_mutex) == -1) {
                perror("sem_wait rw_mutex");
                sem_post(mutex);
                break;
            }
        }

        if (sem_post(mutex) == -1) {
            perror("sem_post mutex");
            break;
        }

        // Critical section: read data
        printf("Consumer read: %s", shm->data);

        // Reader: release
        if (sem_wait(mutex) == -1) {
            perror("sem_wait mutex");
            break;
        }

        shm->read_count--;
        if (shm->read_count == 0) {
            // Last reader releases rw_mutex
            if (sem_post(rw_mutex) == -1) {
                perror("sem_post rw_mutex");
                sem_post(mutex);
                break;
            }
        }

        if (sem_post(mutex) == -1) {
            perror("sem_post mutex");
            break;
        }

        sleep(1); // avoid spamming
    }

    if (shmdt(shm) == -1) {
        perror("shmdt");
    }

    sem_close(mutex);
    sem_close(rw_mutex);

    return 0;
}
