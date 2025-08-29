#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <stdlib.h>
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>

void readFromFile(int fd) {
    char a;
    while(read(fd, &a, sizeof(a)) == 1){
        int wr = write(1, &a, sizeof(a));
        if (wr < 0){
            err(1, "Could not write!");
        }
    }
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++ ){
        if(!strcmp(argv[i], "-")){
            readFromFile(0);
            continue;
        }

        int fd = open(argv[i], O_RDONLY);
        if (fd < 0){
            continue;
        }

        readFromFile(fd);
        close(fd);
    }

    if(argc == 1){
        readFromFile(0);
    }

    return 0;
}
