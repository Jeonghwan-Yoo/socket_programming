#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_SIZE 100

void error_handling(char* message);

int main(void) {
    int fd;
    char buf[BUF_SIZE];

    // 파일 data.txt를 읽기 전용으로 열고 있다.
    fd = open("data.txt", O_RDONLY);
    if (fd == -1)
        error_handling("open() error!");
    printf("file descriptor : %d\n", fd);

    // read 함수를 이용해서 배열 buf에 읽어 들인 데이터를 저장하고 있다.
    if (read(fd, buf, sizeof(buf)) == -1)
        error_handling("read() error!");
    printf("file data : %s", buf);
    close(fd);
    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
