#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

pid_t pids[10];
int done[10] = {0};

int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 11){
        err(11, "Wrong arguments!");
    }

    for(int i = 1; i <= argc; i++){
        int pid = fork();
        if(pid < 0){
            err(13, "Could not fork!");
        }
        pids[i-1] = pid;
        if(pid == 0){
            execlp(argv[i], argv[i], NULL);
            err(14, "Could not exec!");
        }
    }

    int finished = 0;
    while(finished < argc - 1){
        int status;
        pid_t pid = wait(&status);
        if(pid < 0){
            err(15, "Could not wait!");
        }

        int index = -1;
        for(int i = 0; i < argc - 1; i++){
            if(pids[i] == pid){
                index = i;
            }
        }

        if(index < 0){
            continue;
        }
    
        if(WIFEXITED(status)){
            int code = WEXITSTATUS(status);
            if(code == 0){
                finished++;
                done[index] = 1;
            } else {
                pid_t ppid = fork();
                if(ppid < 0){
                    err(13, "Could not fork!");
                }
                pids[index] = ppid;
                if(ppid == 0){
                    execlp(argv[index + 1], argv[index + 1], NULL);
                    err(14, "Could not exec!");
                }
            }
        } else if(WIFSIGNALED(status)) {
            for(int j = 0; j < argc -1;j++){
                if(j==index) continue;
                if(!done[j] && pids[j] > 0){
                    kill(pids[j], SIGTERM);
                }
            }

            while(wait(NULL) > 0);
            exit(index+1);
        }
    }
    
    return 0;
}
