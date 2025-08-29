#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>

void checkDigit(char digit[]) {
    if(strlen(digit) != 1) {
        err(2, "You must provide positive digits as an arguments!");
    }

    if(digit[0] < '0' || digit[0] > '9') {
        err(2, "You must provide positive digits as an arguments!");
    }
}

int wrapper_read(int fd, void* buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(5, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void * buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(4, "Could not write!");
    }
    return wr;
}

int pipe_p[2], pipe_c[2];

void executeChild(void){
    int pid = fork();
    if(pid < 0){
        err(4, "Could not fork!");
    }
    if(pid == 0) {
        close(pipe_p[1]);
        close(pipe_c[0]);
        uint8_t signal;
        while(wrapper_read(pipe_p[0],&signal,1)==1){
            wrapper_write(1, "DONG\n", 5);
            wrapper_write(pipe_c[1], &signal, 1);
        }
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        err(1, "Wrong arguments!");
    }
    
    checkDigit(argv[1]);
    checkDigit(argv[2]);

    int N = argv[1][0] - '0';
    int D = argv[2][0] - '0';

    if(pipe(pipe_p) < 0 || pipe(pipe_c) < 0) {
        err(3, "Could not pipe!");
    } 
    
    executeChild();
    
    close(pipe_p[0]);
    close(pipe_c[1]);

    for(int i = 1; i <= N; i++) {
        wrapper_write(1, "DING ", 5);
        uint8_t signal = 0;
        wrapper_write(pipe_p[1], &signal, 1);
        wrapper_read(pipe_c[0], &signal, 1);
        sleep(D);
    }

    close(pipe_p[1]);
    close(pipe_c[0]);
    return 0;
}
