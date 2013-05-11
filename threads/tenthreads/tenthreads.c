#include <stdio.h>
#include <string.h>
#include <err.h>
#include <pthread.h>

void* child_routine(void*);

int
main(int argc, char const *argv[])
{
    pthread_t thread;
    int res;

    if((res = pthread_create(&thread, NULL, child_routine, "Hello World")) != 0)
        errx(1, "pthread_create: %s", strerror(res));


    if((res = pthread_join(thread, NULL)) != 0)
        errx(1, "pthread_join: %ss", strerror(res));

    return 0;
}

void*
child_routine(void* args)
{
    char *string = (char*)args;
    pthread_t id = pthread_self();
    printf("'%s' from Thread %lu\n", string, id);
    pthread_exit(0);
}
