#include "XHttpServer.h"
#include "XHttpClient.h"
#include<thread>
#include<iostream>
#include<sys/epoll.h>
using namespace std;
void XHttpServer::Stop()
{
	isexit = true;
}
void XHttpServer::Main()
{
	// while (!isexit)
	// {
	// 	int count = epoll_wait(epfd,event,20,500);
	// 	if(count <= 0) continue;
	// 	for(int i=0;i<count;i++)
	// 	{
	// 		if(event[i].data.fd == server.sock)
	// 		{
	// 			XTcp client = server.Accept();
	// 			if (client.sock <= 0) continue;
	// 			ev.data.fd = client.sock;
	// 			ev.events = EPOLLIN|EPOLLET;
	// 			epoll_ctl(epfd,EPOLL_CTL_ADD,client.sock,&ev);
	// 		}		
	// 	}
	// 	// XTcp client = server.Accept();
	// 	//  if (client.sock <= 0) continue;
	// 	XHttpClient *th = new XHttpClient();
	// 	th->Start(client);
	// }
}
bool XHttpServer::Start(unsigned short port)
{
	int epfd = epoll_create(256);
	epoll_event ev;
	ev.data.fd = server.sock;
	ev.events = EPOLLIN|EPOLLET;
	epoll_ctl(epfd,EPOLL_CTL_ADD,server.sock,&ev);
	epoll_event event[20];
	
	isexit = false;
	server.CreateSocket();
	if (!server.Bind(port))  return false;
	// thread sth(&XHttpServer::Main, this);
	// sth.detach();
	while (!isexit)
	{
		int count = epoll_wait(epfd,event,20,500);
		if(count <= 0) continue;
		XTcp client = server.Accept();
		for(int i=0;i<count;i++)
		{
			if(event[i].data.fd == server.sock)
			{
				
				if (client.sock <= 0) continue;
				ev.data.fd = client.sock;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_ADD,client.sock,&ev);

			}	
			else
			{
				XHttpClient *th = new XHttpClient();
				th->Start(client,epfd,ev);
			}	
		}
		// XTcp client = server.Accept();
		//  if (client.sock <= 0) continue;

	}


	return true;
}
