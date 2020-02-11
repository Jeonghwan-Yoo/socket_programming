#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHandling(char* message);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    FILE* fp;
    char buf[BUF_SIZE];
    int readCnt;

    SOCKADDR_IN servAdr, clntAdr;
    int clntAdrSize;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    fp = fopen("file_server_win.cpp", "rb");
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (struct sockaddr*)&servAdr, sizeof(servAdr)) == -1)
        ErrorHandling("bind() error");

    if (listen(hServSock, 5) == -1)
        ErrorHandling("listen() error");

    clntAdrSize = sizeof(clntAdr);
    hClntSock = accept(hServSock, (struct sockaddr*)&clntAdr, &clntAdrSize);

    while (1) {
        readCnt = fread((void*)buf, 1, BUF_SIZE, fp);
        if (readCnt < BUF_SIZE) {
            send(hClntSock, (char*)&buf, readCnt, 0);
            break;
        }
        send(hClntSock, (char*)&buf, BUF_SIZE, 0);
    }
    shutdown(hClntSock, SD_SEND);
    recv(hClntSock, (char*)buf, BUF_SIZE, 0);
    printf("Message from client : %s\n", buf);

    fclose(fp);
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
