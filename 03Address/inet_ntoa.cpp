#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    struct sockaddr_in addr1, addr2;
    char* str_ptr;
    char str_arr[20];

    addr1.sin_addr.s_addr = htonl(0x1020304);
    addr2.sin_addr.s_addr = htonl(0x1010101);

    // IP주소 정보를 담은 문자열의 주소 값이 반환
    str_ptr = inet_ntoa(addr1.sin_addr);
    strcpy(str_arr, str_ptr);
    printf("Dotted_Decimal notatoin1 : %s\n", str_ptr);

    // 반환된 주소 값에는 다른 문자열 정보가 채워진다.
    inet_ntoa(addr2.sin_addr);
    printf("Dotted-Decimal notation2 : %s\n", str_ptr);
    printf("Dotted-Decimal notation3 : %s\n", str_arr);
    return 0;
}