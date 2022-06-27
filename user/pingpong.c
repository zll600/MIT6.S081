#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int argc, char *argv[]) {
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);

    if (fork() == 0) {
        close(p2c[1]);
        close(c2p[0]);

        char buf;
        read(p2c[0], &buf, 1);
        printf("%d, receive ping\n", getpid());
        write(c2p[1], "1", 1);
        exit(0);
    } else {
        close(p2c[0]);
        close(c2p[1]);

        write(p2c[1], "!", 1);
        char buf;
        read(c2p[0], &buf, 1);
        printf("%d: receive pong\n", getpid());
        wait(0);
    }
    exit(0);
}

