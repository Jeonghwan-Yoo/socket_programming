#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void error_handling(char* message);

int main(void) {
    int fd;
    char buf[] = "Let's go!\n";

    // 아무것도 저장되어 있지 않은 새로운 파일이 생성되어 쓰기만 가능하게 된다. 이미 파일이름이 존재한다면, 데이터는 지워져 버린다.
    fd = open("data.txt", O_CREAT|O_WRONLY|O_TRUNC);
    if (fd == -1)
        error_handling("open() error!");
    printf("file descriptor : %d\n", fd);
    // fd에 저장된 파일 디스크립터에 해당하는 파일에 buf에 저장된 데이터를 전송하고 있다.
    if (write(fd, buf, sizeof(buf)) == -1)
        error_handling("write() error!");
    close(fd);
    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
