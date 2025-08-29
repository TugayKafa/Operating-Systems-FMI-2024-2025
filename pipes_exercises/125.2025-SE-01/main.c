#include <err.h> // err()
#include <unistd.h> //fork(), exec*(), pipe(), dup2(), read(), write(), close()
#include <sys/wait.h> // wait(), waitpid()
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

const char* wheel_program = "fake_wheel";
const char* driver_program = "fake_driver";

int wrapper_read(int fd, void* buffer, int size){
    int rd = read(fd, buffer, size);
    if(rd < 0 || rd != size){
        err(5, "Could not read!");
    }
    return rd;
}

int wrapper_write(int fd, const void* buffer, int size){
    int wr = write(fd, buffer, size);
    if(wr < 0 || wr != size){
        err(6, "Could not write!");
    }
    return wr;
}

typedef struct {
    uint8_t buff[16];
} package_t;

int pd[2];
int pw[4][2];
int wp[4][2];

int pids[5];

void handleWheel(int ind){
    if(pipe(pw[ind]) < 0){
        err(1, "Could not pipe!");
    }
    if(pipe(wp[ind]) < 0){
        err(1, "Could not pipe!");
    }
    
    int pid_wheel = fork();
    if(pid_wheel == 0){
        close(pw[ind][1]);
        close(wp[ind][0]);
        if(dup2(pw[ind][0], 0) < 0){
            err(3, "Could not dup!");
        }
        if(dup2(wp[ind][1], 1) < 0){
            err(3, "Could not dup!");
        }

        execlp(wheel_program, wheel_program, NULL);
        err(4, "Could not exec!");
    }
    pids[ind] = pid_wheel;
    close(pw[ind][0]);
    close(wp[ind][1]);
}

void handleDriver(void){
    if(pipe(pd) < 0){
        err(1, "Could not pipe!");
    }
    int pid_driver = fork();
    if(pid_driver < 0){
        err(2, "Could not fork!");
    }
    if(pid_driver == 0){
        close(pd[0]);
        if(dup2(pd[1],1) < 0){
            err(3, "Could not dup!");
        }
        execlp(driver_program, driver_program, NULL);
        err(4, "Could not exec!");
    }
    pids[4] = pid_driver;
    close(pd[1]);
}

void handleDeadProcesses(void){
    int st; pid_t dead;
    while ((dead = waitpid(-1, &st, WNOHANG)) > 0) {
        if (dead == pids[4]) {
            close(pd[0]);
            handleDriver();
        } else {
            for (int i = 0; i < 4; i++) if (dead == pids[i]) {
                close(pw[i][1]); close(wp[i][0]);
                handleWheel(i);
                break;
            }
        }
    }
}

int main(void) {

    handleDriver();
    
    for(int i = 0; i < 4;i++){
        handleWheel(i);
    }

    uint16_t I = 0;
    while(1){
        handleDeadProcesses();

        package_t package;
        wrapper_read(pd[0], &package, sizeof(package));
        
        uint16_t omega = 0;
        omega |= (package.buff[8] << 8);
        omega |= package.buff[9];

        int average = 0;
        for(int i = 0; i < 4; i++){
            wrapper_read(wp[i][0], &package, sizeof(package));
            
            uint16_t skorost = 0;
            skorost |= (package.buff[2] << 8);
            skorost |= package.buff[3];
            average += (int) skorost;
        }
        average /= 4;
        if((uint16_t) average < omega) I++;
        else if((uint16_t) average > omega) I--;

        for(int i = 0;i<16;i++){
            package.buff[i] = 0;
        }
        package.buff[2] = ((I >> 8) & 0xFF);
        package.buff[3] = (I & 0xFF);

        for(int i = 0; i<4; i++){
            wrapper_write(pw[i][1], &package, sizeof(package));
        }
    }

    return 0;
}
