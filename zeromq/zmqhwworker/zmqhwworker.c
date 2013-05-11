#include "../zhelpers.h"

int
main(int argc, char const *argv[])
{
    void *context = zmq_ctx_new();

    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:8000");

    while(1) {
        char *string = s_recv(responder);
        printf("Received request: [%s]\n", string);
        free(string);

        sleep(1);

        s_send(responder, "World");
    }

    zmq_close(responder);
    zmq_ctx_destroy(context);

    return 0;
}
