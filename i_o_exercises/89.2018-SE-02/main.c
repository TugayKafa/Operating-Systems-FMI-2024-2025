#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t

void writeToFile(int fd, char ch){
    int wr = write(fd, &ch, sizeof(ch));
    if(wr != 1){
        err(3, "Could not write!");
    }
}

void copyFile(int src, int dst){
    char a;
    while(read(src, &a, sizeof(a))==1){
        writeToFile(dst, a);
    }
}

off_t wrapper_lseek(int fd, off_t offset, int whence){
    int l = lseek(fd, offset, whence);
    if(l < 0){
        err(4, "Could not seek!");
    }
    return l;
}

int wrapper_read(int fd, void* buff, int size){
    int r = read(fd, buff, size);
    if(r<0){
        err(5, "Could not seek!");
    }
    return r;
}

int wrapper_write(int fd, void* buff, int size){
    int w = write(fd, buff, size);
    if(w<0){
        err(5, "Could not seek!");
    }
    return w;
}

void sortFileData(int fd){
    struct stat info;
    if(fstat(fd, &info) == -1){
        err(4,"Could not stat");
    }

    int size1 = info.st_size/sizeof(uint32_t);
    uint32_t ch1;
    uint32_t ch2;

    for(int i = 0; i < size1 - 1; i++){
        size_t iNumber = i*sizeof(uint32_t);
        size_t minIndex=iNumber;
        wrapper_lseek(fd, iNumber, SEEK_SET);
        wrapper_read(fd, &ch1, sizeof(ch1));
        for(int j = i+1; j < size1; j++ ){
            wrapper_read(fd, &ch2, sizeof(ch2));
            if(ch2 < ch1){
                minIndex=j*sizeof(uint32_t);
                ch1=ch2;
            }
        }
        if(minIndex!=iNumber){
            wrapper_lseek(fd, iNumber, SEEK_SET);
            uint32_t curr;
            wrapper_read(fd, &curr, sizeof(curr));

            wrapper_lseek(fd, minIndex, SEEK_SET);
            wrapper_write(fd, &curr, sizeof(curr));
            wrapper_lseek(fd, iNumber, SEEK_SET);
            wrapper_write(fd, &iNumber, SEEK_SET);
        }
    }
}

int main(int argc, char* argv[]){
    if (argc != 3){
        err(1, "Wrong arguments!");
    }
    int f1 = open(argv[1], O_RDONLY);
    if(f1 < 0){
        err(2, "Could not open source file!");
    }

    int f2 = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if(f2 < 0){
        err(3, "Could not open destination file!");
    }

    copyFile(f1, f2);
    sortFileData(f2);
    close(f1);
    close(f2);
    return 0;
}
