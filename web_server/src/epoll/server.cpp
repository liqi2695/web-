
#include<iostream>
#include<stdlib.h>
#include"XTcp.h"
#include<thread>
#include<string.h>
#include<stdio.h>
using namespace std;
class TcpThread
{
public:
	void Main()
	{

		char buf[1024] = { 0 };
		for (;;)
		{
			int recvlen = client.Recv(buf, sizeof(buf) - 1);
			if (recvlen <= 0) break;

			buf[recvlen] = '\0';
			if (strstr(buf, "quit") != NULL)
			{
				char re[] = "quit success!\n";
				client.Send(re, strlen(re) + 1);
				break;
			}
			int sendlen = client.Send("ok\n", 3);
			cout << "recv:" << buf << endl;
		}
		client.Close();
		delete this;
	}
	XTcp client;
};
#include<sys/epoll.h>
int main(int argc, char* argv[])
{
	XTcp server;

	unsigned short port = 8080;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}

	server.CreateSocket();
	server.Bind(port);
    //创建epoll,里面最多放256个套接字
	int epfd = epoll_create(256);

	//注册事件
	epoll_event ev;
	ev.data.fd = server.sock;//将此sock注册进事件
	ev.events = EPOLLIN|EPOLLET;//ET边缘触发,相比于水平出发效率较高
	epoll_ctl(epfd,EPOLL_CTL_ADD,server.sock,&ev);
	epoll_event events[20];//20：这次最多等待20个
	char buf[1024] ={0};
	const char *msg ="HTTP/1.1 200 OK\r\nContent-Length: 1\r\n\r\nX";
	int size = strlen(msg);
	server.SetBlock(false);

	for (;;)
	{
		int count = epoll_wait(epfd,events,20,500);//超过500ms就朝下走,不会一直阻塞
		if(count <= 0) continue;
		int i=0;
		for(i=0;i < count;i++)
		{
			if(events[i].data.fd == server.sock)
			{
				for(;;)
				{
					XTcp client = server.Accept();
					if(client.sock<=0) break;
					ev.data.fd = server.sock;
					ev.events = EPOLLIN|EPOLLET;
					epoll_ctl(epfd,EPOLL_CTL_ADD,client.sock,&ev);
					//增加
				}
			}
			else
			{
				XTcp client;
				client.sock = events[i].data.fd;
				client.Recv(buf,1024);
				client.Send(msg,size);
				epoll_ctl(epfd,EPOLL_CTL_DEL,client.sock,&ev);
				//删除
				client.Close();
			}
					
		}
		/*XTcp client = server.Accept();

		//���ǵ��̴߳����Ķ�����˭�����������Լ�����or�����߳�����
		//��������ѡ��ͨ���Լ�������
		TcpThread* th = new TcpThread();
		th->client = client;
		//�����߳�
		thread sth(&TcpThread::Main, th);
		//���̲߳�Ҫ�������̵߳Ĳ���������ֹͣ��������һ�㲻ȥ����
		//�ͷ����߳�ӵ�е����߳���Դ
		sth.detach();*/
	}
	server.Close();
	getchar();
	return 0;
}