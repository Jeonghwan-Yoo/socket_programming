#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100

void* thread_inc(void *arg);
void* thread_des(void *arg);

long long num = 0;
// 뮤텍스의 참조 값 저장을 위한 변수가 선언.
pthread_mutex_t mutex;

int main(int argc, char *argv[]) {
    pthread_t thread_id[NUM_THREAD];
    int i;

    pthread_mutex_init(&mutex, NULL);
    
    for (i = 0; i < NUM_THREAD; i++) {
        if (i % 2)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }

    for (i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("result : %lld\n", num);
    // 뮤텍스의 소멸을 보이고 있다.
    // 뮤텍스는 필요가 없어지면 소멸해야 한다.
    pthread_mutex_destroy(&mutex);
    return 0;
}

void* thread_inc(void *arg) {
    int i;
    // 임계영역
    pthread_mutex_lock(&mutex);
    for (i = 0; i < 50000000; i++)
        num += 1;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* thread_des(void *arg) {
    int i;
    // 임계영역
    for (i = 0; i < 50000000; i++) {
        pthread_mutex_lock(&mutex);
        num -= 1;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
