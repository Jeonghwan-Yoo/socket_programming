#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

void ErrorHandling(char* meesage);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    char message[30];
    int strLen;
    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // 소켓 라이브러리 초기화.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // 소켓 생성.
    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    // 서버에 연결요청
    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");

    // 서버로부터 전송되는 데이터를 수신.
    strLen = recv(hSocket, message, sizeof(message) - 1, 0);
    if (strLen == -1)
        ErrorHandling("read() error!");
    printf("Message from server : %s\n", message);
    // 소켓라이브러리를 해제.
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}