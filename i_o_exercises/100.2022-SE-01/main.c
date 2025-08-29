#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>
#include <stdio.h>

int wrapper_read(int fd, void* buffer, int size){
    int rd = read(fd, buffer, size);
    if(rd < 0 || rd != size){
        err(3, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buffer, int size){
    int wr = write(fd, buffer, size);
    if(wr < 0 || wr != size){
        err(4, "Could not write!");
    }
    return wr;
}

int wrapper_lseek(int fd, int offset, int whence){
    int seek = lseek(fd, offset, whence);
    if(seek < 0){
        err(5, "Could not lseek!");
    }
    return seek;
}

typedef struct {
    uint32_t magic;
    uint32_t count;
} header1_t;

typedef struct {
    uint32_t magic1;
    uint16_t magic2;
    uint16_t reserved;
    uint64_t count;
} header2_t;

typedef struct {
    uint16_t type;
    uint16_t reserved[3];
    uint32_t offset1;
    uint32_t offset2;
} compare_t;

int checkCompare(compare_t cmp){
    if(cmp.type > 1){
        return 0;
    }

    for(int i = 0; i<3; i++){
        if(cmp.reserved[i] != 0){
            return 0;
        }
    }

    return 1;
}

int isNeedToSwap(int type, uint64_t num1, uint64_t num2){
    if(type){
        if(num1 < num2) return 1;
    } 
    else if(num1 > num2) return 1;

    return 0;
}

int main(int argc, char* argv[]) {
    if(argc != 3){
        err(1, "Wrong arguments!");
    }

    int fd_d = open(argv[1], O_RDWR);
    if(fd_d < 0){
        err(2, "Could not open data file!");
    }

    header1_t header1;
    if(wrapper_read(fd_d, &header1, sizeof(header1)) != 8){
        err(6, "Could not read header of data file!");
    }

    if(header1.magic != 0x21796F4A){
        err(7, "Data file does not follow specification!");
    }
    
    int fd_c = open(argv[2], O_RDONLY);
    if(fd_c < 0){
        err(2, "Could not open comparator file!");
    }

    header2_t header2;
    if(wrapper_read(fd_c, &header2, sizeof(header2)) != 16){
        err(6, "Could not read header of comparator file!");
    }

    if(header2.magic1 != 0xAFBC7A37 || header2.magic2 != 0x1C27){
        err(7, "Comparator file does not follow specification!");
    }

    for(uint64_t i = 0; i < header2.count; i++){
        compare_t cmp;
        wrapper_read(fd_c, &cmp, sizeof(cmp));
        if(!checkCompare(cmp)){
            continue;
        }

        if(cmp.offset1 >= header1.count || cmp.offset2 >= header1.count){
            continue;
        }
    
        uint64_t num1;
        uint64_t num2;
        wrapper_lseek(fd_d, 8+cmp.offset1*sizeof(uint64_t), SEEK_SET);
        wrapper_read(fd_d, &num1, sizeof(num1));
        wrapper_lseek(fd_d, 8+cmp.offset2*sizeof(uint64_t), SEEK_SET);
        wrapper_read(fd_d, &num2, sizeof(num2));

        if(isNeedToSwap(cmp.type, num1, num2)){
            wrapper_lseek(fd_d, 8+cmp.offset1*sizeof(uint64_t), SEEK_SET);
            wrapper_write(fd_d, &num2, sizeof(num2));
            wrapper_lseek(fd_d, 8+cmp.offset2*sizeof(uint64_t), SEEK_SET);
            wrapper_write(fd_d, &num1, sizeof(num1));
        }
    }

    close(fd_d);
    close(fd_c);
    return 0;
}
