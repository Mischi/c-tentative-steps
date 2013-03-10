#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    printf("Hello World!\n");
    execlp();
    return 0;
}
