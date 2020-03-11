#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
void ErrorHandling(char *msg);

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN sendAdr;

    WSABUF dataBuf;
    char msg[] = "Network is Computer!";
    int sendBytes = 0;

    WSAEVENT evObj;
    WSAOVERLAPPED overlapped;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    memset(&sendAdr, 0, sizeof(sendAdr));
    sendAdr.sin_family = AF_INET;
    sendAdr.sin_addr.s_addr = inet_addr(argv[1]);
    sendAdr.sin_port = htons(atoi(argv[2]));

    if (connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");

    // Event 오브젝트의 생성 및 전송할 데이터의 버퍼정보 초기화 과정.
    evObj = WSACreateEvent();
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = evObj;
    dataBuf.len = strlen(msg) + 1;
    dataBuf.buf = msg;

    // WSASend가 SOCKET_ERROR를 반환하지 않으면, 데이터 전송이 완료된 상황이니, sendBytes에 채워진 값은 의미를 갖는다.
    if (WSASend(hSocket, &dataBuf, 1, (LPDWORD)sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR) {
        // WSA_IO_PENDING을 반환하는 경우는 데이터의 전송이 완료되지는 않았지만 계속해서 진행중인 상태.
        // 이 경우 sendBytes에 채워진 값은 의미가 없다.
        if (WSAGetLastError() == WSA_IO_PENDING) {
            puts("Background data send");
            // 데이터 전송이 완료되면 등록된 Event 오브젝트가 signaled 상태가 되므로, 데이터 전송의 완료를 기다릴 수 있다.
            // 그리고 데이터의 전송완료가 확인되면 전송결과를 확인할 수 있다.
            WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
            WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD)sendBytes, FALSE, NULL);
        }
        else {
            ErrorHandling("WSASend() error");
        }
    }
    printf("Send data size : %d\n", sendBytes);
    WSACloseEvent(evObj);
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
