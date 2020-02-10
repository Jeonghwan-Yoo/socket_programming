#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
// 피연산자의 바이트 수와 연산결과의 바이트 수를 상수화.
#define RLT_SIZE 4
#define OPSZ 4
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int sock;
    // 데이터의 송수신을 위한 메모리 공간은 배열을 기반으로 생성하는 것이 좋다.
    // 데이터를 누적해서 송수신해야 하기 때문이다.
    char opmsg[BUF_SIZE];
    int result, opnd_cnt, i;    
    struct sockaddr_in serv_addr;
    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // 소켓을 생성. TCP여야한다.
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    // 소켓의 주소할당을 위해 구조체 변수를 초기화.
    // 초기화되는 값은 연결을 목적으로 하는 서버 소켓의 IP와 PORT이다.
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 연결요청 정보가 서버의 대기 큐에 등록이 되면, connect 함수는 정상적으로 호출을 완료.
    // 연결되었다고 해도 서버에서 accept 함수를 호출하지 않으면 서비스가 이뤄지지 않는다.
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
    else
        puts("Connected.....");

    // 피연자의 개수 정보를 입력 받은 후, opmsg에 저장.
    // char형변환은 피연산자의 개수정보를 1바이트 정수형태로 전달한다.
    // 개수정보는 음수가 될 수 없으므로, 부호 없는 양의 정수형태로 작성하는것이 합리적.
    fputs("Operand count : ", stdout);
    scanf("%d", &opnd_cnt);
    opmsg[0] = (char)opnd_cnt;

    // 정수를 입력 받아서 배열 opmsg에 이어서 저장.
    // char형 배열에 4바이트 int형 정수를 저장해야 하기 때문에 int형 포인터로 형변환.
    for (i = 0; i < opnd_cnt; i++) {
        printf("Operand %d : ", i + 1);
        scanf("%d", (int*)&opmsg[i * OPSZ + 1]);
    }
    // 앞에 남아있는 \n문자의 삭제를 위해 fgetc 함수가 호출.
    fgetc(stdin);
    fputs("Operator : ", stdout);
    // 연산자 정보를 받아 배열 opmsg에 저장.
    scanf("%c", &opmsg[opnd_cnt * OPSZ + 1]);
    // write 함수호출을 통해서 opmsg에 저장되어 있는 연산과 관련돈 정보를 한방에 전송.
    // 한번에 하든 여러 번 나눠서 하든 TCP는 데이터의 경계가 존재하지 않는다.
    write(sock, opmsg, opnd_cnt * OPSZ + 2);
    // 연산 결과의 저장.
    read(sock, &result, RLT_SIZE);

    printf("Operation result : %d\n", result);
    close(sock);
    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
