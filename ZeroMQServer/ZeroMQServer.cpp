// ZeroMQServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "zmq.h"
#include "assert.h"

int _tmain(int argc, _TCHAR* argv[])
{
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(responder, "tcp://*:5555");
    assert(rc == 0);

    while (1)
    {
        char buffer[10];
        zmq_recv(responder, buffer, 10 , 0);
        printf("Received Hello\n");
        zmq_send(responder, "World", 5, 0);
        Sleep(1);
    }
	return 0;
}

