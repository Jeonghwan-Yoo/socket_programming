#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void error_handling(char* message);

int main(int argc, char* argv[]) {
    char* addr = "127.232.124.79";
    struct sockaddr_in addr_inet;

    // in_addr 변수의 주소값을 넣어준다.
    if(!inet_aton(addr, &addr_inet.sin_addr))
        error_handling("Conversoin error");
    else
        printf("Network ordered integer addr : %#x\n", addr_inet.sin_addr.s_addr);
    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}