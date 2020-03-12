#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(char *message);

// 소켓의 핸들과 버퍼, 버퍼관련 정보를 담는 WSAABUF형 변수가 묶여 있다.
typedef struct {
    SOCKET hClntSock;
    char buf[BUF_SIZE];
    WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hLisnSock, hRecvSock;
    SOCKADDR_IN lisnAdr, recvAdr;
    LPWSAOVERLAPPED lpOvLp;
    DWORD recvBytes;
    LPPER_IO_DATA hbInfo;
    int mode = 1, recvAdrSz, flagInfo = 0;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // 소켓을 넌-블로킹모드로 변경하고 있다.
    // 소켓은 처음 생성되면 블로킹 모드이기 때문에 넌-블로킹 모드로의 변환과정을 거쳐야 한다.
    hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    ioctlsocket(hLisnSock, FIONBIO, (u_long*)&mode); // for non-blocking mode socket

    memset(&lisnAdr, 0, sizeof(lisnAdr));
    lisnAdr.sin_family = AF_INET;
    lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);    
    lisnAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hLisnSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");
    
    recvAdrSz = sizeof(recvAdr);
    // 반복문 안에서 accept 함수를 호출하고 있다.
    // 특히 호출의 대상이 되는 소켓이 넌-블로킹 모드이므로, INVALID_SOCKET이 반환될 때의 처리과정 주목.
    while (1) {
        SleepEx(100, TRUE); // for alertable wait state
        hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);
        if (hRecvSock == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
                continue;
            else
                ErrorHandling("accept() error");
        }
        puts("Client connect...");
        // Overlapped IO에 필요한 구조체 변수를 할당하고, 초기화.
        // 반복문 안에서 WSAOVERLAPPED 구조체를 할당하는 이유는 클라이언트 하나당 WSAOVERLAPPED 구조체 변수를 하나씩 할당해야 하기 때문.
        lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
        memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));

        // PER_IO_DATA 구조체 변수를 동적 할당한 다음, 소켓의 핸들 정보를 저장하고 있다.
        // 이 소켓은 초기화된 버퍼를 사용하게 된다.
        hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        hbInfo->hClntSock = (DWORD)hRecvSock;
        (hbInfo->wsaBuf).buf = hbInfo->buf;
        (hbInfo->wsaBuf).len = BUF_SIZE;

        // WSAOVERLAPPED 구조체 변수 멤버 hEvent에 할당된 변수의 주소 값을 저장하고 있다.
        // Completion Routine 기반의 Overlapped IO에서는 Event 오브젝트가 불필요하기 때문에 hEvent에 필요한 다른 정보를 채워도 된다.
        lpOvLp->hEvent = (HANDLE)hbInfo; 
        // WSARecv를 호출하면서 ReadCompRoutine 함수를 Completion Routine으로 지정하고 있다.
        // WSAOVERLAPPED 구조체 변수의 주소 값은 Completion Routine의 세 번째 매개변수에 전달된다. 
        // Completion Routine 함수 내에서는 입출력이 완료된 소켓의 핸들과 버퍼에 접근할 수 있다.
        // 그리고 Completoin Routine의 실행을 위해서 SleepEx 함수를 반복 호출하고 있다.
        WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, (LPDWORD)flagInfo, lpOvLp, ReadCompRoutine);
    }
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

// 데이터의 입력이 완료되었다는 것.
// 따라서 수신된 데이터를 에코 클라이언트에게 전송해야 한다.
void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    // 입력이 완료된 소켓의 핸들과 버퍼정보를 추출하고 있는데, 이는 PER_IO_DATA 구조체 변수의 주소 값을 WSAOVERLAPPED 구조체 변수의 멤버 hEvent에 저장했기 때문.
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD sentBytes;

    // EOF의 수신을 의미하므로, 그에 따른 처리를 해야 한다.
    if (szRecvBytes == 0) {
        closesocket(hSock);
        free(lpOverlapped->hEvent);
        free(lpOverlapped);
        puts("Client disconnected...");
    }
    // Completion Routine으로 지정하면서 WSASend 함수를 호출하고 있다.
    // 클라이언트로 에코 메시지가 전송된다.
    else { // echo!
        bufInfo->len = szRecvBytes;
        WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine);
    }
}

// 에코 메시지가 전송된 이후에 호출된다. 그런데 다시 데이터를 수신해야 한다.
void CALLBACK WriteCompRoutine(DWORD dwError, DWORD dzSendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hSock = hbInfo->hClntSock;
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD recvBytes;
    int flagInfo = 0;
    WSARecv(hSock, bufInfo, 1, &recvBytes, (LPDWORD)flagInfo, lpOverlapped, ReadCompRoutine);
}

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}