#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h> 
#include <stdio.h>

void isOpen(int flag) {
    if (flag < 0) {
        err(2, "Could not open!");
    }
}

void writeNumber(int n) {
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%6d  ", n);
    write(1, buffer, len);
}

void readData(int isIndexed, int fd) {
    static int index = 1;
    isOpen(fd);

    char c;
    int newLine = 1;

    while (read(fd, &c, 1) == 1) {
        if (newLine && isIndexed) {
            writeNumber(index++);
            newLine = 0;
        }

        write(1, &c, 1);

        if (c == '\n') {
            newLine = 1;
        }
    }

    if (fd != 0) {
        close(fd);
    }
}

int main(int argc, char* argv[]) {
    int isIndexed = 0;
    int ind = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        isIndexed = 1;
        ind = 2;
    }

    for (; ind <= argc; ind++) {
        int fd = -1;

        if (strcmp(argv[ind], "-") == 0) {
            fd = 0;
        } else {
            fd = open(argv[ind], O_RDONLY);
        }

        readData(isIndexed, fd);
    }

    return 0;
}

