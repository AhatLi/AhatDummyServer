#ifndef __AHAT_DUMMYSERVER_H__
#define __AHAT_DUMMYSERVER_H__

#include <iostream>
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <string>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <map>
#include <queue>

#include <fcntl.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <direct.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <WS2tcpip.h>

#define read(X, Y, Z) _read(X, Y, Z)
#define close(X) _close(X)

#pragma comment(lib, "ws2_32.lib")
#elif __linux__
#include <sys/resource.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#endif

#include "HTTPMessage.h"
#include "ahatlogger.h"

class DummyServer
{
	std::string makeResult(char* msg, int port, HTTPMessage message, InReqItem& reqitem);
	int client_connect(int client_sock, InReqItem reqitem);
	std::string getFileData(std::string filepath, int port, HTTPMessage message);

	std::queue<std::pair<int, InReqItem> > q;
public:
	int start();
	void Enqueue(int client_sock, InReqItem reqitem);
	std::pair<int, InReqItem> Dequeue();
};


std::string trim(std::string str);
int closeOsSocket(int socket);
char* strtok_all(char* _String, const char* _Delimiter, char** _Context);

#endif