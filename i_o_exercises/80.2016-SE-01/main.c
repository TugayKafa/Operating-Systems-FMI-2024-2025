#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h> // за strlen()

#define MAX_SIZE 65536

int compare(const void *a, const void *b) {
    const unsigned char *x = (const unsigned char *)a;
    const unsigned char *y = (const unsigned char *)b;
    return (*x - *y);
}

void bubble_sort(unsigned char *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                unsigned char temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        const char *msg = "Usage: ./sort_bytes_static <binary_file>\n";
        write(2, msg, strlen(msg));
        exit(1);
    }

    const char *filename = argv[1];
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        const char *msg = "Error opening file\n";
        write(2, msg, strlen(msg));
        exit(2);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        const char *msg = "Error getting file info\n";
        write(2, msg, strlen(msg));
        close(fd);
        exit(3);
    }

    if (st.st_size <= 0 || st.st_size > MAX_SIZE) {
        const char *msg = "File size invalid or too large\n";
        write(2, msg, strlen(msg));
        close(fd);
        exit(4);
    }

    unsigned char buffer[MAX_SIZE];

    if (read(fd, buffer, st.st_size) != st.st_size) {
        const char *msg = "Error reading file\n";
        write(2, msg, strlen(msg));
        close(fd);
        exit(5);
    }

    bubble_sort(buffer, st.st_size);
    
    int fd2 = open("results.bin",O_WRONLY | O_CREAT | O_RDONLY | O_TRUNC, 0644);
    if(fd2 < 0) {
        write(2, "Errfile",8 );
        exit(7);
    }


    lseek(fd, 0, SEEK_SET);
    if (write(fd2, buffer, st.st_size) != st.st_size) {
        const char *msg = "Error writing file\n";
        write(2, msg, strlen(msg));
        close(fd2);
        exit(6);
    }

    close(fd);
    close(fd2);
    return 0;
}

