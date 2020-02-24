#pragma once
#include"XTcp.h"
#include"XHttpResonse.h"
#include"Alloctor.h"
class XHttpClient
{
public:
	bool Start(XTcp client);
	XTcp client;
	void Main();
	XHttpResonse res;
};

