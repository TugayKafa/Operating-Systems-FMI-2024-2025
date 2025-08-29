#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    uint8_t buf[1024];
    ssize_t r = read(0, buf, sizeof(buf));
    if (r < 0) {
        perror("read");
        exit(1);
    }

    // Ако входът започва с 0xDE, 0xAD, 0xBE, 0xEF -> предизвикай краш
    if (r >= 4 && buf[0] == 0xDE && buf[1] == 0xAD && buf[2] == 0xBE && buf[3] == 0xEF) {
        char *p = NULL;
        *p = 42; // segmentation fault
    }

    return 0;
}
