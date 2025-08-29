#include <err.h> // err()
#include <string.h>
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>

int main(void) {
    int pfd[3][2];
    if(pipe(pfd[0]) < 0){
        err(1, "Could not pipe!");
    }

    int pid = fork();
    if(pid == 0){
        close(pfd[0][0]);
        if(dup2(pfd[0][1], 1) < 0){
            err(2, "Could not dup!");
        }

        execlp("cut", "cut", "-d", ":", "-f", "7", "/etc/passwd", NULL);
        err(3, "Could not exec!");
    }
    close(pfd[0][1]);

    if(pipe(pfd[1]) < 0){
        err(1, "Could not pipe!");
    }

    pid = fork();
    if(pid == 0){
        close(pfd[1][0]);
        if(dup2(pfd[0][0], 0) < 0){
            err(2, "Could not dup!");
        }
        if(dup2(pfd[1][1], 1) < 0){
            err(2, "Could not dup!");
        }

        execlp("sort", "sort", NULL);
        err(3, "Could not exec!");
    }
    close(pfd[1][1]);
    close(pfd[0][0]);

    if(pipe(pfd[2]) < 0){
        err(1, "Could not pipe!");
    }

    pid = fork();
    if(pid == 0){
        close(pfd[2][0]);
        if(dup2(pfd[1][0], 0) < 0){
            err(2, "Could not dup!");
        }
        if(dup2(pfd[2][1], 1) < 0){
            err(2, "Could not dup!");
        }

        execlp("uniq", "uniq", "-c", NULL);
        err(3, "Could not exec!");
    }
    close(pfd[2][1]);
    close(pfd[1][0]);

    pid = fork();
    if(pid == 0){
        if(dup2(pfd[2][0], 0) < 0){
            err(2, "Could not dup!");
        }

        execlp("sort", "sort", "-n", NULL);
        err(3, "Could not exec!");
    }
    close(pfd[2][0]);

    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}
