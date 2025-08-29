#include <err.h> // err()
#include <string.h>
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if(argc > 2){
        err(1, "Too many arguments!");
    }
    
    char command[5] = "echo";
    if(argc == 2){
        if(strlen(argv[1]) > 4){
            err(2, "Command is not supported");  
        }
        strcpy(command, argv[1]);
    }


    char w1[5], w2[5];
    char word[5];
    int word_len = 0;
    char* words[2] = {w1, w2};
    int count = 0;


    char buffer[4096];
    int bytes;
    while((bytes=read(0, &buffer, sizeof(buffer))) > 0){
         for(int i = 0; i < bytes; i++){
            if(buffer[i] == ' ' || buffer[i] == '\n'){
                if(word_len == 0) {
                    continue;
                }
                
                word[word_len] = '\0';
                strcpy(words[count++], word);
                word_len = 0;
                if(count == 2){
                    count = 0;
                    
                    pid_t pid = fork();
                    if(pid == 0){
                        execlp(command, command, words[0], words[1], NULL );
                        err(4, "Could not exec!");
                    }
                    else{
                         wait(NULL);
                    }
                    count = 0;
                }
            } 
            else {
                if(word_len >= 4){
                    err(3, "Invalid argument!");
                }
                word[word_len++] = buffer[i];
            }
        }
    }

   if(count > 0){
        pid_t pid = fork();
        if(pid == 0){
            execlp(command, command, words[0], NULL);
            err(4, "Could not exec");
        }
        else {
            wait(NULL);
        }
    } 

    return 0;
}
