//  Synchronized subscriber

#include "zhelpers.h"
#include <unistd.h>
#include <string>

int main (void)
{
    void *context = zmq_ctx_new ();

    //  First, connect our subscriber socket
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://localhost:5561");
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "", 0);

    //  0MQ is so fast, we need to wait a while...
    sleep (1);

    //  Second, synchronize with publisher
    void *syncclient = zmq_socket (context, ZMQ_REQ);
    zmq_connect (syncclient, "tcp://localhost:5562");

    std::string str = "";
    for (int i = 0; i < 10; i++) {
        s_send (syncclient, "hello");
        printf("lz-------syncclient send message hello %d\n", i);

        //  - wait for synchronization reply
        char *string = s_recv (syncclient);
        printf("lz-------syncclient receive message %s %d\n", string, i);
        free (string);
    }

    //  Third, get our updates and report how many we got
    int update_nbr = 0;
    while (1) {
        char *string = s_recv (subscriber);
        if (strcmp (string, "END") == 0) {
            free (string);
            break;
        }
        printf("lz-------sub receive message %s\n", string);
        free (string);
        update_nbr++;
    }
    printf ("Received %d updates\n", update_nbr);

    zmq_close (subscriber);
    zmq_close (syncclient);
    zmq_ctx_destroy (context);
    return 0;
}
