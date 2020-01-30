#include "XTcp.h"

//ͷ�ļ������þ�������cpp�У���Ϊ����һ�㶼�ǵ���.h�ļ����������.h�ļ���
//��������ɽ���Ӱ��
#ifdef WIN32
#include<Windows.h>
#define socklen_t int
//Ϊ�˽��window������û��socklen_t�Ĵ���
#else
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<fcntl.h>
#define closesocket close
#endif 
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<thread>
using namespace std;

XTcp::XTcp()
{
#ifdef WIN32
	static bool first = true;
	if (first)
	{
		first = false;
		WSADATA ws;
		WSAStartup(MAKEWORD(2, 2), &ws);
	}
#endif 
}

int XTcp::CreateSocket()
{
	//
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		cout << "create socket failed" << endl;
		//return -1;
	}
	cout << "sock:" << sock << endl;
	return sock;
}
bool XTcp::Bind(unsigned short port)
{
	if (sock <= 0)
		CreateSocket();
	struct sockaddr_in caddr;
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(port);
	caddr.sin_addr.s_addr = htonl(0);

	//3��bind��������ip�Լ�port
	if (bind(sock, (struct sockaddr*) & caddr, sizeof(caddr)) != 0)
	{
		cout << "bind port ip failed" << port << endl;
		return false;
	}
	cout << "bind port success:" << port << endl;
	//4������listenתΪ�ɽ�������״̬
	listen(sock, 10);  //�ڶ����������׽���ʹ�õĶ��г���
	return true;
}
void XTcp::Close()
{
	if (sock <= 0) return;
	closesocket(sock);
}
//每次创建一个XTcp对象，
XTcp XTcp::Accept()
{
	XTcp tcp;
	sockaddr_in saddr;
	socklen_t len = sizeof(saddr);
	int client = accept(sock, (sockaddr*)&saddr, &len);
	if (client <= 0) return tcp;
	cout << "accept client:" << client << endl;
	char *ip = inet_ntoa(saddr.sin_addr);
	strcpy(tcp.ip, ip);
    tcp.port = ntohs(saddr.sin_port);
	tcp.sock = client;
	cout << "ip:" << tcp.ip << "cport:" << tcp.port << endl;
	return tcp;
}
int XTcp::Recv(char *buf, int bufsize)
{
	return recv(sock, buf, bufsize, 0);
}
int XTcp::Send(const char* buf, int size)
{
	int sendedsize = 0;
	while (sendedsize != size)
	{
		int len = send(sock, buf + sendedsize, size - sendedsize, 0);
		if (len <= 0) break;
		sendedsize += len;
	}
	return sendedsize;
}
bool XTcp::Connect(const char *ip,unsigned short port,int timeoutms)
{
	if(sock <= 0 ) CreateSocket();
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	//将本地字节序转换为网络字节序，如果写成htonl就会出现“无法分配请求的地址的错误”
	saddr.sin_addr.s_addr = inet_addr(ip);
	SetBlock(false);
	fd_set set;
	//文件句柄的数组，使用select
	if(connect(sock , (sockaddr*)&saddr , sizeof(saddr)) != 0)
	{
		FD_ZERO(&set);
		FD_SET(sock, &set);
		timeval tm;//设置超时，我可以等你，但我不会一直等你
		tm.tv_sec = 0;
		tm.tv_usec = timeoutms * 1000;
		//select调用set，只要其中有一个就可以返回；select本身阻塞函数
		if (select(sock + 1, 0, &set, 0, &tm) <= 0)
		{
			cout << "connect timeout or errot" << endl;
			cout << "connect " << ip << " " << port << "failed! ";
			printf("error %s", strerror(errno));
			cout << endl;
			return false;
		}
	}
	SetBlock(true);
	cout << "connect" << ip << " " << port << "success!";
	cout<<endl;
	return true;
}
bool XTcp::SetBlock(bool isblock)
{
	if(sock <= 0) return false;
#ifdef  WIN32
	unsigned long ul = 0;
	if (!isblock) ul = 1;
	ioctlsocket(sock, FIONBIO, &ul);//ul为0表示阻塞模式,1代0表非阻塞模式
#else
	int flags = fcntl(sock, F_GETFL , 0);//获取属性
	if (flags < 0) return false;
	//设置成阻塞模式
	if (isblock)
	{
		flags = flags&~O_NONBLOCK;//取反
		//~O_NONBLOCK：将除了当前非阻塞位变为0，其它位置全部置为1，再与原来的flags做与操作，
		//所以执行上条语句，非阻塞位置为0，那么就是阻塞模式了
	}
	//设置成非阻塞模式
	else
	{
		flags = flags | O_NONBLOCK;//非阻塞模式。非阻塞位置为1
	}
	//上面仅仅改变成员变量，再使用下面函数将flags设置进去
	if (fcntl(sock, F_SETFL, flags) != 0) return false;
	

#endif

	return true;
}