#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>

int N;
int W;
int pipes[8][2];
const char* words[] = {"tic ", "tac ", "toe\n"};

void wrapper_pipe(int pipe1[]) {
    if(pipe(pipe1) < 0){
        err(4, "Could not pipe!");
    }
}

int wrapper_fork(void){
    int pid = fork();
    if(pid < 0) {
        err(5, "Could not fork!");
    } 
    return pid;
}

int wrapper_wait(void) {
    int status;
    if(wait(&status) < 0){
        err(8, "Could not wait!");
    }
    if(!WIFEXITED(status)){
        err(9, "child was killed!");
    }
    if(WEXITSTATUS(status) != 0){
        err(10, "Error exit status different than 0!");
    }
    return status;
}

int wrapper_read(int fd, void* buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(6, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(7, "Could not write!");
    }
    return wr;
}

void closePipes(int write, int read){
    for(int i =0; i<N+1;i++){
        if(i!=write){
            close(pipes[i][1]);
        }
        if(i!=read){
            close(pipes[i][0]);
        }
    }
}

void runChild(int next) {
    closePipes((next+1)%(N+1), next);

    int idx;
    while(wrapper_read(pipes[next][0],&idx, sizeof(idx)) == sizeof(idx)){
        
        if(idx >= W){
            break;
        }

        wrapper_write(1, words[idx%3], 4);
        idx++;
        wrapper_write(pipes[(next+1)%(N+1)][1], &idx, sizeof(idx));
    }
    close(pipes[next][1]);
    close(pipes[(next+1)%(N+1)][1]);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        err(1, "Wrong arguments!");
    }
     
    N = atoi(argv[1]);
    W = atoi(argv[2]);
    if(N < 1 || N > 7) {
        err(2, "Wrong first argument - You must provide positive number between 1-7!");
    }
    if(W < 1 || W > 35) {
        err(3, "Wrong second argument - You must provide positive number between 1-35");
    }

    for(int i = 0;i<N+1;i++) {
        wrapper_pipe(pipes[i]);
    }

    for(int i = 0; i < N; i++){
        int pid = wrapper_fork();
        if(pid == 0){
            runChild(i);
            exit(0);
        }
    }

    int idx = 0;
    wrapper_write(1, words[idx], 4);
    idx++;
    wrapper_write(pipes[0][1], &idx, sizeof(idx));
    runChild(N);

    for(int i = 0; i< N; i++){
        wrapper_wait();
    }

    return 0;
}
