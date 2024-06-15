#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int fd;
    fd = open("/home/lihao/install.sh", O_RDONLY);
    if (fd != -1)
        fprintf(stdout, "openfile successed!\n");
    else
        fprintf(stdout, "openfile failed!\n");
    close(fd);
    return 0;
}
