#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// Signal Handler
void timeout(int sig) {
    if (sig == SIGALRM)
        puts("Time out!");
    // 2초 간격으로 SIGALRM 시그널을 반복 발생시키기 위해 시그널 핸들러 내에서 alarm 함수를 호출하고 있다.
    alarm(2);
}
// Signal Handler
void keycontrol(int sig) {
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}

int main(int argc, char* argv[]) {
    int i;
    // 시그널 핸들러를 등록.
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    // 시그널 SIGALRM의 발생을 2초 뒤로 예약.
    alarm(2);

    for (i = 0; i < 3; i++) {
        puts("wait...");
        // 시그널의 발생과 시그널 핸들러의 실행을 확인
        // 5분으로 예상하지만 실제 실행시간은 짧다.
        sleep(100);
    }
    return 0;
}
