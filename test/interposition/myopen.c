#include <stdio.h>

extern int __open(char*, int, int);

int open(char* path, int flags, int mode) {
    printf("open : %s\n", path);
    return __open(path, flags, mode);
}
