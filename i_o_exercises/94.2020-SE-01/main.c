#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

int wrapper_read(int fd, void* buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(3, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(5, "Could not write!");
    }
    return wr;
}

int wrapper_lseek(int fd, int offset, int whence){
    int seek = lseek(fd, offset, whence);
    if(seek < 0){
        err(4, "Could not seek!");
    }
    return seek;
}

typedef struct {
    uint16_t header[8];
} header_t;

uint16_t readCount(int fd){
    uint16_t count;
    header_t header;
    wrapper_read(fd, &header, sizeof(header_t));
    count=header.header[2];
    return count;
}

void is_open(int fd){
    if(fd < 0){
        err(3, "File could not open!");
    }
}

typedef struct {
    uint16_t start;
    uint16_t count;
} interval_t;

typedef struct {
    interval_t post;
    interval_t pre;
    interval_t in;
    interval_t suf;
} complect_t;

void write_infix(int fd_in, int in_count ,interval_t interval, int fd_cruc){
    if(interval.start > in_count){
        return;
    }
    if(interval.start + interval.count - 1 > in_count){
        interval.count=in_count - interval.start + 1;
    }

    wrapper_lseek(fd_in, (interval.start - 1)*sizeof(uint16_t), SEEK_SET);
    for(int i = 0; i < interval.count; i++){
        uint16_t num;
        if(wrapper_read(fd_in, &num, sizeof(num)) != sizeof(num)){
            err(5, "Could not read proparly!");
        }
        wrapper_write(fd_cruc, &num, sizeof(num));
    }
}

void write_prefix(int fd_pre, int pre_count, interval_t interval, int fd_cruc){
    if(interval.start > pre_count){
        return;
    }
    if(interval.start + interval.count - 1 > pre_count){
        interval.count=pre_count - interval.start + 1;
    }

    wrapper_lseek(fd_pre, (interval.start - 1)*sizeof(uint8_t), SEEK_SET);
    for(int i = 0; i < interval.count; i++){
        uint8_t num;
        if(wrapper_read(fd_pre, &num, sizeof(num)) != sizeof(num)){
            err(5, "Could not read proparly!");
        }
        wrapper_write(fd_cruc, &num, sizeof(num));
    }
}

void write_postfix(int fd_post, int post_count, interval_t interval, int fd_cruc){
    if(interval.start > post_count){
        return;
    }
    if(interval.start + interval.count - 1 > post_count){
        interval.count=post_count - interval.start + 1;
    }

    wrapper_lseek(fd_post, (interval.start - 1)*sizeof(uint32_t), SEEK_SET);
    for(int i = 0; i < interval.count; i++){
        uint32_t num;
        if(wrapper_read(fd_post, &num, sizeof(num)) != sizeof(num)){
            err(5, "Could not read proparly!");
        }
        wrapper_write(fd_cruc, &num, sizeof(num));
    }
}

void write_suffix(int fd_suf, int suf_count , interval_t interval, int fd_cruc){
    if(interval.start > suf_count){
        return;
    }
    if(interval.start + interval.count - 1 > suf_count){
        interval.count=suf_count - interval.start + 1;
    }

    wrapper_lseek(fd_suf, (interval.start - 1)*sizeof(uint64_t), SEEK_SET);
    for(int i = 0; i < interval.count; i++){
        uint64_t num;
        if(wrapper_read(fd_suf, &num, sizeof(num)) != sizeof(num)){
            err(5, "Could not read proparly!");
        }
        wrapper_write(fd_cruc, &num, sizeof(num));
    }
}

void makeHeader(int fd_cruc){
    wrapper_lseek(fd_cruc, 0, SEEK_SET);
    uint16_t count = 0;
    uint8_t byte;
    while(wrapper_read(fd_cruc, &byte, 1) == 1){
        count++;
    }

    header_t header;
    header.header[2] = count;
    wrapper_lseek(fd_cruc, 0, SEEK_SET);
    wrapper_write(fd_cruc, &header, sizeof(header_t));
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        err(1, "You need to provide 6 arguments!");
    }

    int fd_af = open(argv[1], O_RDONLY);
    is_open(fd_af);
    uint16_t affix_count = readCount(fd_af);
    
    int fd_post = open(argv[2], O_RDONLY);
    is_open(fd_post);
    uint16_t post_count = readCount(fd_post);

    int fd_pre = open(argv[3], O_RDONLY);
    is_open(fd_pre);
    uint16_t pre_count = readCount(fd_pre);

    int fd_in = open(argv[4], O_RDONLY);
    is_open(fd_in);
    uint16_t in_count = readCount(fd_in);

    int fd_suf = open(argv[5], O_RDONLY);
    is_open(fd_suf);
    uint16_t suf_count = readCount(fd_suf);

    int fd_cruc = open(argv[6], O_RDWR | O_CREAT | O_TRUNC, 0666);
    is_open(fd_suf);

    for(int i = 0; i < affix_count; i++){
        complect_t complect;
        if(wrapper_read(fd_af, &complect, sizeof(complect_t)) != sizeof(complect_t)){
            err(4, "Could not read complect!");
        }
        
        write_postfix(fd_post, post_count, complect.post, fd_cruc);
        write_prefix(fd_pre, pre_count, complect.pre, fd_cruc);
        write_infix(fd_in, in_count, complect.in, fd_cruc);
        write_suffix(fd_suf, suf_count, complect.suf, fd_cruc);
    }

    makeHeader(fd_cruc);

    close(fd_af);
    close(fd_post);
    close(fd_pre);
    close(fd_in);
    close(fd_suf);
    close(fd_cruc);
    return 0;
}
