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

int main(int argc, char* argv[]) {
    if(argc < 2){
        err(1, "Wrong arguments!");
    }

    int fd = open("fifo", O_RDONLY);
    if(fd < 0){
        err(2, "Could not open pipe");
    }

    int dp = dup2(fd, 0);
    close(fd);
    if(dp < 0){
        err(3, "Could not dup!");
    }
    if(unlink("fifo") < 0){
        err(4, "Error on unlink!");
    }
    argv++;
    execvp(argv[0], argv);
    err(5, "Could not exec!");
    return 0;
}
