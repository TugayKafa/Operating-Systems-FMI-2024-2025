#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

typedef struct {
    uint32_t pos;
    uint32_t count;
} pair_t;

int main(int argc, char *argv[]) {
    if(argc != 3){
     err(1,"You need to have 2 arguments");
    }
    
    int fd1 = open( argv[1], O_RDONLY );
    if(fd1 < 0){
        err(2, "Could not open file with instructions!");
    }
    
    struct stat info_fd1;
    int stat_fd1 = fstat(fd1, &info_fd1);
    if(stat_fd1 < 0) {
        err(3, "Could not stat file with instructions!");       
    }

    int fd1_pairs = info_fd1.st_size / sizeof(pair_t);

    int fd2 = open( argv[2], O_RDONLY);
    if(fd2 < 0){
        err(2, "Could not open file with data!");
    }

    struct stat info_fd2;
    int stat_fd2 = fstat(fd2, &info_fd2);
    if(stat_fd2 < 0) {
        err(3, "Could not stat file with data!");
    }
    
    uint32_t fd2_elements = info_fd2.st_size / sizeof(uint32_t);

    int fd3 = open("f3", O_CREAT | O_RDWR | O_TRUNC);
    if(fd3 < 0) {
        err(2, "Could not open result file!");
    }

    pair_t pair;
    for(int i = 0; i < fd1_pairs; i++) {
        int readBytes = read(fd1, &pair, sizeof(pair_t));
        if(readBytes < 0) {
            err(4, "Could not read from file!");
        }
        
        if(fd2_elements <= pair.pos || fd2_elements < pair.pos+pair.count-1) {
            continue;
        }

        int pos = lseek(fd2, pair.pos*sizeof(uint32_t) , SEEK_SET);
        if(pos < 0){
            err(5, "Could not seek in file!");
        }


        for(uint32_t k = 1; k <= pair.count; k++){
            uint32_t num;
            
            int readBts = read(fd2, &num, sizeof(uint32_t));
            if(readBts < 0) {
                err(4, "Could not read from file!");
            }

            int writeBytes = write(fd3, &num, sizeof(uint32_t));
            if(writeBytes < 0) {
                err(5, "Could not write number to file!");
            }
        }
    }

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}

