#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

typedef struct {
    char name[8];
    uint32_t offset;
    uint32_t length;
} triple_t;

int wrapper_fork(void){
    int pid = fork();
    if(pid < 0){
        err(4, "Could not fork!");
    }
    return pid;
}

int getFileSize(int fd){
    struct stat info;
    if(fstat(fd, &info)<0){
        err(3, "Could not stat!");
    }
    return info.st_size;
}

void wrapper_lseek(int fd, int offset, int whence){
    int ls = lseek(fd, offset, whence);
    if(ls < 0){
        err(7, "Could not seek!");
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2){
        err(1, "Wrong arguments!");
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd<0){
        err(2, "File could not open!");
    }

    int size = getFileSize(fd)/sizeof(triple_t);
    if(size > 8) {
        size=8;
    }

    int pipe1[2];
    if(pipe(pipe1) < 0){
        err(5, "Could not pipe!");
    }
    for (int i = 1; i <= size; i++){
        triple_t triple;
        int rd = read(fd, &triple, sizeof(triple_t));
        if(rd < 0){
            err(6, "Could not read triplet");
        }

        int pid = wrapper_fork();
        if(pid==0){
            close(pipe1[0]);
            int fd2 = open(triple.name, O_RDONLY);
            if(fd2 < 0){
                err(2, "File could not open!");
            }
            wrapper_lseek(fd2, 2*triple.offset, SEEK_SET);
            uint16_t res = 0;
            for(uint32_t j = 1; j<triple.length; j++){
                uint16_t curr;
                int rd_cur = read(fd2, &curr, sizeof(curr));
                if(rd_cur<0){
                    err(8, "Could not read num!");
                }
                res ^= curr;
            }
            int wr = write(pipe1[1], &res, sizeof(res));
            if(wr < 0){
                err(9, "Could not write to pipe!");
            }
            close(fd2);
            close(pipe1[1]);
            exit(0);
        }
    }
    close(fd);
    close(pipe1[1]);
    uint16_t res = 0;
    uint16_t cur;
    for(int i = 1; i<=size;i++){
        int rd = read(pipe1[0], &cur, sizeof(uint16_t));
        if(rd < 0){
            err(10, "Could not read to pipe!");
        }
        res ^= cur;
    }
    char buff[1024];
    int len = snprintf(buff, 1024, "%X\n", res);
    write(1, "result: ", 8);
    int wr = write(1, buff, len);
    if(wr < 0){
        err(11, "Could not write result!");
    }
    
    for(int i = 0;i<size;i++){
        wait(NULL);
    }

    close(pipe1[0]);
    return 0;
}
