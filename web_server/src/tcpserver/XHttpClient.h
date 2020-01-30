#pragma once
#include"XTcp.h"
#include"XHttpResonse.h"
//启动线程，就收用户数据，调用php生成结果返回给用户
class XHttpClient
{
public:
	bool Start(XTcp client);
	XTcp client;
	void Main();
	XHttpResonse res;
};

