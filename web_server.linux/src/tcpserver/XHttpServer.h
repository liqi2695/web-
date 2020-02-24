#pragma once
#include"XTcp.h"
#include"Alloctor.h"
class XHttpServer
{
public:                                                                     
	bool Start(unsigned short port);
	XTcp server;
	void Main();     
	void Stop(); 
	bool isexit = false;
};

