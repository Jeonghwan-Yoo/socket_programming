#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void* read(void *arg);
void* accu(void *arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char *argv[]) {
    pthread_t id_t1, id_t2;
    // 세마포어를 두 개 생성하고 있다.
    // 하나는 값이 0, 하나는 1이다.
    sem_init(&sem_one, 0, 0);
    sem_init(&sem_two, 0, 1);

    pthread_create(&id_t1, NULL, read, NULL);
    pthread_create(&id_t2, NULL, accu, NULL);

    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);

    sem_destroy(&sem_one);
    sem_destroy(&sem_two);
    return 0;
}

void* read(void *arg) {
    int i;
    for (i = 0; i < 5; i++) {
        fputs("input num : ", stdout);
        // 이는 accu함수를 호출하는 쓰레드가 값을 가져가지도 않았는데, read 함수를 호출하는 쓰레드가 값을 덮어써버리는 상황을 막기 위함.
        sem_wait(&sem_two);
        scanf("%d", &num);
        // 이는 read함수를 호출하는 쓰레드가 새로운 값을 가져다 놓기 전에 accu함수가 이전 값을 다시 가져가는 상황을 막기 위함.
        sem_post(&sem_one);
    }
    return NULL;
}

void* accu(void *arg) {
    int sum = 0, i;
    for (i = 0; i < 5; i++) {
        sem_wait(&sem_one);
        sum += num;
        sem_post(&sem_two);
    }
    printf("Result : %d\n", sum);
    return NULL;
}