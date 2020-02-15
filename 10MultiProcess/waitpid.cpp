#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int status;
    pid_t pid = fork();

    if (pid == 0) {
        // 자식 프로세스의 종료를 늦추기 위해.
        sleep(15);
        return 24;
    }
    else {
        // 종료된 자식 프로세스가 없으면 0을 반환한다.
        while (!waitpid(-1, &status, WNOHANG)) {
            sleep(3);
            puts("sleep 3sec.");
        }
        if (WIFEXITED(status))
            printf("Child send %d\n", WEXITSTATUS(status));
    }
    return 0;
}