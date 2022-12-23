//  Hello World client
#include <zmq.h>
#include <stdio.h>
 #include <unistd.h>

int main (void)
{
    //
    #if 1
    #pragma region
    printf ("Connecting to hello world server...\n");
    /// 1、创建上下文
    void *context = zmq_ctx_new ();   // 创建 0MQ上下文  线程安全的

    /// 2.创建、绑定套接字
    // ZMQ_REQ  客户端使用ZMQ_REQ类型的套接字向服务发送请求并从服务接收答复
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5556");

    ///3.循环发送数据、接收数据
    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        printf ("Sending Hello %d...\n", request_nbr);
        /// 4.发送数据
        zmq_send (requester, "Hello", 5, 0);
        // 5.接收回复数据
        zmq_recv (requester, buffer, 10, 0);
        printf ("Received World %s %d\n", buffer, request_nbr);
    }
    /// 6.关闭套接字、销毁上下文
    zmq_close (requester);
    zmq_ctx_destroy (context);
    #pragma endregion
    #endif

    //模拟停止、关闭、开始、连接操作
    #if 0
    #pragma region
     /// 1、创建上下文
    void *context = zmq_ctx_new ();   // 创建 0MQ上下文  线程安全的

    /// 2.创建、绑定套接字
    // ZMQ_REQ  客户端使用ZMQ_REQ类型的套接字向服务发送请求并从服务接收答复
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://10.66.150.1:5555");

    /// 3.发送客户端数据，本机ip和zmq pull套接字的端口号
    printf ("Sending begin \n");
    char addr_pull[] = "tcp://10.66.150.1:5556";

    zmq_send (requester, addr_pull, 22, 0);
    char rec_message[23] = "";
    zmq_recv(requester, rec_message, 8, 0);
    printf("receive message = %s\n", rec_message);
    // zmq_send (requester, begin, 22, 0);
    sleep(1);

    char addr_pub[] = "tcp://10.66.150.1:5557";
    zmq_send (requester, addr_pub, 22, 0);
    zmq_recv(requester, rec_message, 8, 0);

    void *pull = zmq_socket (context, ZMQ_PULL);
    zmq_bind (pull, addr_pull);

    void *pub = zmq_socket (context, ZMQ_PUB);
    zmq_bind (pub, addr_pub);

    ///4.循环接收数据
    char stop[] = "cmd:stop";
    char begin[] = "cmd:begin";
    int request_nbr;
    for (request_nbr = 0; request_nbr != 50; request_nbr++) {
        char buffer[128];
        // 5.接收数据
        printf ("next do recv World\n");
        zmq_recv (pull, buffer, 128, 0);
        printf ("Received World %d %s\n", request_nbr, buffer);
        // sleep(3);
        if (request_nbr != 0 && request_nbr % 10 == 0) {
            zmq_send (pub, stop, 8, 0);
            // zmq_send (pull, begin, 22, 0);
            sleep(5);
            zmq_send (pub, begin, 9, 0);
        }
    }
    char close[] = "cmd:close";
    zmq_send (pub, close, 9, 0);
    zmq_close (pub);
    /// 6.关闭套接字、销毁上下文
    zmq_close (requester);
    zmq_ctx_destroy (context);
    #pragma endregion
    #endif
    return 0;
}
