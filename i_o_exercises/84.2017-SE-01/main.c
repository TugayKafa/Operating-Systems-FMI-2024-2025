#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

void isFileOpened(int flag){
    if(flag<0){
        err(2, "File could not open successfully!");
    }
}

void isStatSucc(int flag){
    if(flag < 0){
        err(3, "File could not stat1");
    }
}

void isSeekSucc(int flag){
    if(flag < 0){
        err(4, "Could not seek!");
    }
}

void isReadSucc(int flag){
    if(flag < 0){
        err(5, "Could not read!");
    }
}

void isWriteSucc(int flag){
    if(flag < 0){
        err(6, "Could not write!");
    }
}

typedef struct{
    uint16_t seek;
    uint8_t orig;
    uint8_t new;
} patch_t;

int main(int argc, char *argv[]) {
    if(argc != 4){
        err(1, "Wrong arguments!");
    }

    int fd1 = open(argv[1], O_RDONLY);
    isFileOpened(fd1);

    int fd2 = open(argv[2], O_RDONLY);
    isFileOpened(fd2);

    struct stat info_fd1, info_fd2;
    int st1 = fstat(fd1, &info_fd1);
    isStatSucc(st1);
    int st2 = fstat(fd2, &info_fd2);
    isStatSucc(st2);

    int fd1_size = info_fd1.st_size;
    int fd2_size = info_fd2.st_size;

    if(fd1_size != fd2_size){
        err(4, "Files must be with same lenght!");
    }
    
    int seek = lseek(fd1, 0, SEEK_SET);
    isSeekSucc(seek);
    seek = lseek(fd2, 0, SEEK_SET);
    isSeekSucc(seek);
    
    int fd3 = open(argv[3], O_CREAT | O_RDWR | O_TRUNC, 0644);
    isFileOpened(fd3);

    for(int i = 0; i < fd1_size; i++){
        char a, b;
        int rd = read(fd1, &a, 1);
        isReadSucc(rd);
        rd = read(fd2, &b, 1);
        isReadSucc(rd);
        
        patch_t patch;
        if(a != b){
            patch.seek = (uint16_t) i;
            patch.orig = a;
            patch.new = b;
            int wr = write(fd3, &patch, sizeof(patch_t));
            isWriteSucc(wr);
        }
    }

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
