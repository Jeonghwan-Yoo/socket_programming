#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char* argv[]) {
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds);
    pid = fork();
    if (pid == 0) {
        // 자식 프로세스의 실행 영역.
        // 데이터를 전송
        write(fds[1], str1, sizeof(str1));
        sleep(2);
        // 데이터를 수신
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output : %s\n", buf);
    }
    else {
        // 부모 프로세스의 실행 영역
        // 데이터를 수신.
        read(fds[0], buf, BUF_SIZE);
        printf("Parent proc output : %s\n", buf);
        // 데이터를 전송.
        write(fds[1], str2, sizeof(str2));
        // 부모 프로세스가 먼저 종료되면 명령 프롬프트가 떠버린다.
        // 자식 프로세스는 일을 진행하므로 크게 문제가 되진 않는다.
        // 자식 프로세스가 끝나기 전에 명령 프롬프트가 뜨는 것을 방지.
        sleep(3);
    }
    return 0;
}