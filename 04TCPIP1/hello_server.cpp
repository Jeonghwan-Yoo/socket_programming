#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char* message);

int main(int argc, char* argv[]) {
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    char message[] = "Hello World!";

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    // 소켓을 생성.  
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) 
        error_handling("socket() error");

    // 소켓의 주소할당을 위해 구조체 변수를 초기화.
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.port = htons(atoi(argv[1]));

    // bind 함수를 호출.
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // 연결요청 대기상태로 들어가기 위해서 listen 함수를 호출.
    // 연결요청 대기 큐의 크기도 5로 설정하고 있다. 드디어 서버 소켓.
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    // 대기 큐에서 첫 번째로 대기 중에 있는 연결요청을 참조하여 클라이언트와의 연결을 구성하고, 이 때 생성된 소켓의 파일 디스크립터를 반환한다.
    // 비어 있다면 대기 큐가 찰 때까지, 연결요청이 들어올 때까지 accept 함수는 반환하지 않는다.
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error");

    // 데이터를 전송.
    write(clnt_sock, message, sizeof(message));
    // 연결을 끊음.
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}