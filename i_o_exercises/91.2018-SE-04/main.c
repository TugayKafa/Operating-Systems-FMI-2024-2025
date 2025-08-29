#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>

int wrapper_read(int fd, void * buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(4, "Could not read from file!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buff, int size) {
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(5, "Could not write to file!");
    }
    return wr;
}

int getSizeOfFile(int fd){
    struct stat info;
    if(fstat(fd, &info) < 0){
        err(6, "Could not stat!");
    }
    return info.st_size;
}

int wrapper_lseek(int fd, int offset, int whence){
    int pos = lseek(fd, offset, whence);
    if(pos < 0){
        err(7, "Could not seek!");
    }
    return pos;
}


void sortNumbersInFile(int fd){
    int size = getSizeOfFile(fd)/sizeof(uint16_t);
    for(int i = 0; i < size-1;i++){
        int minIndex = i;
        wrapper_lseek(fd,i*sizeof(uint16_t), SEEK_SET);
        uint16_t cur;
        wrapper_read(fd, &cur, sizeof(cur));
        uint16_t num = cur;
        for(int j = i+1; j<size;j++) {
            uint16_t tmp;
            wrapper_read(fd, &tmp, sizeof(tmp));
            if(tmp < num){
                num = tmp;
                minIndex = j;
            }
        }
        wrapper_lseek(fd,i*sizeof(uint16_t), SEEK_SET);
        wrapper_write(fd, &num, sizeof(num));
        wrapper_lseek(fd, minIndex*sizeof(uint16_t), SEEK_SET);
        wrapper_write(fd, &cur, sizeof(cur));
    }
}

int main(int argc, char* argv[]){
    if (argc != 3){
        err(1, "Wrong arguments!");
    }

    int fd_in = open(argv[1], O_RDONLY);
    if(fd_in < 0) {
        err(2, "Could not open input file!");
    }
    int fd_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(fd_out < 0) {
        err(3, "Could not open output file!");
    }

    uint16_t num;
    while(wrapper_read(fd_in, &num, sizeof(num))){
        wrapper_write(fd_out, &num, sizeof(num));
    }
    
    sortNumbersInFile(fd_out);

    close(fd_in);
    close(fd_out);
    return 0;
}
