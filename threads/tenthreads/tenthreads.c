#include <stdio.h>      // printf
#include <stdlib.h>     // malloc
#include <unistd.h>     // sleep
#include <string.h>     // strerror
#include <err.h>        // err errx
#include <pthread.h>    // pthread_*

#define THREADCOUNT 10

struct data {
    int number;
    char *msg;
};

void *child_routine(void*);

int
main(int argc, char const *argv[])
{
    pthread_t thread[THREADCOUNT];
    struct data *thread_data;
    int res, i;


    for(i = 0; i < THREADCOUNT; i++)
    {
            if((thread_data = malloc(sizeof(struct data))) == NULL)
                err(1, "malloc");

            thread_data->number = i+1;
            thread_data->msg = "Hello World";

            if((res = pthread_create(&thread[i], NULL, child_routine, thread_data)) != 0)
                errx(1, "pthread_create: %s", strerror(res));
    }



    for(i = 0; i < THREADCOUNT; i++)
    {
            if((res = pthread_join(thread[i], NULL)) != 0)
                errx(1, "pthread_join: %ss", strerror(res));
    }


    return 0;
}

void
*child_routine(void *args)
{
    struct data *thread_data = (struct data*)args;
    sleep(thread_data->number);
    printf("'%s' from Thread %d\n", thread_data->msg, thread_data->number);
    pthread_exit(0);
}
