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
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <direct.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

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
	int client_connect(SOCKET client_sock, char* ip, int port);
	std::string getFileData(std::string filepath, int port, HTTPMessage message);
public:
	int start();

	std::queue<SOCKET> q;
};


std::string trim(std::string str);
int closeOsSocket(int socket);
char* strtok_all(char* _String, const char* _Delimiter, char** _Context);

#endif