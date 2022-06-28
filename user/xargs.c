#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

/*
void create_process(char *program, char *args[]) {
    if (fork() == 0) {
        printf("children's pid is %d\n", getpid());
        exec(program, args);
        exit(0);
    }
    return;
}
*/

int main(int argc, char *argv[]) {
    // store arguments for exec
    char *args[MAXARG];
    // pointer to point the first arguments
    int start_idx = 0;
    for (int i = 1; i < argc; ++i) {
        args[start_idx] = argv[i];
        ++start_idx;
    }
    char **args_ptr = &args[start_idx];
    
    // buffer for parsing each argument
    char buf[2048];
    char *buf_ptr = buf, *last_buf_ptr = buf;
    while (read(0, buf_ptr, 1) != 0) {
        if (*buf_ptr == ' ') {
            *buf_ptr = '\0';

            *args_ptr = last_buf_ptr;
            ++args_ptr;
            last_buf_ptr = buf_ptr + 1;
        } else if (*buf_ptr == '\n') {
            *buf_ptr = '\0';

            *args_ptr = last_buf_ptr;
            ++args_ptr;
            last_buf_ptr = buf_ptr + 1;

            *args_ptr = 0;
            if (fork() == 0) {
                exec(argv[1], args);
                exit(0);
            }
            wait((int*)0);

            args_ptr = &args[start_idx];
        }
        ++buf_ptr;
    }

    exit(0);
}
