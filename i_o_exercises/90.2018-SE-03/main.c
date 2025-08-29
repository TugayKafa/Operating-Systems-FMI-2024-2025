#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>

void wrapper_writeChar(char a){
    int wr = write(1, &a, sizeof(a));
    if(wr != 1){
        err(5, "Could not write char!");
    }
}

void cutCharachters(int from, int to){
    char a;
    int index = 1;
    while(read(0, &a, sizeof(a)) == 1){
        if(a=='\n'){
            index=1;
            continue;
        }
        if(index >= from && index<=to){
            wrapper_writeChar(a);
        }
        index++;
    }
}

void cutFields(char delimater, int from, int to){
    char a;
    int index = 1;
    while(read(0, &a, sizeof(a)) == 1){
        if(a=='\n'){
            index=1;
            continue;
        }
        if(index>=from && index<=to){
            wrapper_writeChar(a);
            int isAllWordRead = 0;
            while(read(0, &a, sizeof(a)) == 1){
                if(a=='\n' || a==delimater){
                    if(a=='\n'){
                        index = 1;
                    }
                    isAllWordRead = 1;
                    break;
                }
                wrapper_writeChar(a);
            }
            if(!isAllWordRead){
                break;
            }
        }
        if(a == delimater){
            index++;
        }
        
    }
}

void process(int argc, char* argv[]){
    if(strlen(argv[1]) != 2){
        err(2, "Wrong command!");
    }

    if(argv[1][1] == 'c'){
        if(argc != 3){
            err(3, "You need to define which charachters to be selected!");
        }

        int len = strlen(argv[2]);
        if(len == 1) {
            if(argv[2][0] < '1' || argv[2][0] > '9'){
                err(4, "You must provide positive digit as an argument!");
            }
            int index = argv[2][0] - '0';
            cutCharachters(index, index);
        }
        else if(len == 3){
            if(argv[2][0] < '1' || argv[2][0] > '9'){
                err(4, "You must provide positive digit as an argument!");
            }
            if(argv[2][1] != '-'){
                err(7, "You need to separate digits with -");
            }
            if(argv[2][2] < '1' || argv[2][2] > '9'){
                err(4, "You must provide positive digit as an argument!");
            }
            int from = argv[2][0] - '0';
            int to = argv[2][2] - '0';
            if(to < from) {
                err(8, "Second digit must be grather than the first!");
            }
            cutCharachters(from, to);
        }
        else {
            err(6, "Wrong argument for cutting characters!");
        }
    }
    else if(argv[1][1] == 'd'){
        if(argc != 5){
            err(10, "You need to have delimater and which fields to be selected!");
        }

        char delimater = argv[2][0];
        if(argv[3][1] != 'f'){
            err(11, "Wrong argument!");
        }

        int len = strlen(argv[4]);
        if(len == 1) {
            if(argv[4][0] < '1' || argv[4][0] > '9'){
                err(4, "You must provide positive digit as an argument!");
            }
            int index = argv[4][0] - '0';
            cutFields(delimater,index, index);
        }
        else if(len == 3){
            if(argv[4][0] < '1' || argv[4][0] > '9'){
                err(4, "You must provide positive digit as an argument!");
            }
            if(argv[4][1] != '-'){
                err(7, "You need to separate digits with -");
            }
            if(argv[4][2] < '1' || argv[4][2] > '9'){
                err(4, "You must provide positive digit as an argument!");
            }

            int from = argv[4][0] - '0';
            int to = argv[4][2] - '0';
            if(to < from) {
                err(8, "Second digit must be grather than the first!");
            }
            cutFields(delimater, from, to);
        }
        else {
            err(6, "Wrong argument for cutting characters!");
        }
    }
    else {
        err(9, "Wrong command!");
    }
}

int main(int argc, char* argv[]){
    if (argc < 2){
        err(1, "Wrong arguments!");
    }

    process(argc, argv);

    return 0;
}
