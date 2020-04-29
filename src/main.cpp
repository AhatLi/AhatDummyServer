#include <iostream>

#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <thread>
#include <stdio.h> 
#include <memory>

#ifdef _WIN32
#include <winsock2.h> 
#elif __linux__
#include <sys/epoll.h>
#define EPOLL_SIZE 20
#endif

#include "DummyServer.h"
#include "ahatlogger.h"

int main(int argc, char *argv[]) 
{
#ifdef __linux__
	struct rlimit lim;
	getrlimit(RLIMIT_CORE, &lim);
	lim.rlim_cur = lim.rlim_max;
	setrlimit(RLIMIT_CORE, &lim);
#endif
    int c = 0;

	AhatLogger::setting("", "AhatDummyServer", 0);
	AhatLogger::start();
	
	if(argc < 2)
	{
		AhatLogger::INFO(CODE, "insert port number");
		std::cout << "insert port number\n";
		AhatLogger::stop();
		return 0;
	}
	
	std::vector<std::shared_ptr<DummyServer> > threads;

    int core = std::thread::hardware_concurrency();
    if (core < 2)
        core = 2;
    for (int i = 0; i < core - 1; i++)
    {
        auto server = std::make_shared<DummyServer>();
        threads.push_back(server);
        std::thread t(&DummyServer::start, server);
        t.detach();
    }
	
	AhatLogger::INFO(CODE, "AhatDummyServer start success");
	std::cout << "AhatDummyServer start success\n";


#ifdef _WIN32
    WSADATA wsaData;
    int addrlen;
    SOCKADDR_IN serveraddr, clientaddr;
    fd_set old_fds, new_fds;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup Error\n";
        AhatLogger::ERR(CODE, "WSAStartup Error");
        AhatLogger::stop();
        return 0;
    }
    FD_ZERO(&new_fds);
#elif __linux__
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen;

    int eventn;
    int epollfd;
    struct epoll_event ev, *events;

    events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    if ((epollfd = epoll_create(100)) == -1)
    {
        std::cout << "epoll_create Error!\n";
        AhatLogger::ERR(CODE, "epoll_create Error!");
        AhatLogger::stop();
    }
#endif

    int* listen_sock = new int[argc - 1];
    int *port = new int[argc - 1];
    int client_sock;
    unsigned int i, j, fd_num = 0;
    
    for (int i = 0; i < argc - 1; i++)
    {
        std::stringstream str;
        str << argv[i+1];
        str >> port[i];

        if (port[i] < 0 || port[i] > 65535)
        {
            AhatLogger::ERR(CODE, "%d is bad request!", port);
            AhatLogger::stop();
            return 0;
        }

#ifdef _WIN32
        listen_sock[i] = socket(PF_INET, SOCK_STREAM, 0);
        if (listen_sock[i] == INVALID_SOCKET)
        {
            std::cout << "Socket Error!\n";
            AhatLogger::ERR(CODE, "Socket Error!");
            AhatLogger::stop();
            return 0;
        }
#elif __linux__
        struct sockaddr_in clientaddr;
        socklen_t addrlen;
        listen_sock[i] = socket(PF_INET, SOCK_STREAM, 0);
        if (listen_sock[i] < 0)
        {
            std::cout << "Socket Error!\n";
            AhatLogger::ERR(CODE, "Socket Error!");
            AhatLogger::stop();
            return 0;
        }
#endif
        memset((void*)&serveraddr, 0x00, sizeof(serveraddr));

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port[i]);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(listen_sock[i], (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1)
        {
            AhatLogger::ERR(CODE, "%d port socket error", port);
            AhatLogger::stop();
            return 0;
        }

        if(listen(listen_sock[i], SOMAXCONN) == -1)
        {
            AhatLogger::ERR(CODE, "%d port listen error", port);
            AhatLogger::stop();
            return 0;
        }


#ifdef _WIN32
        FD_SET(listen_sock[i], &new_fds);
#elif __linux__    
        ev.events = EPOLLIN;
        ev.data.fd = listen_sock[i];
        epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock[i], &ev);
#endif
    }

    while (1)
    {
#ifdef _WIN32
        old_fds = new_fds;
        fd_num = select(0, &old_fds, NULL, NULL, NULL);
#elif __linux__    
        eventn = epoll_wait(epollfd, events, EPOLL_SIZE, -1);
#endif
        for (i = 0; i < argc - 1; i++)
        {
#ifdef _WIN32
            if (FD_ISSET(listen_sock[i], &old_fds))
            {
#elif __linux__    
            for (j = 0; j < eventn; j++)
            {
                if (events[j].data.fd == listen_sock[i])    // 듣기 소켓에서 이벤트가 발생함
                {
#endif
                addrlen = sizeof(clientaddr);
                client_sock = accept(listen_sock[i], (struct sockaddr*) & clientaddr, &addrlen);
                if (client_sock < 0)
                {
                    continue;
                }

                InReqItem reqitem(inet_ntoa(clientaddr.sin_addr), std::to_string(port[i]), "");
                threads[c++]->Enqueue(client_sock, reqitem);
                if (core <= c + 1)
                    c = 0;
#ifdef __linux__    
                }
#endif
            }
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif
    AhatLogger::stop();
    return 0;
}