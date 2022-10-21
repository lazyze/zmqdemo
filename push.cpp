
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <string.h>

int main (void)
{

    int major, minor, patch;
    zmq_version(&major, &minor, &patch); // 获取当前使用的ZeroMQ的版本号
    printf("Current ZeroMQ version is %d.%d.%d\n", major, minor, patch);

    ///  1.创建上下文
    void *context = zmq_ctx_new ();   // 创建 0MQ上下文  线程安全的

    /// // 2.创建、绑定套接字
    // ZMQ_REP 服务使用ZMQ_REP类型的套接字来接收来自客户端的请求并向客户端发送回复
    void *sender = zmq_socket (context, ZMQ_PUSH);
    int rc = zmq_connect (sender, "tcp://localhost:5661");
    // int rc = zmq_bind (sender, "tcp://*:5557");
    if (rc == -1) perror("通过 perror 输出错误");
    assert (rc == 0);

    /// 3.循环接收数据、发送数据
    int i = 0;
    while (1) {
        // char buffer [10];
        // /// 4.接收数据
        // // zmq_recv (sender, buffer, 10, 0);
        // printf ("Received Hello\n");
        // sleep (1);
        // /// 5.回送数据
        if (i % 2 == 0)
            zmq_send (sender, "World", 5, 0);
        else 
            zmq_send (sender, "hello", 5, 0);
        i = (++i) % 10000;
        printf("send over \n");
        sleep(1);
        // char* str = (i % 2 == 1 ? "hello1" : "test");

        // char* str;
        // if (i % 2 == 0)
        //     str = "test";
        // else
        //     str = "hello1";
        // i = (++i) % 10000;
        // zmq_msg_t msg;
        // zmq_msg_init_size(&msg, strlen(str));
        // memcpy(zmq_msg_data(&msg), str, strlen(str));
    
        // rc = zmq_msg_send(&msg, sender, 0);
    
        // zmq_msg_close(&msg);

    }
    return 0;
}
