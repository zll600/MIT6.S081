#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void next_stage(int *left_pipe) {
    close(left_pipe[1]);

    int p;
    if (read(left_pipe[0], &p, 1) == 0) {
        exit(0);
    }
    printf("prime %d\n", p);

    int right_pipe[2];
    pipe(right_pipe);

    int pid = fork();
    if (pid == 0) {
        next_stage(right_pipe);
        exit(0);
    } else if (pid > 0) {
        close(right_pipe[0]);
        
        int buf;
        while (read(left_pipe[0], &buf, 1) != 0) {
            if (buf % p != 0) {
                write(right_pipe[1], &buf, 1);
            } 
        }
        close(right_pipe[1]);
    } else {
        printf("for error: \n");
    }
    wait(0);
    exit(0);
}

int main(int argc, char *argv[]) {
    int p2c[2];
    pipe(p2c);

    if (fork() == 0) {
        next_stage(p2c);
        exit(0);
    } else {
        close(p2c[0]);
        for (int i = 2; i <= 35; ++i) {
            write(p2c[1], &i, 1);
        }
        close(p2c[1]);
        wait((int*)0);
    }

    exit(0);
}
