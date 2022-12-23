/*
 * @Author: lize GW00301491@ifyou.com
 * @Date: 2022-12-21 17:52:21
 * @LastEditors: lize GW00301491@ifyou.com
 * @LastEditTime: 2022-12-23 11:06:12
 * @FilePath: /test/home/lize/code/zmqdemo/server.cpp
 * @Description: 服务端基础示例
 * 
 * Copyright (c) 2022 by lize GW00301491@ifyou.com, All Rights Reserved. 
 */

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
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://localhost:5556");
    assert (rc == 0);

    /// 3.循环接收数据、发送数据
    while (1) {
        char buffer [10];
        /// 4.接收数据
        zmq_recv (responder, buffer, 10, 0);
        printf ("Received Hello\n");
        sleep (1);
        /// 5.回送数据
        zmq_send (responder, "World", 5, 0);
    }

    // 测试poll里面放ZMQ_REQ类套接字
    #if 0
    #pragma region
    int major, minor, patch;
    zmq_version(&major, &minor, &patch); // 获取当前使用的ZeroMQ的版本号
    printf("Current ZeroMQ version is %d.%d.%d\n", major, minor, patch);

    ///  1.创建上下文
    void *context = zmq_ctx_new ();   // 创建 0MQ上下文  线程安全的
    // perror("创建套接字前");

    /// // 2.创建、绑定套接字
    // ZMQ_REP 服务使用ZMQ_REP类型的套接字来接收来自客户端的请求并向客户端发送回复
    void *responder = zmq_socket (context, ZMQ_REP);
    // perror("bind前");
    int rc = zmq_bind (responder, "tcp://*:5556");
    errno = 0;
    assert (rc == 0);
    // perror("bind后");

    /// 3.循环接收数据、发送数据
    zmq_pollitem_t items[] = {{responder, 0, ZMQ_POLLIN, 0}, {}};
    // zmq_pollitem_t items[] = {};
    int i = 1;
    // perror("进入循环前，错误原因");
    while (1) {
        int rc = zmq_poll(items, 1, 5000);
        printf("rc = %d\n", rc);
        if(rc != 0 ) {
            perror("错误原因");
            printf("rc != 0, 返回事件值为: %d\n", items[0].revents);
        }
        printf("rc = 0, 返回事件值为: %d\n", items[0].revents);
        sleep(3);
        if (rc > 0) {
            if(items[0].revents & ZMQ_POLLIN) {
                char buffer [10];
                /// 4.接收数据
                zmq_recv (responder, buffer, 10, 0);
                printf ("Received Hello\n");
                sleep (1);
                /// 5.回送数据
                char resbuf[100] = "message ";
                resbuf[strlen(resbuf)] = '0' + i;
                zmq_send (responder, resbuf, strlen(resbuf), 0);
                i = (i + 1) % 10;
            }
        }
        zmq_close(responder);
    }
    #pragma endregion
    #endif
    
    return 0;
}
