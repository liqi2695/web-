#include "XHttpResonse.h"
#include<string>
#include<regex>
#include<iostream>
#define _CRT_SECURE_NO_WARNINGS


using namespace std;
//计算文件大小
bool XHttpResonse::SetRequest(std::string request)
{
	string src = request;
	// /  /index.html  //ff/index.php 
	string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1";
	regex r(pattern);
	smatch mas;
	regex_search(src, mas, r);
	if (mas.size() == 0)
	{
		cout << pattern << " failed" << endl;
		return false;
	}
	//取到的路径结果
	string type = mas[1];//GET
	string path = "/";
	path += mas[2];//index.html
	string filetype = mas[3];
	string query = mas[4];

	if (type != "GET")
	{
		cout << "Not GET!!!" << endl;

		return false;

	}
	string filename = path;
	if (path == "/")
	{
		filename = "/index.html";
	}
	string filepath = "www";
	filepath += filename;

	if (filetype == "php")
	{
		string cmd = "php-cgi ";
		cmd += filepath;
		cmd += " ";
		//query id=1&name=xcj
		//id = 1 name = xcj
		for (int i = 0; i < query.size(); i++)
		{
			if (query[i] == '&') query[i] = ' ';
		}
		cmd += query;

		cmd += " > ";
		filepath += ".html";
		cmd += filepath;
		//上面语句的意思就是php-cgi www/index.php  > www/index.php.html
		printf("%s\n", cmd.c_str());
		system(cmd.c_str());//C语言自带函数，可以直接执行指令
	}

	fp = fopen(filepath.c_str(), "rb");
	if (fp == NULL)
	{
		cout << "open file failed" << filetype.c_str() << endl;
		return false;
	}

	//获取文件大小
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);//会存在int的大小问题
	fseek(fp, 0, 0);
	cout << "file size is:" << filesize << endl;

	//处理头
	if (filetype == "php")
		//只有是php文件才进行头处理，不影响其他静态文件的访问
	{
		char c = 0;
		//\r\n\r\n
		int headsize = 0;
		while (fread(&c, 1, 1, fp) > 0)
		{
			headsize++;
			if (c == '\r')
			{
				fseek(fp, 3, SEEK_CUR);
				//碰见/r向后移动三位，然后就到正文了
				headsize += 3;
				break;
			}
		}
		filesize = filesize - headsize;//将filesize的值进行改变
	}

	return true;
}
//回应请求，设置消息头，。获取头信息
std::string XHttpResonse::GetHead()
{
	//回应http，GET请求
    //消息头
	string rmsg = "";
	rmsg = "HTTP/1.1 200 OK\r\n"; //协议
	rmsg += "Server:XHttp\r\n";//用\r\rn结束
	rmsg += "Content-Type: text/html\r\n";
	rmsg += "Content-Length: ";
	char bsize[128] = { 0 };

	sprintf(bsize, "%d", filesize);//************



	
	rmsg += bsize;
	//rmsg += "10\r\n";//控制发送的大小 
	rmsg += "\r\n\r\n";
	//消息内容
	//rmsg += "0123456789";
	//发送消息头
	return rmsg;
}

//正文
int XHttpResonse::Read(char* buf, int bufsize)
{
	//发送正文
	return  fread(buf, 1, bufsize, fp);
}