#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PLAYERS 100
#define MAX_TRAM_CAPACITY 4
#define MAX_WAITING_GERMANS 3
#define MAX_WAITING_ITALIANS 3

int num_germans = 0;
int num_italians = 0;

sem_t mutex;
sem_t germans_sem;
sem_t italians_sem;
sem_t tram_sem;

void *german_thread(void *arg) {
    int id = *((int *)arg);

    while (1) {
        // Simulate some delay for German arrival
        sleep(rand() % 3);

        sem_wait(&mutex);
        num_germans++;

        printf("German %d arrived. Total Germans: %d, Total Italians: %d\n", id, num_germans, num_italians);

        if (num_germans == MAX_WAITING_GERMANS && num_italians >= 1) {
            // If three Germans and at least one Italian are waiting, let them board the tram
            sem_post(&germans_sem);
            sem_post(&germans_sem);
            sem_post(&germans_sem);
            sem_post(&italians_sem);
            num_germans -= 3;
            num_italians--;
            printf("Tram departing with 3 Germans and 1 Italian...\n");
            sem_post(&tram_sem);
        } else if (num_germans + num_italians == MAX_TRAM_CAPACITY) {
            // If the tram is full, let it depart
            printf("Tram departing with 4 passengers...\n");
            sem_post(&tram_sem);
        }

        sem_post(&mutex);

        // Simulate some delay before the next arrival
        sleep(rand() % 2);
    }

    return NULL;
}

void *italian_thread(void *arg) {
    int id = *((int *)arg);

    while (1) {
        // Simulate some delay for Italian arrival
        sleep(rand() % 3);

        sem_wait(&mutex);
        num_italians++;

        printf("Italian %d arrived. Total Germans: %d, Total Italians: %d\n", id, num_germans, num_italians);

        if (num_italians == MAX_WAITING_ITALIANS && num_germans >= 1) {
            // If three Italians and at least one German are waiting, let them board the tram
            sem_post(&italians_sem);
            sem_post(&italians_sem);
            sem_post(&italians_sem);
            sem_post(&germans_sem);
            num_italians -= 3;
            num_germans--;
            printf("Tram departing with 1 German and 3 Italians...\n");
            sem_post(&tram_sem);
        } else if (num_germans + num_italians == MAX_TRAM_CAPACITY) {
            // If the tram is full, let it depart
            printf("Tram departing with 4 passengers...\n");
            sem_post(&tram_sem);
        }

        sem_post(&mutex);

        // Simulate some delay before the next arrival
        sleep(rand() % 2);
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    sem_init(&mutex, 0, 1);
    sem_init(&germans_sem, 0, 0);
    sem_init(&italians_sem, 0, 0);
    sem_init(&tram_sem, 0, 0);

    pthread_t german_threads[MAX_PLAYERS];
    pthread_t italian_threads[MAX_PLAYERS];

    int german_ids[MAX_PLAYERS];
    int italian_ids[MAX_PLAYERS];

    for (int i = 0; i < MAX_PLAYERS; i++) {
        german_ids[i] = i + 1;
        italian_ids[i] = i + 1;
        pthread_create(&german_threads[i], NULL, german_thread, &german_ids[i]);
        pthread_create(&italian_threads[i], NULL, italian_thread, &italian_ids[i]);
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        pthread_join(german_threads[i], NULL);
        pthread_join(italian_threads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&germans_sem);
    sem_destroy(&italians_sem);
    sem_destroy(&tram_sem);

    return 0;
}

