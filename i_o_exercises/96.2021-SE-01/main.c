#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

int wrapper_read(int fd, void* buffer, int size){
    int rd = read(fd, buffer, size);
    if(rd < 0){
        err(4, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buffer, int size){
    int wr = write(fd, buffer, size);
    if(wr < 0){
        err(5, "Could not write!");
    }
    return wr;
}

int main(int argc, char* argv[]) {
    if(argc != 3){
        err(1, "Wrong arguments!");
    }

    int fd_i = open(argv[1], O_RDONLY);
    if(fd_i < 0){
        err(2, "Could not open input file!");
    }

    int fd_o = open(argv[2], O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(fd_o < 0){
        err(3, "Could not open output file!");
    }

    uint8_t byte;
    while(wrapper_read(fd_i, &byte, 1) == 1){
           
        uint16_t enc = 0;
        for (int i = 7; i >= 0; i--) {
            int b = (byte >> i) & 1;
            enc <<= 2;
            enc |= b ? 2 : 1;      
        }

        uint8_t hi = (enc >> 8) & 0xFF;
        uint8_t lo = enc & 0xFF;
        wrapper_write(fd_o, &hi, 1);
        wrapper_write(fd_o, &lo, 1);
    }
    
    close(fd_i);
    close(fd_o);
    return 0;
}
