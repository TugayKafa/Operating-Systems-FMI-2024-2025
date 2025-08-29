#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

int wrapper_read(int fd, void* buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(5, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0) {
        err(10, "Could not write!");
    }
    return wr;
}

int main(int argc, char* argv[]) {
    if(argc != 4) {
        err(1, "Wrong arguments!");
    }

    int fd = open(argv[3], O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(fd < 0){
        err(2, "Could not open result file!");
    }
    
    int fd_null = open("/dev/null", O_WRONLY);
    if(fd_null < 0){
        err(11, "Could not open!");
    }

    int N = atoi(argv[2]);
    if(N < 0 || N >= 256) {
        err(3, "N must be between 0 and 256!");
    }

    int fd_buf = open("/dev/urandom", O_RDONLY);
    if(fd_buf < 0){
        err(4, "Could not open /dev/urandom!");
    }

    for(int i = 1; i <= N; i++){
        int rand = random() % 65536;
        uint8_t buffer[65536] = {0};
        int rd = wrapper_read(fd_buf, buffer, rand);
        if(rd != rand){
            err(6, "Could not read from /dev/urandom!");
        }
        
        int pipe1[2];
        if(pipe(pipe1) < 0){
            err(7, "Could not pipe!");
        }

        int pid = fork();
        if(pid == 0){
            close(pipe1[1]);
            if(dup2(pipe1[0], 0) < 0){
                err(8, "Could not dup2!");
            }

            if(dup2(fd_null, 1) < 0){
                err(8, "Could not dup2!");
            }

            if(dup2(fd_null,2) < 0){
                err(8, "Could not dup2!");
            }

            close(pipe1[0]);
            close(fd_null);
            execlp(argv[1], argv[1], NULL);
        }

        close(pipe1[0]);
        wrapper_write(pipe1[1], buffer, rand);
        close(pipe1[1]);

        int status;
        if(wait(&status) < 0){
            err(9, "Could not wait!");
        }
            
        if(WIFEXITED(status)){
            continue;
        } else if(WIFSIGNALED(status)){
            wrapper_write(fd, buffer, rand);
            close(fd);
            close(fd_null);
            close(fd_buf);
            return 42;
        }
    }

    close(fd);
    close(fd_null);
    close(fd_buf);
    return 0;
}
