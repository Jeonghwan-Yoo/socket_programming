#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char* argv[]) {
    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    pid_t pid;

    // 두 개의 파이프를 생성하고 있다.
    pipe(fds1), pipe(fds2);
    pid = fork();
    if (pid == 0) {
        // 자식 프로세스에서 부모 프로세스로의 데이터 전송은 배열 fds1이 참조하는 파이프
        write(fds1[1], str1, sizeof(str1));
        // 부모 프로세스에서 자식 프로세스로의 데이터 전송은 배열 fds2이 참조하는 파이프
        read(fds2[0], buf, BUF_SIZE);
        printf("Child proc output : %s\n", buf);
    }
    else {
        read(fds1[0], buf, BUF_SIZE);
        printf("Parent proc output : %s\n", buf);
        write(fds2[1], str2, sizeof(str2));
        // 부모 프로세스의 종료를 지연시키기 위해.
        sleep(3);
    }
    return 0;
}
