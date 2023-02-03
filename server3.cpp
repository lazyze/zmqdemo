/*
 * @Author: lize GW00301491@ifyou.com
 * @Date: 2022-12-21 17:52:21
 * @LastEditors: lize GW00301491@ifyou.com
 * @LastEditTime: 2023-02-03 15:32:19
 * @FilePath: /test/home/lize/code/zmqdemo/server1.cpp
 * @Description: 代理模式
 *
 * Copyright (c) 2022 by lize GW00301491@ifyou.com, All Rights Reserved.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

#include <string>
using namespace std;

#define CLIENT_NUM 3
#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 1000  // 毫秒
void *push_socket[CLIENT_NUM];
void *sub_socket[CLIENT_NUM];
bool push_flag[CLIENT_NUM];
int liveness[CLIENT_NUM];
int sendNum = 0;

void itemAdd(zmq_pollitem_t &item, void *socket) {
  item.socket = socket;
  item.fd = 0;
  item.events = ZMQ_POLLIN;
  item.revents = 0;
}

void sendMessage(int n) {
  sendNum++;
  // char buf[100] = "message ";
  // while (n != 0) {
  //     buf[strlen(buf)] = '0' + n % 10;
  //     n /= 10;
  // }
  string buf = "message";
  buf += to_string(n);
  int num = 0;
  for (int i = 0; i < CLIENT_NUM; i++) {
    if (push_socket[i] != NULL && push_flag[i]) {
      num++;
      zmq_send(push_socket[i], buf.c_str(), strlen(buf.c_str()), 0);
    }
  }
  printf("第%d次发送消息, 发送到%d个客户端\n", sendNum, num);
}

void *thread_function(void *args) {
  void *frontend = zmq_socket(args, ZMQ_XSUB);
  zmq_connect(frontend, "tcp://10.66.150.1:8001");
  void *backend = zmq_socket(args, ZMQ_XPUB);
  zmq_bind(backend, "tcp://*:8002");

  zmq_proxy(frontend, backend, NULL);
}

int main(void) {
#if 1
#pragma region
  int major, minor, patch;
  zmq_version(&major, &minor, &patch);  // 获取当前使用的ZeroMQ的版本号
  printf("Current ZeroMQ version is %d.%d.%d\n", major, minor, patch);

  ///  1.创建上下文
  void *context = zmq_ctx_new();  // 创建 0MQ上下文  线程安全的

  /// // 2.创建、绑定套接字
  //
  void *proxyResponder = zmq_socket(context, ZMQ_REP);

  void *responder = zmq_socket(context, ZMQ_SUB);

  void *proxyPublisher = NULL;

  void *frontend = NULL;
  void *backend = NULL;
  pthread_t mythread = -1;

  bool proxyFlag = false;

  // zmq_poll参数赋值
  zmq_pollitem_t *items =
      (zmq_pollitem_t *)malloc((CLIENT_NUM + 2) * sizeof(zmq_pollitem_t));
  for (int i = 0; i < CLIENT_NUM; i++) {
    itemAdd(items[i], NULL);
  }
  // push_socket套接字赋值
  //  sockerAssign(push_socket);
  for (int i = 0; i < CLIENT_NUM; i++) {
    push_socket[i] = NULL;
    push_flag[i] = false;
  }

  int ret = zmq_bind(proxyResponder, "tcp://*:8000");
  itemAdd(items[CLIENT_NUM + 1], proxyResponder);
  if (ret == 0) {
    printf("申请代理成功\n");
    // proxyFlag = true;
    proxyPublisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(proxyPublisher, "tcp://*:8001");

    // frontend = zmq_socket(context, ZMQ_XSUB);
    // zmq_connect(frontend, "tcp://10.66.150.1:8001");
    // backend = zmq_socket(context, ZMQ_XPUB);
    // zmq_bind(backend, "tcp://*:8002");
    pthread_create(&mythread, NULL, thread_function, context);
    sleep(1);

    while (1) {
      ret = zmq_poll(items, CLIENT_NUM + 2, -1);
      if (ret < 0) {
        printf("zmq_poll返回值为%d\n", ret);
        break;
      }
      // printf("zmq_poll返回值为%d\n", ret);

      // 检测到客户端发送来的连接
      if (items[CLIENT_NUM + 1].revents & ZMQ_POLLIN) {
        printf("client connecting......\n");
        char client_pull_addr[26] = "";
        ret = zmq_recv(proxyResponder, client_pull_addr, 25, 0);
        printf("接收返回值ret = %d, 接收到发送的pull的ip和端口 = %s\n", ret,
               client_pull_addr);
        zmq_send(proxyResponder, client_pull_addr, strlen(client_pull_addr), 0);
        zmq_send(proxyPublisher, client_pull_addr, strlen(client_pull_addr), 0);
      }
      // 检测到代理发送的信息
      if (items[CLIENT_NUM].revents & ZMQ_POLLIN) {
        for (int i = 0; i < CLIENT_NUM; i++) {
          if (push_socket[i] == NULL) {
            push_socket[i] = zmq_socket(context, ZMQ_PUSH);
            sub_socket[i] = zmq_socket(context, ZMQ_SUB);

            // 接收客户端发过来的pull套接字端口信息，发送消息到此套接字接收
            char client_pull_addr[26] = "";
            printf("waiting client %d connect......\n", i);
            ret = zmq_recv(responder, client_pull_addr, 25, 0);
            printf(
                "接收返回值rc = %d, 接收到客户端%d发送的pull的ip和端口 = %s\n",
                ret, i, client_pull_addr);
            // zmq_send(responder, "received pull", 13, 0);

            // push与pull建立连接
            ret = zmq_connect(push_socket[i], client_pull_addr);
            assert(ret == 0);
            // push_flag[i] = true;

            // 接收客户端发过来的pub套接字端口信息,接收此套接字发过来的开始,停止,结束等信息
            char client_sub_addr[26] = "";
            ret = zmq_recv(responder, client_sub_addr, 25, 0);
            printf(
                "接收返回值rc = %d, 接收到客户端%d发送的pub的ip和端口 = %s\n",
                ret, i, client_sub_addr);
            // zmq_send(responder, "received pub", 12, 0);

            // pub与sub建立连接,且设置订阅过滤信息
            ret = zmq_connect(sub_socket[i], client_sub_addr);
            assert(ret == 0);
            ret = zmq_setsockopt(sub_socket[i], ZMQ_SUBSCRIBE, "cmd:", 4);
            assert(ret == 0);

            // 将pub加入items
            itemAdd(items[i], sub_socket[i]);
            break;
          }
        }
      }
      // 检测到pub事件
      for (int i = 0; i < CLIENT_NUM; i++) {
        if (items[i].revents & ZMQ_POLLIN) {
          zmq_msg_t msg;
          zmq_msg_init(&msg);
          char *cmd_info;
          int size = zmq_msg_recv(&msg, items[i].socket, 0);
          cmd_info = (char *)zmq_msg_data(&msg);
          cmd_info[zmq_msg_size(&msg)] = 0;
          printf("接收client操作指令为%s\n", cmd_info);
          char *cmd_op_info = cmd_info + 4;
          if (strcmp(cmd_op_info, "begin") == 0) {  // 开始接收
            printf("接收到开始接收信息\n");
            push_flag[i] = true;
            liveness[i] = HEARTBEAT_LIVENESS;
          } else if (strcmp(cmd_op_info, "stop") == 0) {  // 暂停接收
            printf("接收到暂停接收信息\n");
            push_flag[i] = false;
          } else if (strcmp(cmd_op_info, "close") == 0) {  // 模仿关闭client
            zmq_close(push_socket[i]);
            zmq_close(sub_socket[i]);
            push_socket[i] = NULL;
            push_flag[i] = false;
            sub_socket[i] = NULL;
            items[i].socket = NULL;
            printf("接收到close命令,客户端%d关闭\n", i);
            sleep(5);
          } else if (strcmp(cmd_op_info, "heart") == 0) {  // 模仿关闭client
            liveness[i] = HEARTBEAT_LIVENESS;
          } else {
            printf("无效指令\n");
          }
        } else if (--liveness[i] == 0) {
          // 检测活跃度
          zmq_close(push_socket[i]);
          zmq_close(sub_socket[i]);
          push_socket[i] = NULL;
          push_flag[i] = false;
          sub_socket[i] = NULL;
          items[i].socket = NULL;
          printf("模拟异常退出关闭进程, 客户端%d关闭\n", i);
          // sleep(5);
        }
        // printf();
      }
      if (!proxyFlag) {
        responder = zmq_socket(context, ZMQ_SUB);
        zmq_connect(responder, "tcp://10.66.150.1:8002");
        zmq_setsockopt(responder, ZMQ_SUBSCRIBE, "tcp://:", 6);
        itemAdd(items[CLIENT_NUM], responder);
        proxyFlag = true;
      }
    }
  } else {
    printf("申请代理失败\n");
    responder = zmq_socket(context, ZMQ_SUB);
    zmq_connect(responder, "tcp://10.66.150.1:8002");
    zmq_setsockopt(responder, ZMQ_SUBSCRIBE, "tcp://:", 6);
  }
  // // zmq_poll参数赋值
  // zmq_pollitem_t *items =
  //     (zmq_pollitem_t *)malloc((CLIENT_NUM + 1) * sizeof(zmq_pollitem_t));
  // for (int i = 0; i < CLIENT_NUM; i++) {
  //   itemAdd(items[i], NULL);
  // }
  // itemAdd(items[CLIENT_NUM], responder);
  // // push_socket套接字赋值
  // //  sockerAssign(push_socket);
  // for (int i = 0; i < CLIENT_NUM; i++) {
  //   push_socket[i] = NULL;
  //   push_flag[i] = false;
  // }

  int num = 1;
  while (1) {
    // 给每个连接的客户端发消息
    sendMessage(num++);

    ret = zmq_poll(items, CLIENT_NUM + 2, HEARTBEAT_INTERVAL);
    if (ret < 0) {
      printf("zmq_poll返回值为%d\n", ret);
      break;
    }
    printf("zmq_poll返回值为%d\n", ret);
    // 检测到连接
    if (items[CLIENT_NUM].revents & ZMQ_POLLIN) {
      for (int i = 0; i < CLIENT_NUM; i++) {
        if (push_socket[i] == NULL) {
          push_socket[i] = zmq_socket(context, ZMQ_PUSH);
          sub_socket[i] = zmq_socket(context, ZMQ_SUB);

          // 接收客户端发过来的pull套接字端口信息，发送消息到此套接字接收
          char client_pull_addr[26] = "";
          printf("waiting client %d connect......\n", i);
          ret = zmq_recv(responder, client_pull_addr, 25, 0);
          printf("接收返回值rc = %d, 接收到客户端%d发送的pull的ip和端口 = %s\n",
                 ret, i, client_pull_addr);
          // zmq_send(responder, "received pull", 13, 0);

          // push与pull建立连接
          ret = zmq_connect(push_socket[i], client_pull_addr);
          assert(ret == 0);
          // push_flag[i] = true;

          // 接收客户端发过来的pub套接字端口信息,接收此套接字发过来的开始,停止,结束等信息
          char client_sub_addr[26] = "";
          ret = zmq_recv(responder, client_sub_addr, 25, 0);
          printf("接收返回值rc = %d, 接收到客户端%d发送的pub的ip和端口 = %s\n",
                 ret, i, client_sub_addr);
          // zmq_send(responder, "received pub", 12, 0);

          // pub与sub建立连接,且设置订阅过滤信息
          ret = zmq_connect(sub_socket[i], client_sub_addr);
          assert(ret == 0);
          ret = zmq_setsockopt(sub_socket[i], ZMQ_SUBSCRIBE, "cmd:", 4);
          assert(ret == 0);

          // 将pub加入items
          itemAdd(items[i], sub_socket[i]);
          break;
        }
      }
    }
    // 检测到pub事件
    for (int i = 0; i < CLIENT_NUM; i++) {
      if (items[i].revents & ZMQ_POLLIN) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        char *cmd_info;
        int size = zmq_msg_recv(&msg, items[i].socket, 0);
        cmd_info = (char *)zmq_msg_data(&msg);
        cmd_info[zmq_msg_size(&msg)] = 0;
        printf("接收client操作指令为%s\n", cmd_info);
        char *cmd_op_info = cmd_info + 4;
        if (strcmp(cmd_op_info, "begin") == 0) {  // 开始接收
          printf("接收到开始接收信息\n");
          push_flag[i] = true;
          liveness[i] = HEARTBEAT_LIVENESS;
        } else if (strcmp(cmd_op_info, "stop") == 0) {  // 暂停接收
          printf("接收到暂停接收信息\n");
          push_flag[i] = false;
        } else if (strcmp(cmd_op_info, "close") == 0) {  // 模仿关闭client
          zmq_close(push_socket[i]);
          zmq_close(sub_socket[i]);
          push_socket[i] = NULL;
          push_flag[i] = false;
          sub_socket[i] = NULL;
          items[i].socket = NULL;
          printf("接收到close命令,客户端%d关闭\n", i);
          sleep(5);
        } else if (strcmp(cmd_op_info, "heart") == 0) {  // 模仿关闭client
          liveness[i] = HEARTBEAT_LIVENESS;
        } else {
          printf("无效指令\n");
        }
      } else if (--liveness[i] == 0) {
        // 检测活跃度
        zmq_close(push_socket[i]);
        zmq_close(sub_socket[i]);
        push_socket[i] = NULL;
        push_flag[i] = false;
        sub_socket[i] = NULL;
        items[i].socket = NULL;
        printf("模拟异常退出关闭进程, 客户端%d关闭\n", i);
        // sleep(5);
      }
      // printf();
    }
    sleep(1);
  }

#pragma endregion
#endif
  return 0;
}