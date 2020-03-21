#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

// 클라이언트와 연결된 소켓정보를 담기 위해 정의된 구조체이다.
// 이 구조체를 기반으로 변수가 언제할당되고 어떻게 전달이 되며, 어떻게 활용되는지 관찰해야 한다.
typedef struct // socket info
{
    SOCKET hClntSock;
    SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// IO에 사용되는 버퍼와 Overlapped IO에 반드시 필요한 구조체 변수를 담아서 구조체를 정의.
// 구조체 변수의 주소 값은 구조체 첫 번째 멤버의 주소 값과 일치한다.
typedef struct // buffer info
{
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[BUF_SIZE];
    int rwMode; // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI EchoThreadMain(LPVOID CompletionPortIO);
void ErrorHandling(char *message);

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    HANDLE hComPort;
    SYSTEM_INFO sysInfo;
    LPPER_IO_DATA ioInfo;
    LPPER_HANDLE_DATA handleInfo;
    
    SOCKET hServSock;
    SOCKADDR_IN servAdr;
    int recvBytes, i, flags = 0;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // CP 오브젝트를 생성하고 있다. 마지막 전달인자가 0이니, 코어의 수만큼 쓰레드가 CP 오브젝트에 할당될 수 있다.
    hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    // 현재 실행중인 시스템 정보를 얻기 위해서 GetSystemInfo 함수를 호출하고 있다.
    GetSystemInfo(&sysInfo);
    // dwNumberOfProcessors에는 CPU의 수가 저장된다. CPU의 수만큼 쓰레드를 생성하고 있다.
    // 쓰레드 생성시 CP오브젝트의 핸들을 전달하고 있다.
    // 쓰레드는 이 핸들을 대상으로 CP 오브젝트에 접근을 한다.
    // 쓰레드는 이 핸들로 인해 CP 오브젝트에 할당이 이뤄진다.
    for (i = 0; i < sysInfo.dwNumberOfProcessors; ++i)
        _beginthreadex(NULL, 0, (unsigned (_stdcall *)(void *))EchoThreadMain, (LPVOID)hComPort, 0, NULL);

    hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
    listen(hServSock, 5);

    while (1)
    {
        SOCKET hClntSock;
        SOCKADDR_IN clntAdr;
        int addrLen = sizeof(clntAdr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
        // PER_HANDLE_DATA 구조체 변수를 동적 할당한 다음에, 클라이언트와 연결된 소켓, 그리고 클라이언트의 주소정보를 담고 있다.
        handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
        handleInfo->hClntSock = hClntSock;
        memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

        // CP오브젝트와 생성된 소켓을 연결하고 있다.
        // 이 소켓을 기반으로 하는 Overlapped IO가 완료될 때, 연결된 CP 오브젝트에 완료에 대한 정보가 삽입되고, 이로인해서 GetQueued.. 함수는 반환이 된다.
        // handleInfo도 GetQueued함수가 반환하면서 얻게 된다.
        CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);

        // PER_IO_DATA 구조체 변수를 동적 할당하였다.
        // WSARecv 함수호출에 필요한 OVERLAPPED 구조체 변수와 WSABUF 구조체 변수, 버퍼까지 마련.
        ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        ioInfo->wsaBuf.len = BUF_SIZE;
        ioInfo->wsaBuf.buf = ioInfo->buffer;
        // IOCP는 기본적으로 입력의 완료와 출력의 완료를 구분 지어주지 않는다.
        // 다만 입력이건 출력이건 완료되었다는 사실만 인식을 시켜준다.
        // 따라서 입력을 진행한 것인지, 출력을 진행한 것인지에 대한 정보를 변도로 기록해둬야 한다.
        // PER_IO_DATA 구조체 변수의 멤버 reMode가 이러한 목적으로 삽입된 멤버이다.
        ioInfo->rwMode = READ;
        // WSARecv 함수를 호출하면서 여섯번째 인자로 OVERLAPPED 구조체 변수의 주소 값을 전달하였다.
        // 이 값은 이후에 GetQueued 함수가 반환을 하면서 얻을 수 있다.
        // 구조체 변수의 주소 값은 첫 번째 멤버의 주소 값과 동일하므로 PER_IO_DATA 구조체 변수의 주소값을 전달한 것고 같다.
        WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, (LPDWORD)recvBytes, (LPDWORD)flags, &(ioInfo->overlapped), NULL);
    }
    return 0;
}

// GetQueued 함수를 호출하는 쓰레드를 가리켜 CP 오브젝트에 할당된 쓰레드라 한다.
DWORD WINAPI EchoThreadMain(LPVOID pComPort)
{
    HANDLE hComPort = (HANDLE)pComPort;
    SOCKET sock;
    DWORD bytesTrans;
    LPPER_HANDLE_DATA handleInfo;
    LPPER_IO_DATA ioInfo;
    DWORD flags = 0;

    while (1)
    {
        // IO가 완ㄹ되고, 이에 대한 정보가 등록되었을 때 반환한다. (마지막 전달인자가 INFINITE이므로)
        // 반환할 때 세 번째, 네 번째 인자를 통해서 정보를 얻게 된다.
        GetQueuedCompletionStatus(hComPort, &bytesTrans, (PULONG_PTR)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
        sock = handleInfo->hClntSock;

        // 포인터 ioInfo에 저장된 값은 OVERLAPPED 구조체 변수의 주소 값이지만, PER_IO_DATA의 구조체 변수의 주소 값.
        // 메버 rwMode에 저장된 값의 확인을 통해서 입력의 완료인지 출력의 완료인지 확인하고 있다.
        if (ioInfo->rwMode == READ)
        {
            puts("message received");
            if (bytesTrans == 0) // EOF 전송 시
            {
                closesocket(sock);
                free(handleInfo);
                free(ioInfo);
                continue;
            }
            // 서버가 수신한 메시지를 클라이언트에게 재전송하는 과정을 보이고 있다.
            memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
            ioInfo->wsaBuf.len = bytesTrans;
            ioInfo->rwMode = WRITE;
            WSASend(sock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);

            // 메시지 재전송 이후에 클라이언트가 전송하는 메시지의 수신과정을 보이고 있다.
            ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
            memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
            ioInfo->wsaBuf.len = BUF_SIZE;
            ioInfo->wsaBuf.buf = ioInfo->buffer;
            ioInfo->rwMode = READ;
            WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);

        } 
        // 완료된 IO가 출력인 경우에 실행.
        else 
        {
            puts("message sent!");
            free(ioInfo);
        }
    }
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}