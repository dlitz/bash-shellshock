/*
 * Usage:
 *  ./testenv.t
 *      - Print the environment
 *  ./testenv.t FOO=BAR... -- CMD [ARG...]
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
extern char **environ;

int main(int argc, char **argv) {
    if (argc == 1) {
        char **p;
        for (p = environ; *p != NULL; p++) {
            puts(*p);
        }
        exit(0);
    } else {
        char **env = argv + 1;
        for (; *argv != NULL; argv++) {
            if (argv[0][0] == '-' && argv[0][1] == '-' && argv[0][2] == '\0') {
                *argv = NULL;
                argv++;
                break;
            }
        }
        if (*argv == NULL) {
            fprintf(stderr, "testenv: No argv specified\n");
            exit(127);
        }

        execve(argv[0], argv, env);
        perror("testenv: command failed");
        exit(127);
    }
}
