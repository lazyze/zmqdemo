//  Hello World client
#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main (void)
{
    printf ("Connecting to hello world server...\n");
    /// 1、创建上下文
    void *context = zmq_ctx_new ();   // 创建 0MQ上下文  线程安全的

    /// 2.创建、绑定套接字
    // ZMQ_REQ  客户端使用ZMQ_REQ类型的套接字向服务发送请求并从服务接收答复
    void *g_log_socket = zmq_socket (context, ZMQ_PULL);
    // int rc = zmq_connect (g_log_socket, "tcp://localhost:5661");
    int rc = zmq_bind (g_log_socket, "tcp://*:8000");
    if (rc == -1) perror("通过 perror 输出错误");
    assert(rc == 0);

    ///3.循环发送数据、接收数据
    int request_nbr = 0;
    while (1) {
        char buffer [4096];
        // printf ("Sending Hello %d...\n", request_nbr++);
        /// 4.发送数据
        // zmq_send (g_log_socket, "Hello", 5, 0);
        // 5.接收回复数据
        zmq_recv (g_log_socket, buffer, 4096, 0);
        // printf ("Received World %d\n", request_nbr);
        printf ("Received buffer %s\n", buffer);

        // zmq_msg_t msg;
        // zmq_msg_init(&msg);
        // int rc = 0;
        // rc = zmq_msg_recv(&msg, g_log_socket, 0);
        // if(rc == -1)
        //         return NULL;
        
        // char *str = (char*)malloc(rc + 1);
        // if(str == NULL)
        //     return NULL;
        // memcpy(str, zmq_msg_data(&msg), rc);
    
        // str[rc] = 0;
        // printf("收到消息%s\n", str);

    }
    /// 6.关闭套接字、销毁上下文
    zmq_close (g_log_socket);
    zmq_ctx_destroy (context);
    return 0;
}
