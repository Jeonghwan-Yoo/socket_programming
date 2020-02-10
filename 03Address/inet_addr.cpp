#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    char* addr1 = "1.2.3.4";
    // 1바이트당 표현할 수 있는 최대 크기의 정수는 255이므로 잘못돔. 오류 검출능력을 확인.
    char* addr2 = "1.2.3.256";

    unsigned long conv_addr = inet_addr(addr1);
    if (conv_addr == INADDR_NONE)
        printf("Error occured!\n");
    else
        printf("Network ordered integer addr : %#lx\n", conv_addr);

    // 비정상
    conv_addr = inet_addr(addr2);
    if (conv_addr == INADDR_NONE)
        printf("Error occured\n");
    else
        printf("Network ordered integer addr : %#lx\n\n", conv_addr);
    return 0;   
}