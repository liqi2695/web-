#include<iostream>
#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include"XTcp.h"
#include"Alloctor.h"
#include<signal.h>
using namespace std;

#include"XHttpServer.h"
int main(int argc, char* argv[])
{
	signal(SIGPIPE,SIG_IGN);
	unsigned short port = 8081;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}
	XHttpServer server;
	server.Start(port);
	getchar();
	return 0;
}