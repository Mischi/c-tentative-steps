#include <sys/types.h>  // getaddrinfo freeaddrinfo gai_strerror
#include <sys/socket.h> // getaddrinfo freeaddrinfo gai_strerror
                        // socket setsockopt bind

#include <netdb.h>      // getaddrinfo freeaddrinfo gai_strerror

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>      // printf snprintf
#include <stdlib.h>     // strtonum
#include <unistd.h>     // sleep
#include <string.h>     // strerror memset
#include <limits.h>     // INT_MAX 
#include <err.h>        // errx
#include <pthread.h>    // pthread_*

#include "zhelpers.h"   // zmq_*

#define WORKER 2
#define ENDPOINT "inproc://requests"
#define FD_BUFFER_SIZE 21

#define PORT "8080"
#define BACKLOG 10 /* how many pending connections queue will hold */


void init_zmq_pushsocket(void**, void**);
void init_worker_threads(pthread_t*, void*);
void init_socket();

void push_fd(void*, int);
int pull_fd(void*);

void *get_in_addr(struct sockaddr*);

void *worker_routine(void*);


int
main(int argc, char const *argv[])
{
    pthread_t worker[WORKER];
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    void *context, *publisher;
    int i, res, sockfd, clientfd;

    init_zmq_pushsocket(&context, &publisher);
    init_worker_threads(worker, context);
    init_socket(&sockfd);
    
    printf("listening on port: %s for incoming connections\n", PORT);

    while(1)
    {
        sin_size = sizeof(client_addr);
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (clientfd == -1)
        {
            warn("accept");
            continue;
        }

        push_fd(publisher, clientfd);
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
init_socket(int *sockfd)
{
    struct addrinfo hints, *servinfo, *p;
    int res, yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP


    if ((res = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
        errx(1,"getaddrinfo: %s", gai_strerror(res));


    //loop through all results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((*sockfd = socket(p->ai_family, p->ai_socktype, 
                        p->ai_protocol)) == -1)
        {
            warn("server: socket");
            continue;
        }

        if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
            err(1, "setsockopt");

        if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(*sockfd);
            warn("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        errx(2, "server: failed to bind\n");
    }

    freeaddrinfo(servinfo); // all done with this structure
    
    if (listen(*sockfd, BACKLOG) == -1)
        err(1, "listen");
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
*get_in_addr(struct sockaddr *sa) /* get sockaddr, IPv4 or IPv6 */ 
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void
*worker_routine(void *context)
{
    int clientfd;

    void *subscriber = zmq_socket(context, ZMQ_PULL);
    if(subscriber == NULL)
        err(1, NULL);

    if(zmq_connect(subscriber, ENDPOINT) != 0)
        err(1, NULL);

    while(1)
    {
        clientfd = pull_fd(subscriber);
        
        if (send(clientfd, "Hello, World!", 13, 0) == -1)
            warn("send");

        close(clientfd);
    }
}
