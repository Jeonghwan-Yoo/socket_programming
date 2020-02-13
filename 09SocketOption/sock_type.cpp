#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char* message);

int main(int argc, char* argv[]) {
    int tcp_sock, udp_sock;
    int sock_type;
    socklen_t optlen;
    int state;

    optlen = sizeof(sock_type);
    //TCP, UDP 소켓을 각각 생성하고 있다.
    tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    printf("SOCK_STREAM : %d\n", SOCK_STREAM);
    printf("SOCK_DGRAM : %d\n", SOCK_DGRAM);

    // 소켓의 타입을 얻고 있다.
    // TCP라면 SOCK_STREAM의 상수 값 1
    state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
    if (state)
        error_handling("getsockopt() error!");
    printf("Socket type one : %d\n", sock_type);

    // 소켓의 타입을 얻고 있다.
    // UDP라면 SOCK_DGRAM의 상수 값 2
    state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
    if (state)
        error_handling("getsockopt() error!");
    printf("Socket type two : %d\n", sock_type);
    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}