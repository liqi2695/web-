#pragma once
#include"XTcp.h"
#include"XHttpResonse.h"
#include"Alloctor.h"
#include<sys/epoll.h>
class XHttpClient
{
public:
	bool Start(XTcp client,int epfd,epoll_event ev);
	XTcp client;
	void Main();
	XHttpResonse res;
};

