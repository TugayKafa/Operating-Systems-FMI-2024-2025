#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>
#include <stdio.h>

typedef struct {
    uint16_t magic;
    uint16_t filetype;
    uint32_t count;
} header_t;

int wrapper_read(int fd, void* buffer, int size){
    int rd = read(fd, buffer, size);
    if(rd < 0){
        err(3, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buffer, int size){
    int wr = write(fd, buffer, size);
    if(wr < 0){
        err(4, "Could not write!");
    }
    return wr;
}

int wrapper_lseek(int fd, int offset, int whence){
    int seek = lseek(fd, offset, whence);
    if(seek < 0){
        err(6, "Could not seek!");
    }
    return seek;
}

int main(int argc, char* argv[]) {
    if(argc != 4){
        err(1, "Wrong arguments!");
    }
    
    int fd_l = open(argv[1], O_RDONLY);
    if(fd_l < 0){
        err(2, "Could not open file!");
    }

    header_t header_l;
    wrapper_read(fd_l, &header_l, sizeof(header_l));
    if(header_l.magic != 0x5A4D){
        err(5, "Not Hoge file!");
    }
    if(header_l.filetype != 1){
        err(6, "Not list file! First argument must be list.bin!");
    }

    int fd_d = open(argv[2], O_RDONLY);
    if(fd_d < 0){
        err(2, "Could not open file!");
    }

    header_t header_d;
    wrapper_read(fd_d, &header_d, sizeof(header_d));
    if(header_d.magic != 0x5A4D){
        err(5, "Not Hoge file!");
    }
    if(header_d.filetype != 2){
        err(6, "Not data file! Second argument must be data.bin!");
    }

    int fd_o = open(argv[3], O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(fd_o < 0){
        err(2, "Could not open file!");
    }

    wrapper_lseek(fd_o, 8, SEEK_SET);
    for(uint32_t i = 0; i < header_d.count; i++){
        uint32_t num;
        wrapper_read(fd_d, &num, sizeof(num));
        uint64_t res_num = (uint64_t) num;
        wrapper_write(fd_o, &res_num, sizeof(res_num));
    }

    for(uint32_t i = 0; i< header_l.count; i++){
        uint16_t pos;
        wrapper_read(fd_l, &pos, sizeof(pos));
        
        int from = i;
        int to = pos;
        
        uint64_t num;
        wrapper_lseek(fd_o, 8 + from*sizeof(uint64_t), SEEK_SET);
        wrapper_read(fd_o, &num, sizeof(num));
        wrapper_lseek(fd_o, 8 + from*sizeof(uint64_t), SEEK_SET);
        uint64_t zero = 0;
        wrapper_write(fd_o, &zero, sizeof(zero));
        wrapper_lseek(fd_o, 8 + to*sizeof(uint64_t), SEEK_SET);
        wrapper_write(fd_o, &num, sizeof(num));
    }

    header_t header_o;
    header_o.magic = 0x5A4D;
    header_o.filetype = 3;
    header_o.count = header_d.count;

    wrapper_lseek(fd_o, 0, SEEK_SET);
    wrapper_write(fd_o, &header_o, sizeof(header_o));

    close(fd_l);
    close(fd_d);
    close(fd_o);
    return 0;
}
