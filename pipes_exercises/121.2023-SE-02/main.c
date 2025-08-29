#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

void wrapper_pipe(int pipe1[]){
    if(pipe(pipe1) < 0){
        err(26,"Could not pipe!");
    }
}

int wrapper_fork(void){
    int pid = fork();
    if(pid < 0){
        err(26, "Could not fork!");
    }
    return pid;
}

int wrapper_wait(void){
    int status;
    if(wait(&status) < 0){
        err(26, "Could not wait for process to finish!");
    }
    if(!WIFEXITED(status)){
        //
    }
    if(WEXITSTATUS(status) != 0){
        //
    }
    return status;
}

int wrapper_read(int fd, void* buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(26, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(26, "Could not write!");
    }
    return wr;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        err(26, "Wrong arguments!");
    }

    int pids[1000];
    int pipe1[2];
    wrapper_pipe(pipe1);

    for(int i = 1; i<argc;i++){
        int pid = wrapper_fork();
        pids[i-1] = pid;
        if(pid == 0){
            close(pipe1[0]);
            dup2(pipe1[1], 1);
            close(pipe1[1]);
            execlp(argv[i],argv[i], NULL);
            err(26, "Could not execute!");
        }
    }
    close(pipe1[1]);

    char buff[1024] = {0};
    char ch;
    int idx = 0;
    while(wrapper_read(pipe1[0], &ch, sizeof(ch)) == sizeof(ch)){
        if(ch=='!'){
            buff[idx] = '\0';
            if(!strcmp(buff, "found it")){
                for(int i = 0; i<argc-1;i++){
                    kill(pids[i], SIGTERM);
                }
                for(int i = 0; i<argc-1;i++){
                    waitpid(pids[i], NULL, 0);
                }
                exit(0);
            }
            idx = 0;
            continue;
        }
        buff[idx] = ch;
        idx++;
    }

    close(pipe1[0]);
    for(int i=1;i<argc;i++){
        wrapper_wait();
    }
    exit(1);
}
