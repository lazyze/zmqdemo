/*
 * @Author: lize GW00301491@ifyou.com
 * @Date: 2022-12-23 12:29:47
 * @LastEditors: lize GW00301491@ifyou.com
 * @LastEditTime: 2022-12-23 16:57:59
 * @FilePath: /test/home/lize/code/zmqdemo/client1.cpp
 * @Description: 多客户端
 * 
 * Copyright (c) 2022 by lize GW00301491@ifyou.com, All Rights Reserved. 
 */
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <assert.h>
#include <signal.h>

using namespace std;

void signal_handler(int sig) {
    
}

int main (int argc, char **argv)
{
    signal(SIGINT, signal_handler);
    #if 1
    #pragma region
    printf ("Connecting to server...\n");
    /// 1、创建上下文
    void *context = zmq_ctx_new ();   // 创建 0MQ上下文  线程安全的

    /// 2.创建、绑定套接字
    // ZMQ_REQ  客户端使用ZMQ_REQ类型的套接字向服务发送请求并从服务接收答复
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:8000");
    char push_buf[64] = "";
    char sub_buf[64] = "";

    //创建pull套接字
    void *pull_socket = zmq_socket (context, ZMQ_PULL);

    string ip = "tcp://192.168.61.5:" + std::string(argv[1]);
    const char * pull_ip = ip.c_str();
    int ret = zmq_bind (pull_socket, pull_ip);
    assert(ret == 0);

    printf ("发送pull的ip和端口 %s...\n", pull_ip);
    zmq_send (requester, pull_ip, strlen(pull_ip), 0);

    zmq_recv(requester, push_buf, sizeof(push_buf), 0);
    printf ("接收到服务器push回复消息: %s...\n", push_buf);

    //创建pub套接字
    void *pub_socket = zmq_socket (context, ZMQ_PUB);
    ip = "tcp://192.168.61.5:" + std::string(argv[2]);
    const char * pub_ip = ip.c_str();
    ret = zmq_bind (pub_socket, pub_ip);
    assert(ret == 0);

    printf ("发送pub的ip和端口 %s...\n", pub_ip);
    zmq_send (requester, pub_ip, strlen(pub_ip), 0);

    zmq_recv(requester, sub_buf, sizeof(sub_buf), 0);
    printf ("接收到服务器sub回复消息: %s...\n", sub_buf);

    sleep(1);
    zmq_send (pub_socket, "cmd:begin", 9, 0);

    ///3.循环pull数据
    int num = 0;
    while (1) {
        //接收数据
        char buffer[64] = "";
        zmq_recv (pull_socket, buffer, 63, 0);
        printf ("接收到消息 %s\n", buffer);
        num++;
        if(num == 20) {
            zmq_send (pub_socket, "cmd:stop", 8, 0);
            sleep(25);
            zmq_send (pub_socket, "cmd:begin", 9, 0);
        }


        if(num == 60) {
            zmq_send (pub_socket, "cmd:close", 9, 0);
            break;
        }
    }
    /// 6.关闭套接字、销毁上下文
    zmq_close (requester);
    zmq_close (pull_socket);
    zmq_close(pub_socket);
    zmq_ctx_destroy (context);
    #pragma endregion
    #endif

    return 0;
}
