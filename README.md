# Introduction
* web服务器-采用c++11网络编程编写的可支持PHP脚本的web服务器

# Envoirment
* OS: Ubuntu 18.04.2
* Complier: g++ 7.4.0

# Technical points
* 采用多线程，支持多个客户端连接
* 支持解析PHP脚本
* 使用正则表达式获取请求页面文件


# Run
* 在xscoket中输入指令make，编译动态链接库
* 然后在tcpserver中输入指令make，运行服务器

