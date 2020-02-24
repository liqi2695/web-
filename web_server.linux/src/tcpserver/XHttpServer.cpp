#include "XHttpServer.h"
#include "XHttpClient.h"
#include<thread>
#include<iostream>
using namespace std;
void XHttpServer::Stop()
{
	isexit = true;
}
void XHttpServer::Main()
{
	while (!isexit)
	{
		XTcp client = server.Accept();
		if (client.sock <= 0) continue;
		XHttpClient *th = new XHttpClient();
		int i=0;
		cout<<"中文测试"<<i++<<endl;
		th->Start(client);
	}
}
bool XHttpServer::Start(unsigned short port)
{


	isexit = false;
	server.CreateSocket();
	if (!server.Bind(port))
	{ 	
		WriteLog("bind faild XHttpServer.cpp 30");
		return false;
	}	
	WriteLog("bind success XHttpServer.cpp 30");
	thread sth(&XHttpServer::Main, this);
	sth.detach();


	return true;
}
