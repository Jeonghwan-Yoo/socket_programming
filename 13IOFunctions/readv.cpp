#include <stdio.h>
#include <sys/uio.h>

#define BUF_SIZE 100

int main(int argc, char* argv[]) {
    struct iovec vec[2];

    char buf1[BUF_SIZE] = { 0, };
    char buf2[BUF_SIZE] = { 0, };
    int str_len;

    // 첫 번째 데이터 저장소의 위치와 저장할 데이터의 크기 정보를 설정하고 있다.
    // 저장할 데이터의 크기를 5로 지정했기 때문에 buf1의 크기에 상관없이 최대 5바이트만이 영역에 저장.
    vec[0].iov_base = buf1;
    vec[0].iov_len = 5;
    // vec[0]에 등록되어있는 버퍼에 5바이트가 저장되고, 나머지 데이터는 vec[1]에 등록되어 있는 버퍼에 저장된다.
    // 특히 구조체 iovec의 멤버 iov_len에는 버퍼에 저장할 최대 바이트 크기 정보를 저장해야 한다.
    vec[1].iov_base = buf2;
    vec[1].iov_len = BUF_SIZE;

    // readv 함수의 첫 번째 전달인자가 0이기 때문에 콘솔로부터 데이터를 수신한다.
    str_len = readv(0, vec, 2);
    printf("Read bytes : %d\n", str_len);
    printf("First message : %s\n", buf1);
    printf("Second message : %s\n", buf2);
    return 0;
}