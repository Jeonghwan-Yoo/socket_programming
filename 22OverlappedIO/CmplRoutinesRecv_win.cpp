#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void CALLBACK CompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(char *mesage);

WSABUF dataBuf;
char buf[BUF_SIZE];
int recvBytes = 0;

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;

    WSAOVERLAPPED overlapped;
    WSAEVENT evObj;

    int idx, recvAdrSz, flags = 0;
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    memset(&lisnAdr, 0, sizeof(lisnAdr));
    lisnAdr.sin_family = AF_INET;
    lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    if (listen(hLisnSock, 5) == SOCKET_ERROR)
        ErrorHandling("accept() error");

    memset(&overlapped, 0, sizeof(overlapped));
    dataBuf.len = BUF_SIZE;
    dataBuf.buf = buf;
    evObj = WSACreateEvent(); // Dummy event object

    // Completion Routine에 해당하는 함수의 주소 값을 전달하고 있다.
    // Completion Routine을 이용하더라도 WSAOVERLAPPED 구조체 변수의 주소 값은 반드시 전달해줘야 한다.
    // 단 hEvent의 초기화를 위해서 Event 오브젝트를 생성할 필요는 없다.
    if (WSARecv(hRecvSock, &dataBuf, 1, (LPDWORD)recvBytes, (LPDWORD)flags, &overlapped, CompRoutine) == SOCKET_ERROR) {
        // 데이터의 수신이 Pending 되었는지 확인
        if (WSAGetLastError() == WSA_IO_PENDING)
            puts("Background data receive");
    }

    // main 쓰레드를 alertable wait 상태로 두기 위한 함수호출이다.
    // 이 함수의 호출을 위해서 불필요한 Event 오브젝트를 하나 생성하였다. (더미 오브젝트)
    // 만약에 SleepEx 함수를 활용한다면 더미 오브젝트의 생성을 피할 수 있다.
    idx = WSAWaitForMultipleEvents(1, &evObj, FALSE, WSA_INFINITE, TRUE);
    // WAIT_IO_COMPLETION이 반환되었다는 것은 IO의 정상완료를 뜻한다.
    if (idx == WAIT_IO_COMPLETION)
        puts("Overlapped I/O Completed");
    else // If error occurred;
        ErrorHandling("WSARecv() error");

    WSACloseEvent(evObj);
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

void CALLBACK CompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    if (dwError != 0) {
        ErrorHandling("CompRoutine error");
    }
    else {
        recvBytes = szRecvBytes;
        printf("Received mesage ; %s\n", buf);
    }
}

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}