#include <err.h> // err()
#include <fcntl.h> 
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()

int main(int argc, char* argv[]) {
    if(argc != 2){
        err(1, "Wrong arguments!");
    }

    int pfd[2];
    if(pipe(pfd) < 0){
        err(2, "Could not pipe!");
    }

    int pid = fork();
    if(pid == 0){
        close(pfd[0]);
        if(dup2(pfd[1], 1) < 0){
            err(4, "Could not dup!");
        }

        execlp("cat", "cat", argv[1], NULL);
        err(5, "Could not exec!");
    }
    close(pfd[1]);

    pid = fork();
    if(pid == 0){
        if(dup2(pfd[0], 0) < 0){
            err(4, "Could not dup!");
        }

        execlp("sort", "sort", NULL);
        err(5, "Could not exec!");
    }

    wait(NULL);
    wait(NULL);
    return 0;
}
