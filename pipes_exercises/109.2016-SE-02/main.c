#include <err.h> // err()
#include <string.h>
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>

int main(void) {

    char buffer[256];

    while(1) {
        write(1, "prompt> ", 8);

        int readBytes = read(0, buffer, sizeof(buffer) - 1);
        if(readBytes < 0) {
            err(1, "Could read from prompt!");
        }

        for(int i = 0; i < readBytes; i++) {
            if(buffer[i] == '\n') {
                buffer[i] = '\0';
                break;
            }
        }
        
        if(!strcmp(buffer, "exit")) {
            break;
        }

        pid_t pid = fork();
        if(pid < 0) {
            err(2, "Could not fork!");
        }

        if(pid == 0) {
            execlp(buffer,buffer,NULL);
            err(3, "Could not exec!");
        }
        else {
            int status;
            wait(&status);
        }
    } 
    
	return 0;
}
