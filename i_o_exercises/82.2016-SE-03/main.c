#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

void insertionSort(int fd, int numbers){
    for(int i = 1; i < numbers; i++){
         int seek = lseek(fd, i*sizeof(uint32_t), SEEK_SET);
         if(seek < 0){
            err(5, "Could not seek");
        }
        uint32_t key;
        int readKey = read(fd, &key, sizeof(uint32_t));
        if(readKey < 0){
            err(6, "Could not read num! - 1");
        }
        int j = i-1; 
        while(j>=0){
            seek = lseek(fd, j*sizeof(uint32_t), SEEK_SET);
            if(seek < 0){
                err(5, "Could not seek");
            }
            uint32_t num;
            int readNum = read(fd, &num, sizeof(uint32_t));
            if(readNum < 0){
                err(6, "Could not read num! - 2");
            }
            
            if(num<=key){
                break;
            }
            seek = lseek(fd, (j+1)*sizeof(uint32_t), SEEK_SET);
            if(seek < 0){
                err(6, "Could not seek");
            } 
            int writeNum = write(fd, &num, sizeof(uint32_t));
            if(writeNum < 0){
                err(7, "Could not write number 1 ");
            }
            j=j-1;
        }
        seek = lseek(fd, (j+1)*sizeof(uint32_t), SEEK_SET );
        if(seek < 0){
            err(5, "Could not seek!");
        }
        int writeKey = write(fd, &key, sizeof(uint32_t));
        if(writeKey < 0){
            err(7,"Could not write number 2");
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2){
     err(1,"You need to have 1 argument");
    }
    
    int fd1 = open( argv[1], O_RDWR);
    if(fd1 < 0){
        err(2, "Could not open file with instructions!");
    }
    
    struct stat info_fd1;
    int stat_fd1 = fstat(fd1, &info_fd1);
    if(stat_fd1 < 0) {
        err(3, "Could not stat file with instructions!");       
    }

    int numbers = info_fd1.st_size / sizeof(uint32_t);

    insertionSort(fd1,numbers);    
    
    close(fd1);
    return 0;
}

