#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>

void writeChar(char a){
    if(write(1, &a, 1) != 1){
        err(2, "Could not write!");
    }
}

void deleteCharacters(char* str){
    int len = strlen(str);
    if(len < 0){
        err(1, "Could not strlen!");
    }

    char a;
    while(read(0,&a,sizeof(char))==1){
        int isForbiden = 0;
        for(int i = 0; i < len; i++){
            if(str[i]==a){
                isForbiden=1;
                continue;
            }
        }

        if(isForbiden){
            continue;
        }
        
        writeChar(a);
    }
}

void squizeCharacters(char* str){
    int len = strlen(str);
    if(len < 0){
        err(1, "Could not strlen!");
    }

    char a;
    char b;
    int isSquized = 0;
    while(read(0,&a,sizeof(char))==1){
        if(isSquized){
            if(a!=b){
                writeChar(b);
            }
            isSquized=0;
        }

        int isSquizable = 0;
        for(int i = 0; i < len; i++){
            if(str[i]==a){
                isSquizable=1;
                continue;
            }
        }

        if(isSquizable){
            int rd;
            while((rd=read(0, &b, sizeof(char)))==1 && a==b);
            if(rd != 1) {
               writeChar(b);
               return;
            }
            
            isSquized = 1;
        }

        writeChar(a);
    }

    if(isSquized){
        writeChar(b);
    }
}

void translateCharacters(char* from, char* to){
    int len_from = strlen(from);
    if(len_from < 0){
        err(1, "Could not strlen!");
    }

    int len_to = strlen(to);
    if(len_to < 0){
        err(1, "Could not strlen!");
    }

    if(len_to != len_from){
        err(2, "Not translateable charachters!");
    }

    char a;
    while(read(0,&a,sizeof(char))==1){
        int isTranslated = 0;
        for(int i = 0; i < len_from; i++){
            if(from[i]==a){
                writeChar(to[i]);
                isTranslated = 1;
                break;
            }
        }

        if(isTranslated){
            continue;
        }

        writeChar(a);
    }
}

void process(char* argv[]){
     if (!strcmp(argv[1], "-d")){
        deleteCharacters(argv[2]);
    }
    else if(!strcmp(argv[1], "-s")){
        squizeCharacters(argv[2]);
    }
    else {
        translateCharacters(argv[1], argv[2]);    
    }
}

int main(int argc, char* argv[]) {
    if(argc < 3){
        err(1, "Not enouth arguments!");
    }

    process(argv);

    return 0;
}
