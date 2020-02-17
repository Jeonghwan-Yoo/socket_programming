#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char* argv[]) {
    int fds[2];
    char str[] = "Who are you?";
    char buf[BUF_SIZE];
    pid_t pid;

    // pipe 함수호출을 통해서 파이프를 생성하고 있다.
    // 배열 fds에는 입출력을 위한 파일 디스크립터가 각각 저장된다.
    pipe(fds);
    // 자식 프로세스는 두 개의 파일 디스크립터를 함께 소유하게 된다.
    // 파이프가 복사된 것이 아니라 파이프의 입출력에 사용되는 파일 디스크립터가 복사된 것.
    // 부모와 자식 프로세스가 동시에 입출력 파일 디스크립터를 모두 소유.
    pid = fork();
    if (pid == 0) {
        // 자식 프로세스는 파이프로 문자열을 전달한다.
        write(fds[1], str, sizeof(str));
    }
    else {
        // 부모 프로세스는 파이프로부터 문자열을 수신한다.
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }
    return 0;
}