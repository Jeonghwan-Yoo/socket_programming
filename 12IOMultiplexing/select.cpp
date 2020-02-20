#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char* argv[]) {
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;

    // fd_set형 변수를 초기화하고 파일 디스크립터 0의 위치를 1로 설정하고 있다.
    // 표준입력에 변화가 있는지 관심을 두고 보겠다는 뜻.
    FD_ZERO(&reads);
    FD_SET(0, &reads); // 0 is standard input(console)

    /*
    // select 함수의 타임아웃 설정을 위한 코드.
    // 이 위치에서 타임아웃을 설정하게 되면 select 함수호출 후에는 구조체 timeval의 멤버 tv_sec와 tv_usec에 저장된 값이 타임아웃이 발생하기까지 남았던 기간으로 바뀌기 때문.
    // 따라서 select 함수를 호출하기 전에 매번 timeval 구조체 변수의 초기화를 반복해야 한다.
    timeout.tv_sec = 5;
    timeout.tv_usec = 5000;
    */

   while (1) {
       // 미리 준비해 둔 fd_set형 변수 reads의 내용을 변수 temps에 복사.
       // select 함수호출이 끝나면 변화가 생긴 파일 디스크립터의 위치를 제외한 나머지 위치의 비트들은 0으로 초기화 된다.
       // 원본의 유지를 위해서는 복사의 과정을 거쳐야 한다.
       temps = reads;
       // timeval 구조체 변수의 초기화 코드를 반복문 안에 삽입해서 select 함수가 호출되기 전에 매번 새롭게 값이 초기화되도록 해야 한다.
       timeout.tv_sec = 5;
       timeout.tv_usec = 0;
       // 콘솔로부터 입력된 데이터가 있다면 0보다 큰 수가 반환되며, 입력된 데이터가 없어서 타임아웃이 발생하는 경우에는 0이 반환된다.
       result = select(1, &temps, 0, 0, &timeout);
       if (result == -1) {
           puts("select() error!");
           break;
       }
       else if (result == 0) {
           puts("Time-out!");
       }
       else {
           // select 함수가 0보다 큰 수를 반환했을 때 실행되는 영역.
           // 파일 디스크립터가 표준입력이 맞는지 확인하고, 맞으면 표준입력으로부터 데이터를 읽어서 콘솔로 데이터를 출력하고 있다.
           if (FD_ISSET(0, &temps)) {
               str_len = read(0, buf, BUF_SIZE);
               buf[str_len] = 0;
               printf("message from console : %s", buf);
           }
       }
   }
   return 0;
}
