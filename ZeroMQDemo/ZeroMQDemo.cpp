// ZeroMQDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "zmq.h"
#include "stdio.h"

void Demo1();
void Demo2();

int _tmain(int argc, _TCHAR* argv[])
{
    //Demo1();
    Demo2();
	return 0;
}

void Demo1()
{
    printf("Connecting to hello world server...\n");
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; ++request_nbr)
    {
        char buffer[10];
        printf("Sending Hello %d...\n", request_nbr);
        zmq_send(requester, "Hello", 5, 0);
        zmq_recv(requester, buffer, 10, 0);
        printf("Received World %d\n", request_nbr);
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);
}

void *zmq_socket_new (void *context)
{
    int linger = 1;
    void *zsocket = zmq_socket (context, ZMQ_REQ);
    zmq_setsockopt(zsocket, ZMQ_LINGER, &linger, sizeof(linger));
    zmq_connect (zsocket, "tcp://localhost:5555");
    return zsocket;
}

void Demo2()
{
    void *context = zmq_ctx_new ();
    void *zsocket = zmq_socket_new(context);

    char buffer [255];
    char *send_s = "Hello";
    int retries_left = 3;

    while (retries_left) {
        // 发送消息
        zmq_send (zsocket, send_s, strlen(send_s), 0);
        printf ("Send %s\n", send_s);
        // 重试次数
        int expect_reply = 1;
        while (expect_reply) {
            // 停止重试
            if (retries_left == 0) {
                printf("Server offline, abandoning ...\n");
                break;
            }
            // 多路复用
            zmq_pollitem_t items [] = { { zsocket, 0, ZMQ_POLLIN, 0 } };
            int rc = zmq_poll (items, 1, 3000);
            if (rc == -1) break; // Interrupted
            if (items [0].revents & ZMQ_POLLIN) {
                // 接收反馈
                int size = zmq_recv (zsocket, buffer, 10, 0);
                if (size > 255) size = 255;
                buffer[size] = 0;
                printf ("Recv %s\n", buffer);
                if (buffer) {
                    retries_left = 3;
                    expect_reply = 0;
                }
            }
            // 重试连接
            else {
                printf("Retry connecting ...\n");
                zmq_close (zsocket);
                zsocket = zmq_socket_new(context);
                // 重发消息
                zmq_send (zsocket, send_s, strlen(send_s), 0);
                printf ("Send %s\n", send_s);
                --retries_left;
            }
        }
    }
    zmq_close (zsocket);
    zmq_ctx_destroy (context);
}
