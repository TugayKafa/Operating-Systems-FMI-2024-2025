#include <err.h> // err()
#include <string.h>
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        err(1, "Wrong arguments!");
    }

    int pfd_find[2];
    if(pipe(pfd_find) < 0){
        err(2, "Could not pipe!");
    }
    int pid = fork();
    if(pid == 0){
        close(pfd_find[0]);
        if(dup2(pfd_find[1], 1) < 0){
            err(3, "Could not dup2!");
        }

        execlp("find", "find", argv[1], "-type", "f", "-printf", "%TD %TT %p \n", NULL);
        err(4, "Could not exec!");
    }
    close(pfd_find[1]);

    int pfd_sort[2];
    if(pipe(pfd_sort) < 0){
        err(2, "Could not pipe!");
    }
    pid = fork();
    if(pid == 0){
        close(pfd_sort[0]);
        if(dup2(pfd_find[0], 0) < 0){
            err(3, "Could not dup2!");
        }
        if(dup2(pfd_sort[1], 1) < 0){
            err(3, "Could not dup2!");
        }

        execlp("sort", "sort", "-nr", NULL);
        err(4, "Could not exec!");
    }
    close(pfd_sort[1]);
    close(pfd_find[0]);

    int pfd_cut[2];
    if(pipe(pfd_cut) < 0){
        err(3, "Could not pipe!");
    }

    pid = fork();
    if(pid == 0){
        close(pfd_cut[0]);
        if(dup2(pfd_sort[0], 0) < 0){
            err(3,"Could not dup2!");
        }
        if(dup2(pfd_cut[1], 1) < 0){
            err(3, "Could not dup2!");
        }

        execlp("cut", "cut", "-d", " ", "-f", "3", NULL);
        err(4, "Could not exec!");
    }
    close(pfd_cut[1]);
    close(pfd_sort[0]);

    pid = fork();
    if(pid==0){
        if(dup2(pfd_cut[0], 0) < 0){
            err(3, "Could not dup2!");
        }

        execlp("head", "head", "-n", "1", NULL);
        err(4, "Could not exec!");
    }
    close(pfd_cut[0]);

    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}

