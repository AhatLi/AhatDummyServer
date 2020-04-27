#include <iostream>

#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h> 
#include <thread>
#include <stdio.h> 

#include "DummyServer.h"
#include "ahatlogger.h"



#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[]) 
{
#ifdef __linux__
	struct rlimit lim;
	getrlimit(RLIMIT_CORE, &lim);
	lim.rlim_cur = lim.rlim_max;
	setrlimit(RLIMIT_CORE, &lim);
#endif

	AhatLogger::setting("", "AhatDummyServer", 0);
	AhatLogger::start();
	
	if(argc < 2)
	{
		AhatLogger::INFO(CODE, "insert port number");
		std::cout << "insert port number\n";
		AhatLogger::stop();
		return 0;
	}
	
	std::vector<DummyServer*> threads;

	DummyServer* server = new DummyServer();
	threads.push_back(server);

    std::thread t(&DummyServer::start, server);
	
	for(int i = 1; i < argc; i++)
	{
		std::stringstream str;
		str << argv[i];
		int a = 0;
		str >> a;
		
		if(a < 0 || a > 65535)
		{
			AhatLogger::ERR(CODE, "%d is bad request!", a);
			return 0;
		}
		//추후 스레드 관리를 위하여 vector에 저장
	}
	AhatLogger::INFO(CODE, "AhatDummyServer start success");
	std::cout << "AhatDummyServer start success\n";









    WSADATA wsaData;
    SOCKET listen_fd[1], accept_fd, max_fd = 0, sock_fd;
    struct sockaddr_in listen_addr, accept_addr;

    int readn, addr_len;
    unsigned int i, j, fd_num = 0;

    fd_set old_fds, new_fds;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 1;

    int port = 5555;
    int num = 2;

    FD_ZERO(&new_fds);
    for (i = 0; i < num; i++)
    {
        listen_fd[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd[i] == INVALID_SOCKET)
            return 1;

        memset((void*)&listen_addr, 0x00, sizeof(listen_addr));

        listen_addr.sin_family = AF_INET;
        listen_addr.sin_port = htons(port++);
        listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(listen_fd[i], (struct sockaddr*) & listen_addr, sizeof(listen_addr)) == SOCKET_ERROR)
            return 1;

        if (listen(listen_fd[i], SOMAXCONN) == SOCKET_ERROR)
            return 1;

        FD_SET(listen_fd[i], &new_fds);
    }

    while (1)
    {
        int testi = 0;
        old_fds = new_fds;

        printf("accept wait %d\n", new_fds.fd_count);

        for (i = 2; i <= new_fds.fd_count; i++)
        {
            printf("pd = %d\n", new_fds.fd_array[i]);
        }

        fd_num = select(0, &old_fds, NULL, NULL, NULL);

        for (i = 0; i < num; i++)
        {
            if (FD_ISSET(listen_fd[i], &old_fds))
            {
                addr_len = sizeof(struct sockaddr_in);
                accept_fd = accept(listen_fd[i], (struct sockaddr*) & accept_addr, &addr_len);
                if (accept_fd == INVALID_SOCKET)
                {
                    continue;
                }

                server->q.push(accept_fd);
            }
        }
    }
    closesocket(listen_fd[0]);
    WSACleanup();
    return 0;







	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	AhatLogger::stop();
	return 0;
}
