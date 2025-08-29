#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int wrapper_fork(void){
    int pid = fork();
    if(pid < 0){
        err(2, "Could not fork!");
    }
    return pid;
}

void wrapper_pipe(int p[]){
    if(pipe(p) < 0){
        err(3, "Could not pipe!");
    }
}

int wrapper_wait(void){
    int status;
    if(wait(&status) < 0){
        err(5, "Could wait for process!");
    }
    if(!WIFEXITED(status)){
        err(6, "Child was killed!");
    }
    if(WEXITSTATUS(status) != 0){
        err(7, "Error exit status different than 0!");
    }
    return status;
}

int wrapper_read(int fd, void* buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(8, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(9, "Could not write!");
    }
    return wr;
}

int main(int argc, char* argv[]) {
    if(argc != 2){
        err(1, "wrong arguments!");
    }

    int pipe1[2];
    wrapper_pipe(pipe1);
    int pid = wrapper_fork();

    if(pid == 0){
        close(pipe1[0]);
        dup2(pipe1[1], 1);
        execlp("find", "find", argv[1], "-type", "f", "!", "-name", "*.hash", NULL);
        err(4,"Could not execute find command!");
    }
    
    int child_count = 0;
    close(pipe1[1]);
    while(1){
        char buff[1024] = {0};
        int idx=0;
        char ch;
        int bytes;
        while((bytes=wrapper_read(pipe1[0], &ch, sizeof(ch))) == sizeof(ch)){
            if(ch=='\n' || ch==' ') {
                break;
            }
            buff[idx] = ch;
            idx++;
        }
        
        if(bytes!=sizeof(ch)){
            break;
        }
        
        int pid_ch = wrapper_fork();
        child_count++;
        if(pid_ch == 0){
            close(pipe1[0]);
            char newFileName[1024];
            strcpy(newFileName, buff);
            strcat(newFileName, ".hash");
            int tmp = open(newFileName, O_CREAT | O_TRUNC | O_RDWR, 0644);
            if(tmp < 0){
                err(11, "Could not create new file!");
            }
            dup2(tmp, 1);
            execlp("md5sum", "md5sum", buff, NULL);
            err(10,"Could not hash file!");
        }
    }

    for(int i=1;i<=child_count+1;i++){
        wrapper_wait();
    }
    close(pipe1[0]);
    return 0;
}
