// #undef는 기존에 정의된 매크로를 해제하는 경우에 사용한다.
// VC++ 자체적으로 이 두 매크로를 정의하는 경우가 있는데, 두 함수의 매개변수형이 유니코드 기반으로 바뀌어서 잘못된 실행결과.
#undef UNICODE
#undef _UNICODE
#include <stdio.h>
#include <winsock2.h>

int main(int argc, char* argv[]) {
    char* strAddr = "203.211.218.102:9190";
    
    char strAddrBuf[50];
    SOCKADDR_IN servAddr;
    int size;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    size = sizeof(servAddr);
    WSAStringToAddress(strAddr, AF_INET, NULL, (SOCKADDR*)&servAddr, &size);
    
    size = sizeof(strAddrBuf);
    // 구조체 변수에 저장된 주소정보를 문자열의 형태로 변환하고 있다.
    WSAAddressToString((SOCKADDR*)&servAddr, sizeof(servAddr), NULL, strAddrBuf, (LPDWORD)&size);

    printf("Second conv result : %s\n", strAddrBuf);
    WSACleanup();
    return 0;
}