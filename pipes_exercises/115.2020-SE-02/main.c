#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void isForkSuccessfull(int pid){
    if(pid<0){
        err(2, "Could not fork!");
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3){
        err(1, "Wrong arguments!");
    }

    int pipe1[2], pipe2[2];
    if(pipe(pipe1) < 0 || pipe(pipe2) < 0){
        err(3, "Could not pipe!");
    }

    pid_t pid = fork();
    isForkSuccessfull(pid);
    if(pid==0){
        int dp = dup2(pipe1[1], 1);
        if(dp < 0){
            err(4, "Could not dup2!");
        }
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);

        execlp("cat", "cat", argv[1], NULL);
        err(5, "Could not exec cat!");
    }


    pid = fork();
    isForkSuccessfull(pid);
    if(pid == 0){
        int dp1 = dup2(pipe1[0], 0);
        int dp2 = dup2(pipe2[1], 1);
        if(dp1 < 0 || dp2 < 0){
            err(6, "Could not dup2 dp1 or dp2!");
        }
       
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);

        execlp("sed", "sed", "s/U//g", NULL);
        err(7, "Could not sed!");
    }
    int dp = dup2(pipe2[0], 0);
    if(dp<0){
        err(4, "Could not dup2!");
    }
    close(pipe1[0]); close(pipe1[1]);
    close(pipe2[0]); close(pipe2[1]);

    int fd = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0644);
    if(fd < 0){
        err(9, "Could not open output file!");
    }

    char a;
    while(read(0, &a, sizeof(char)) == 1){
        if(a==0x7D){
            if(read(0, &a, sizeof(char))!=1){
                break;
            }
            a ^= 0x20;
        }
        if(write(fd, &a, sizeof(char))!=1){
           err(8, "Could not write!");
        }
    }

    wait(NULL);
    wait(NULL);

    return 0;
}
