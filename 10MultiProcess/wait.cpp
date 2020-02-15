#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int status;
    // 생성된 자식 프로세스는 return 문 실행을 통해서 종료하게 된다.
    pid_t pid = fork();

    if (pid == 0) {
        return 3;
    }
    else {
        printf("Child PID:%d\n", pid);
        // 생성된 자식 프로세스는 exit 함수 호출을 통해서 종료하게 된다.
        pid = fork();
        if (pid == 0) {
            exit(7);
        }
        else {
            printf("Child PID:%d\n", pid);
            // wait 함수를 호출하고 있다.
            // 이로 인해서 종료된 프로세스 관련 정보는 status에 담기게 되고, 해당 정보의 프로세스는 완전히 소멸된다.
            wait(&status);
            // 자식 프로세스의 정상종료 여부를 확인하고 있다.
            if (WIFEXITED(status))
                // 정상종료인 경우에 한해서 자식 프로세스가 전달한 값을 출력하고 있다.
                printf("Child send one:%d\n", WEXITSTATUS(status));
            wait(&status);
            if (WIFEXITED(status))
                printf("Child send two:%d\n", WEXITSTATUS(status));
            // 부모 프로세스의 종료를 멈추기 위해서 삽입.
            // 이시간에 자식 프로세스의 상태를 확인하면 된다.
            sleep(30); // sleep 30 secs.
        }
    }
    return 0;
}
