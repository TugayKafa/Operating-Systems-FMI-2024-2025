#include <fcntl.h> // open(), close()
#include <unistd.h> // fstat(), stat()
#include <sys/stat.h> // struct stat
#include <err.h> //err, errno, errx, 
#include <stdint.h> //uint32_t
#include <string.h>
#include <stdio.h>

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

void startHeader(int fd_o, uint32_t arrN, char* filename){
    wrapper_write(fd_o, "#ifndef ", 8);
    int padding = 'a' - 'A';
    for(int i = 0; i < (int) strlen(filename); i++){
        char c;
        if(filename[i] >= 'a' && filename[i] <= 'z'){
            c = filename[i] - padding;
        }
        else if(filename[i] >= 'A' && filename[i] <= 'Z'){
            c = filename[i];
        } else if(filename[i] == '.') {
            c = '_';
        }
        wrapper_write(fd_o, &c, 1);
    }
    char newLine = '\n';
    wrapper_write(fd_o, &newLine, 1);

    wrapper_write(fd_o, "#define ", 8);
    for(int i = 0; i < (int) strlen(filename); i++){
        char c;
        if(filename[i] >= 'a' && filename[i] <= 'z'){
            c = filename[i] - padding;
        }
        else if(filename[i] >= 'A' && filename[i] <= 'Z'){
            c = filename[i];
        } else if(filename[i] == '.') {
            c = '_';
        }
        wrapper_write(fd_o, &c, 1);
    }
    wrapper_write(fd_o, &newLine, 1);

    const char* incl = "#include <stdint.h>\n";
    wrapper_write(fd_o, incl, strlen(incl));
    const char* arg = "static const uint32_t arrN = ";
    wrapper_write(fd_o, arg, strlen(arg));
    
    char nbuf[32];
    int n = snprintf(nbuf, sizeof nbuf, "%u", arrN);
    wrapper_write(fd_o, nbuf, n);

    const char* finLine = ";\n";    
    wrapper_write(fd_o, finLine, strlen(finLine));
    wrapper_write(fd_o, &newLine, 1);
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
        err(2, "Could not open result file!");
    }

    struct stat info;
    if(fstat(fd_i, &info) < 0){
        err(3, "Could not stat!");
    }

    uint32_t arrN = info.st_size / sizeof(uint16_t);
    if(arrN > 524288){
        err(7, "input file is currpted!");
    }
    startHeader(fd_o, arrN, argv[2]);

    const char* arrName = "static const uint16_t arr[] = {";
    wrapper_write(fd_o, arrName, strlen(arrName));
    
    char out[32];
    for (uint32_t i = 0; i < arrN; i++) {
        uint8_t b[2];
        // четем точно 2 байта
        if (wrapper_read(fd_i, b, 2) != 2) err(4, "read");

        // little-endian -> стойност
        uint16_t v = (uint16_t)b[0] | ((uint16_t)b[1] << 8);

        int n = snprintf(out, sizeof out,
                (i + 1 == arrN) ? "0x%04X" : "0x%04X, ", v);

        wrapper_write(fd_o, out, n);
    }

    const char* arrEnd = "};\n";
    wrapper_write(fd_o, arrEnd, strlen(arrEnd));
    char newLine = '\n';
    wrapper_write(fd_o, &newLine, 1);
    const char* finishFile = "#endif";
    wrapper_write(fd_o, finishFile, strlen(finishFile));

    close(fd_i);
    close(fd_o);
    return 0;
}
