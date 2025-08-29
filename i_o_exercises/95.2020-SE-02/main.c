#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

int wrapper_read(int fd, void* buff,int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(7, "Could not read!");
    }

    return rd;
}

int wrapper_write(int fd, const void* buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(8, "Could not write!");
    }
    return wr;
}

int wrapper_lseek(int fd, int pos, int whence){
    int seek = lseek(fd, pos, whence);
    if(seek < 0){
        err(9, "Could not seek!");
    }

    return seek;
}

void sort_result(int fd_res){
    struct stat info_res;
    if(fstat(fd_res, &info_res) < 0){
        err(5, "Could not stat scl file!");
    }

    uint16_t size = info_res.st_size / sizeof(uint16_t);

    for(int i = 1; i < size; size++){
        wrapper_lseek(fd_res, i*2, SEEK_SET);
        uint16_t key;
        wrapper_read(fd_res, &key, sizeof(key));

        int j = i - 1;
        while(j >= 0){
            wrapper_lseek(fd_res, j*2, SEEK_SET);
            uint16_t num;
            wrapper_read(fd_res, &num, sizeof(num));
            if(num > key){
                wrapper_write(fd_res, &num, sizeof(num));
            }
            else {
                break;
            }
            j--;
        }
        wrapper_write(fd_res, &key, sizeof(key));
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3){
        err(1, "Wrong arguments!");
    }

    int fd_scl = open(argv[1], O_RDONLY);
    if(fd_scl < 0){
        err(2, "Could not open scl file!");
    }

    int fd_sdl = open(argv[2], O_RDONLY);
    if(fd_sdl < 0){
        err(3, "Could not open sdl file!");
    }

    int fd_res = open("result.sdl", O_RDWR | O_TRUNC | O_CREAT, 0666);
    if(fd_res < 0){
        err(4, "Could not create result.sdl file!");
    }

    struct stat info_sdl;
    if(fstat(fd_sdl, &info_sdl) < 0){
        err(6, "Could not stat sdl file!");
    }

    int sdl_size = info_sdl.st_size / sizeof(uint16_t);
    for(int i = 0; i < sdl_size; i++){
        uint16_t pos;
        wrapper_read(fd_sdl, &pos, sizeof(pos));

        uint16_t bucket = pos/8;
        uint16_t ostatik = pos%8;
        uint8_t byte;
        if(ostatik == 0){
            wrapper_lseek(fd_scl, bucket-1, SEEK_SET);
            ostatik=8;
        }
        else{
            wrapper_lseek(fd_scl, bucket, SEEK_SET);
        }
        wrapper_read(fd_scl, &byte, 1);
        
        uint8_t mask = 1 << (8-ostatik);
        uint8_t res = byte & mask;
        if(res != 0){
            wrapper_write(fd_res, &pos, sizeof(pos));
        }
    }

    sort_result(fd_res);

    close(fd_scl);
    close(fd_sdl);
    close(fd_res);

    return 0;
}
