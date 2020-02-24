#pragma once
#include<string>
class XHttpResonse
{
public:
	bool SetRequest(std::string request);
	std::string GetHead();
	int Read(char* buf, int bufsize);
private:
	int filesize = 0;
	FILE* fp = NULL;

	
};

