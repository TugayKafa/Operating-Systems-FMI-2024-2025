#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>

int wrapper_read(int fd, void* buff, int size){
    int rd = read(fd, buff, size);
    if(rd < 0){
        err(3, "Could not read from file!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buff, int size){
    int wr = write(fd, buff, size);
    if(wr < 0){
        err(4, "Could not write to file!");
    }
    return wr;
}

int wrapper_lseek(int fd, int offset, int whence){
    int ls = lseek(fd, offset, whence);
    if(ls < 0){
        err(5, "Could not seek!");
    }
    return ls;
}

void copyFile(int from, int to) {
    wrapper_lseek(from, 0, SEEK_SET);
    char ch;
    while(wrapper_read(from, &ch, sizeof(ch)) == sizeof(ch)){
        wrapper_write(to, &ch, sizeof(ch));
    }
}

typedef struct {
    uint32_t magic;
    uint8_t header_version;
    uint8_t data_version;
    uint16_t count;
    uint32_t reserved1;
    uint32_t reserved2;
} header_t;

typedef struct {
    uint16_t offset;
    uint8_t orig;
    uint8_t new;
} version1_t;

typedef struct {
    uint32_t offset;
    uint16_t orig;
    uint16_t new;
} version2_t;

void patchCharachtersV1(int patch, int fd, int count){
    struct stat info;
    if(fstat(patch, &info) < 0){
        err(9, "could not stat file!");
    }

    struct stat fd_info;
    if(fstat(fd, &fd_info) < 0){
        err(9, "Could not stat file!");
    }

    uint8_t byte;
    version1_t patch_info;
    for(int i = 1; i <= count; i++){
        if(wrapper_read(patch, &patch_info, sizeof(patch_info)) != sizeof(patch_info)){
            err(11, "Could not read patch info!");
        }
        if(patch_info.offset >= fd_info.st_size){
            err(13, "Offset is out of bounds of file!");
        }

        wrapper_lseek(fd, sizeof(byte)*patch_info.offset, SEEK_SET);
        if(wrapper_read(fd, &byte, sizeof(byte)) != sizeof(byte)){
            err(12, "Could not read bytes from file!");
        }
        if(byte != patch_info.orig){
            err(14, "Not correct byte on the position!");
        }
        wrapper_lseek(fd, sizeof(byte)*patch_info.offset, SEEK_SET);
        if(wrapper_write(fd, &patch_info.new, sizeof(uint8_t)) != sizeof(uint8_t)){
            err(15, "Could not write new byte!");
        }
    }
}

void patchCharachtersV2(int patch, int fd, int count){
    struct stat info;
    if(fstat(patch, &info) < 0){
        err(9, "could not stat file!");
    }

    struct stat fd_info;
    if(fstat(fd, &fd_info) < 0){
        err(9, "Could not stat file!");
    }

    uint16_t byte;
    version2_t patch_info;
    for(int i = 1; i <= count; i++){
        if(wrapper_read(patch, &patch_info, sizeof(patch_info)) != sizeof(patch_info)){
            err(11, "Could not read patch info!");
        }
        if(patch_info.offset >= fd_info.st_size){
            err(13, "Offset is out of bounds of file!");
        }

        wrapper_lseek(fd, sizeof(byte)*patch_info.offset, SEEK_SET);
        if(wrapper_read(fd, &byte, sizeof(byte)) != sizeof(byte)){
            err(12, "Could not read bytes from file!");
        }
        if(byte != patch_info.orig){
            err(14, "Not correct byte on the position!");
        }
        wrapper_lseek(fd, sizeof(byte)*patch_info.offset, SEEK_SET);
        if(wrapper_write(fd, &patch_info.new, sizeof(uint16_t)) != sizeof(uint16_t)){
            err(15, "Could not write new byte!");
        }
    }
}

void process(int patch, int fd){
    header_t header;
    if(wrapper_read(patch, &header, sizeof(header)) != sizeof(header)){
        err(5, "Could not read patch header!");
    }

    if(header.magic != 0xEFBEADDE){
        err(6, "Wrong header specification!");
    }

    if(header.header_version != 0x01){
        err(7, "Wrong header version!");
    }

    if(header.data_version != 0x00 && header.data_version != 0x01){
        err(8, "Wrong data version!");
    }

    if(header.data_version == 0x00){
        patchCharachtersV1(patch, fd, header.count);
    }
    else {
        patchCharachtersV2(patch, fd, header.count);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 4){
        err(1, "Wrong arguments!");
    }

    int fd1 = open(argv[2], O_RDONLY);
    if(fd1 < 0){
        err(2, "Could not open input file!");
    }
    int fd2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if(fd2 < 0){
        err(2, "Could not open output file!");
    }

    copyFile(fd1, fd2);

    int fdp = open(argv[1], O_RDONLY);
    if(fdp < 0){
        err(2, "Could not open patch file!");
    }

    process(fdp, fd2);

    close(fd1);
    close(fd2);
    close(fdp);
    return 0;
}
