#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

typedef struct {
    uint16_t pos;
    uint8_t len;
    uint8_t blabla;
} instruction;

void isFileOpenedSuccessfully(int fd){
    if(fd < 0){
        err(1, "File could not open!");
    }
}

void isStatGetSuccessfully(int stat_fd){
    if(stat_fd < 0){
        err(2, "File could not stat!");
    }
}

void isReadWentSuccessfully(int st){
    if(st < 0){
        err(4, "Could not read!");
    }
}

void isSeekSuccessfully(int sk){
    if(sk < 0){
        err(5, "Could not seek!");
    }
}

void isWriteSuccessfull(int wr){
    if(wr < 0){
        err(6, "Could not write!");
    }
}

void makeMagicHappen(int fd1, int fd1_size, int fd2, int fd2_size, int fd3, int fd4)  {
    instruction instr;
    int seek = lseek(fd2, 0, SEEK_SET);
    isSeekSuccessfully(seek);
    for(int i = 0; i < fd2_size; i++){
        int rd = read(fd2, &instr, sizeof(instruction));
        isReadWentSuccessfully(rd);

        if(fd1_size <= instr.pos || fd1_size <= instr.pos + instr.len - 1){
            continue;
        }

        seek = lseek(fd1, instr.pos, SEEK_SET);
        isSeekSuccessfully(seek);

        char ch;
        rd = read(fd1, &ch, 1);
        isReadWentSuccessfully(rd);
        if(ch < 'A' || ch > 'Z'){
            continue;
        }

        instruction instr2;
        instr2.pos = lseek(fd3, 0, SEEK_END);
        instr2.len = instr.len;
        instr2.blabla = 0x00;
        int wr = write(fd4, &instr2, sizeof(instr2)); // writing instructions of result file
        isWriteSuccessfull(wr);

        wr = write(fd3, &ch, 1);
        isWriteSuccessfull(wr);
        for(int k=2; k <= instr.len; k++){
            rd = read(fd1, &ch, 1);
            isReadWentSuccessfully(rd);

            wr = write(fd3, &ch, 1);
            isWriteSuccessfull(wr);
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 5){
     err(1,"You need to have 4 argument");
    }
    
    int fd1 = open(argv[1], O_RDONLY);
    isFileOpenedSuccessfully(fd1);

    struct stat info_fd1;
    int stat_fd = fstat(fd1, &info_fd1);
    isStatGetSuccessfully(stat_fd);
    int fd1_size = info_fd1.st_size;
    
    int fd2 = open(argv[2], O_RDONLY);
    isFileOpenedSuccessfully(fd2);

    struct stat info_fd2;
    stat_fd = fstat(fd2, &info_fd2);
    isStatGetSuccessfully(stat_fd);

    if(info_fd2.st_size % sizeof(instruction) != 0){
        err(3, "Instruction file is corrupted!");
    }

    int fd2_size = info_fd2.st_size / sizeof(instruction);

    int fd3 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0644);
    isFileOpenedSuccessfully(fd3);

    int fd4 = open(argv[4], O_CREAT | O_TRUNC | O_RDWR, 0644);
    isFileOpenedSuccessfully(fd4);

    makeMagicHappen(fd1, fd1_size, fd2, fd2_size, fd3, fd4);

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);
    return 0;
}
