#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char* argv[]) {
    struct iovec vec[2];
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";
    int str_len;

    // 첫 번째로 전송할 데이터가 저장된 위치와 크기정보
    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;
    // 두 번째로 전송할 데이터가 저장된 위치와 크기정보
    vec[1].iov_base = buf2;
    vec[1].iov_len = 4;

    // 콘솔로 출력이 이뤄진다.
    str_len = writev(1, vec, 2);
    puts("");
    printf("wrtie types : %d\n", str_len);
    return 0;
}