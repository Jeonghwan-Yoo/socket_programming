#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* thread_main(void *arg);

int main(int argc, char *argv[]) {
    pthread_t t_id;
    int thread_param = 5;

    // 별도의 실행흐름을 구성하는 쓰레드의 생성을 요청하고 있다.
    if (pthread_create(&t_id, NULL, thread_main, (void*)&thread_param) != 0) {
        puts("pthread_create() error");
        return -1;
    }
    // main 함수의 실행을 10초간 중지시키고 있다. 프로세스의 종료시기를 늦추기 위함이다.
    sleep(10);
    puts("end of main");
    // 프로세스의 종료는 그 안에서 생성된 쓰레드의 종료로 이어진다.
    return 0;
}

// 네번 째 전달인자 thread_param이 매개변수가 된다.
void* thread_main(void *arg) {
    int i;
    int cnt = (*(int*)arg);
    for (i = 0; i < cnt; i++) {
        sleep(1);
        puts("running thread");
    }
    return NULL;
}
