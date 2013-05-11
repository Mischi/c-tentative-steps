#include <stdio.h>

#include "../zhelpers.h"

int
main(int argc, char const *argv[])
{
    void *context = zmq_ctx_new();

    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:8000");

    int request_nbr;
    for(request_nbr = 0; request_nbr != 10; request_nbr++)
    {
        s_send(requester, "Hello");
        char *string = s_recv(requester);
        printf("Received reply %d [%s]\n", request_nbr, string);
        free(string);
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);

    return 0;
}
