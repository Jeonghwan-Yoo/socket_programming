#include <stdio.h>
#include <windows.h>
#include <process.h>

#define STR_LEN 100

unsigned WINAPI NumberOfA(void *arg);
unsigned WINAPI NumberOfOthers(void *arg);

static char str[STR_LEN];
static HANDLE hEvent;

int main(int argc, char *argv[]) {
    HANDLE hThread1, hThread2;
    // manual-reset 모드의 Event 오브젝트를 non-signaled 상태로 생성하고 있다.
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    // 함수호출을 통해서 문자열이 입력되기만을 기다린다.
    hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);

    fputs("Input string : ", stdout);
    fgets(str, STR_LEN, stdin);
    // 문자열이 입력된 후에 Event 오브젝트를 signaled 상태로 변경하고 있다.
    // 동시에 두 쓰레드가 함께 대기상태에서 빠져나올 수 있는 이유는 Event 오브젝트가 여전히 signaled 상태에 있기 때문.
    SetEvent(hEvent);

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);
    // 여기선 필요없지만 Event 오브젝트를 다시 non-signaled 상태로 되돌리고 있다.
    // 직접적으로 되돌려놓지 않으면, 계속해서 signaled 상태에 놓여있게 된다.
    ResetEvent(hEvent);
    CloseHandle(hEvent);
    return 0;
}

unsigned WINAPI NumberOfA(void *arg) {
    int i, cnt = 0;
    // 대기 중이던 쓰레드가 대기 상태에서 빠져 나와 실행을 이어간다.
    WaitForSingleObject(hEvent, INFINITE);
    for (i = 0; str[i] != 0; i++) {
        if (str[i] == 'A')
            cnt++;
    }
    printf("Num of A : %d\n", cnt);
    return 0;
}

unsigned WINAPI NumberOfOthers(void *arg) {
    int i, cnt = 0;
    // 대기 중이던 쓰레드가 대기 상태에서 빠져 나와 실행을 이어간다.
    WaitForSingleObject(hEvent, INFINITE);
    for (i = 0; str[i] != 0; i++) {
        if (str[i] != 'A')
            cnt++;
    }
    printf("Num of others : %d\n", cnt - 1);
    return 0;
}