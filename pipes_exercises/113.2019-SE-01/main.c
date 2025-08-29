#include <err.h> // err()
#include <string.h>
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if(argc < 3){
        err(1, "Wrong arguments!");
    }

    int dur = strtol(argv[1], NULL, 10);

    if(dur < 1 || dur > 9){
        err(2, "Duration is not in correct format!");
    }

    int fd = open("run.log", O_CREAT | O_RDWR | O_TRUNC, 0644);

    if(fd < 0){
        err(3, "File could not open!");
    }

    int counter = 0;

    while(1){
        int pid = fork();
        int beg = time(NULL);
    
        if(pid < 0){
            err(4, "Could not fork!");
        }

        if(pid == 0){
            char** params = argv + 2;
            execvp(argv[2], params);
            err(5, "Could not exec!");
        }

        int end = time(NULL);

        int status;
        int code;

        if(wait(&status) < 0){
            err(6, "Could not wait!");
        }
        
        if(!WIFEXITED(status)){
            code = 129;
        }
        else {
            code = WEXITSTATUS(status);
        }

        char buff[256];
        if(snprintf(buff, 256, "%d %d %d \n", beg, end, code) < 0){
            err(7, "Could not write to buffer!");
        }

        if(write(fd, &buff, strlen(buff)) != (ssize_t) strlen(buff)){
            err(8, "Could not write to file correctly!");
        }

        if(end - beg < dur && code != 0){
            counter++;
        }
        else {
            counter = 0;
        }
    
        if(counter == 2){
            break;
        }
    }

    close(fd);
    return 0;
}
