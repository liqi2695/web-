#ifndef XTcp_H
#define XTcp_H
//什么意思，表示头文件只能被调用一次
#ifdef WIN32
#ifdef XSOCKET_EXPORTS
#define XSOCKET_API __declspec(dllexport)//�����˺�
#else
#define XSOCKET_API __declspec(dllimport)
#endif
#else
#define XSOCKET_API
#endif

#include<string>
class XTcp
{
public:
	//创建socket
	int CreateSocket();
	//绑定
	bool Bind(unsigned short port);

	XTcp Accept();
	//返回值为对象，所以不能在析构中关闭。会生成多个对象，返回为对象就对导致内部定义的
	//对象无法被析构。  解决方法：再定义一个关闭的函数
	
	void Close();

	//cs数据交互
	int Recv(char *buf, int bufsize);
	int Send(const char* buf, int size);

	bool Connect(const char *ip,unsigned short port,int timeoutms = 1000);
	//最后一个超时参数
	bool SetBlock(bool isblock);//设置阻塞与非阻塞
	
	XTcp();
	//virtual  ~XTcp();
	//sock,port,ip
	int sock;
	unsigned short port;
	char ip[16];
};
#endif

