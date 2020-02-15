#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char* argv[]) {
    pid_t pid;
    int lval = 20;
    gval++, lval += 5;

    // 자식 프로세스를 생성하고 있다.
    // 부모 프로세스의 pid에는 자식 프로세스의 ID가 저장되며, 자식 프로세스의 pid에는 0이 저장된다.
    pid = fork();
    if (pid == 0) // Child Process. pid = 0
        gval += 2, lval += 2;
    else // Parent Process. pid = 자식 프로세스의 pid.
        gval -=2, lval -= 2;
    
    if (pid == 0)
        printf("Child Proc : [%d, %d]\n", gval, lval);
    else
        printf("Parent Proc : [%d, %d]\n", gval, lval);
    return 0;
}
