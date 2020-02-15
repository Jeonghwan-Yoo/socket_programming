#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    pid_t pid = fork();

    if (pid == 0) { // Child Process
        puts("Hi, I am a child process");
    }
    else {
        // 자식 프로세스의 ID를 출력하고 있다.
        // 이 값을 통해서 자식 프로세스의 상태(좀비인지 아닌지)를 확인할 수 있다.
        printf("Child Process ID : %d\n", pid);
        // 30초간 부모 프로세스를 멈춘다.
        // 부모 프로세스가 종료되면 좀비 상태에 있던 자식 프로세스도 함께 소멸되기 때문에 좀비의 확인을 위해서는 부모 프로세스의 종료를 지연시킬 필요가 있다.
        sleep(30); // Sleep 30 sec.
    }
    if (pid == 0)
        puts("End child process");
    else
        puts("End parent process");
    return 0;
}