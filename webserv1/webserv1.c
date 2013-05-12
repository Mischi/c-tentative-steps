#include <stdio.h>      // printf snprintf
#include <stdlib.h>     // strtonum
#include <unistd.h>     // sleep
#include <string.h>     // strerror
#include <limits.h>     // INT_MAX 
#include <err.h>        // errx
#include <pthread.h>    // pthread_*

#include "zhelpers.h"   // zmq_*

#define WORKER 2
#define ENDPOINT "inproc://requests"
#define FD_BUFFER_SIZE 21

void init_zmq_pushsocket(void**, void**);
void init_worker_threads(pthread_t*, void*);

void push_fd(void*, int);
int pull_fd(void*);

void *worker_routine(void*);


int
main(int argc, char const *argv[])
{
    pthread_t worker[WORKER];
    void *context, *publisher;
    int i, res;

    init_zmq_pushsocket(&context, &publisher);
    init_worker_threads(worker, context);


    /* TODO: bind to socket and start handleing requests */
    printf("Sending test messages\n");
    for(i = 0; i < 1000; i++)
    {
        push_fd(publisher, i);
    }
    
    /* wait for threads to exit */
    for(i = 0; i < WORKER; i++)
    {
        if((res = pthread_join(worker[i], NULL)) != 0)
            errx(1, "pthread_join: %s", strerror(res));
    }

    /* close zmq publisher */
    zmq_close(publisher);
    zmq_ctx_destroy(context);

    return 0;
}

void
init_zmq_pushsocket(void** context, void** pusher)
{
    /* starting zmq publisher */
    printf("Starting zmq publisher\n");
    
    *context = zmq_ctx_new();
    if(*context == NULL)
        err(1, NULL);

    *pusher = zmq_socket(*context, ZMQ_PUSH);
    if(*pusher == NULL)
        err(1, NULL);

    if(zmq_bind(*pusher, ENDPOINT) != 0)
        err(1, NULL);
}


void
init_worker_threads(pthread_t *worker, void *context)
{
    int i, res;

    printf("Starting worker threads\n");
    for(i = 0; i < WORKER; i++)
    {
        if((res = pthread_create(&worker[i], NULL, worker_routine, context)) != 0)
            errx(1, "pthread_create: %s", strerror(res));
    }

    /*give workers a chance to connect */
    sleep(1);
}

void
push_fd(void *socket, int fd)
{
    char pushbuffer[FD_BUFFER_SIZE];
    snprintf(pushbuffer, FD_BUFFER_SIZE, "%d", fd);

    if(zmq_send(socket, pushbuffer, FD_BUFFER_SIZE, 0) == -1)
        warn("zmq_send");
}

int
pull_fd(void *socket)
{
    int fd;
    const char *errstr;
    char pullbuffer[FD_BUFFER_SIZE];

    if(zmq_recv(socket, pullbuffer, FD_BUFFER_SIZE, 0) == -1)
        err(1, NULL);

    fd = strtonum(pullbuffer, 0, INT_MAX, &errstr);
    if(errstr)
        errx(1, "fd is %s: %s", errstr, pullbuffer);

    return fd;
}

void
*worker_routine(void *context)
{
    int fd;

    void *subscriber = zmq_socket(context, ZMQ_PULL);
    if(subscriber == NULL)
        err(1, NULL);

    if(zmq_connect(subscriber, ENDPOINT) != 0)
        err(1, NULL);

    while(1)
    {
        fd = pull_fd(subscriber);
        printf("result: %d\n", fd);

        /* TODO: Handle request and send response */
    }
}
