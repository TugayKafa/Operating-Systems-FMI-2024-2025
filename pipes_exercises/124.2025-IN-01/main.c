#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

typedef struct __attribute((packed)) {
    uint16_t ram_size;
    uint16_t register_count;
    char filename[8];
} triplet_t;

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
        err(7, "Could not write!");
    }
    return wr;
}

int main(int argc, char* argv[]) {
    if(argc != 2){
        err(1, "Wrong arguments!");
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0){
        err(2, "File could not open!");
    }

    struct stat info;
    if(fstat(fd, &info) < 0){
        err(3, "Could not stat!");
    }

    int size = info.st_size / sizeof(triplet_t);
    for(int i = 1; i <= size; i++){
        triplet_t triplet;
        wrapper_read(fd, &triplet, sizeof(triplet));
        
        pid_t pid = fork();
        if (pid == 0){
            int fd_f = open(triplet.filename, O_RDWR);
            if(fd_f < 0){
                err(2, "File could not open!");
            }

            uint8_t register_values[32];
            wrapper_read(fd_f, register_values, triplet.register_count);
            uint8_t register_ram_init[512];
            wrapper_read(fd_f, register_ram_init, triplet.ram_size);
            uint8_t instruction[4];
            while(wrapper_read(fd_f, instruction, 4) == 4){
                switch(instruction[0]){
                    case 0: {
                        register_values[instruction[1]] = register_values[instruction[2]] & register_values[instruction[3]];
                        break;
                    }
                    case 1: {
                        register_values[instruction[1]] = register_values[instruction[2]] | register_values[instruction[3]];
                        break;
                    }
                    case 2: {
                        register_values[instruction[1]] = register_values[instruction[2]] + register_values[instruction[3]];
                        break;
                    }
                    case 3: {
                        register_values[instruction[1]] = register_values[instruction[2]] * register_values[instruction[3]];
                        break;
                    }
                    case 4: {
                        register_values[instruction[1]] = register_values[instruction[2]] ^ register_values[instruction[3]];
                        break;
                    }
                    case 5: {
                        wrapper_write(1, &register_values[instruction[1]], 1);
                        break;
                    }
                    case 6: {
                        sleep(instruction[1]);
                        break;
                    }
                    case 7: {
                        register_values[instruction[1]] = register_ram_init[register_values[instruction[2]]];
                        break;
                    }
                    case 8: {
                         register_ram_init[register_values[instruction[2]]] = register_values[instruction[1]];
                        break;
                    }
                    case 9: {
                        
                    }
                    case 10: {
                        register_values[instruction[1]] = instruction[2];
                        break;
                    }
                    case 11: {
                        register_ram_init[register_values[instruction[1]]] = instruction[2];
                    }
                }
            }

            if(lseek(fd_f, 0, SEEK_SET) < 0){
                err(6, "Could not seek!");
            }
            
            wrapper_write(fd_f, register_values, triplet.register_count);
            wrapper_write(fd_f, register_ram_init, triplet.ram_size);
        }
    }

    for(int i = 1; i<=size; i++){
        wait(NULL);
    }

    return 0;
}
