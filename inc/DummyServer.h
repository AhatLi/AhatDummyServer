#ifndef __AHAT_DUMMYSERVER_H__
#define __AHAT_DUMMYSERVER_H__

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <map>

#include <unistd.h>
#include <fcntl.h>

/*
string makeBody_HitcountSize(string group);
string makeBody_HitcountList(string group);
string makeBody_Post();
string makeBody_FileInfo(string filename);
*/
std::string getFileData(std::string filepath, int port);
std::string makeResult(char* msg, int port);
std::string makeHeader(std::string body);
int client_connect(int client_sock, int port);
int DummyServer(int port);
#endif
