#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N       200
#define HALF    100
#define SPACES    8

sem_t s, t, m;

int occupied = 0;

void *producer(int *nothing);
void *consumer(int *nothing);

int buffer[SPACES] = {0};

int main()
{
    pthread_t th[N+1];
    int i;
    int procount=0, concount=0;
    int id[N];


    sem_init(&s, 0, SPACES);
    sem_init(&t, 0, 0);
    sem_init(&m, 0, 1);
    srandom(getpid());

    for(i=1;i<=N;i++){
        id[i] = i;
        if(procount == HALF){
            pthread_create(&th[i], 0, (void *) consumer, &id[i]);
            concount++;
        } else if(concount == HALF){
            pthread_create(&th[i], 0, (void *) producer, &id[i]);
            procount++;
        } else if(random() % 100 < 50){
            pthread_create(&th[i], 0, (void *) producer, &id[i]);
            procount++;
        } else {
            pthread_create(&th[i], 0, (void *) consumer, &id[i]);
            concount++;
        }
    }

    for(i=1;i<=N;i++)
        pthread_join(th[i], 0);
}

void *producer(int *nothing)
{
    sem_wait(&s);
    usleep(1 + random() % 100);
    sem_wait(&m);
    buffer[occupied++] = *nothing;
    printf("thread %d (producer) stored item %d in buffer\n", *nothing, *nothing);
    sem_post(&m);
    sem_post(&t);
    pthread_exit(0);
}

void *consumer(int *nothing)
{
    sem_wait(&t);
    usleep(1 + random() % 100);
    sem_wait(&m);
    printf("thread %d (consumer) removed item %d from buffer\n", *nothing, buffer[--occupied]);
    buffer[occupied] = 0;
    sem_post(&m);
    sem_post(&s);
    pthread_exit(0);
}