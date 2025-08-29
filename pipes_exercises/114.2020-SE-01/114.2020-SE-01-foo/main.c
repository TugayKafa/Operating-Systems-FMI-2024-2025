#include <err.h> // err()
#include <string.h>
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if (argc != 2){
        err(1, "Wrong arguments!");
    }

    if(mkfifo("fifo", 0666) == -1){
        if(errno != EEXIST){
            err(5, "Could not mkfifo!");
        }
    }
    
    int fd_fifo = open("fifo", O_WRONLY);
    if(fd_fifo<0) {
        err(4, "Could not open!");
    }

    int dp = dup2(fd_fifo, 1);
    if(dp < 0){
        err(7, "Could not dup!");
    }

    close(fd_fifo);
    
    execlp("cat", "cat", argv[1], NULL);
    err(6, "Could not exec!");

    return 0;
}
