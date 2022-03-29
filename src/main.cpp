/*
2020-04-29
AhatDummyServer 1.0.0 Version Realese
*/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <thread>
#include <stdio.h> 
#include <memory>

#include "DummyServer.h"
#include "ahatlogger.h"

int main(int argc, char *argv[]) 
{

#ifdef _WIN32
    unsigned int fd_num = 0;
#elif __linux__
	struct rlimit lim;
	getrlimit(RLIMIT_CORE, &lim);
	lim.rlim_cur = lim.rlim_max;
	setrlimit(RLIMIT_CORE, &lim);
    unsigned int j;
#endif
    unsigned int i, c = 0;
    int port_size = 0;
    int* port = new int[port_size];

    bool debug = false;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-g") == 0)
        {
            debug = true;
            continue;
        }
        port_size++;
    }

    if (debug)
    {
        AhatLogger::setting("logs", "AhatDummyServer", 0);
        AhatLogger::start();
        AhatLogger::DEBUG(CODE, "AhatDummyServer DEBUG MODE Start");
    }
    else
    {
        AhatLogger::setting("logs", "AhatDummyServer", 1);
        AhatLogger::start();
    }

    if (argc < 2)
    {
        AhatLogger::INFO(CODE, "insert port number");
        std::cout << "insert port number\n";
        AhatLogger::stop();
        return 0;
    }

    int idx = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-g") == 0)
        {
            continue;
        }

        std::stringstream str;
        str << argv[i];
        str >> port[idx];

        if (port[idx] < 0 || port[idx] > 65535)
        {
            AhatLogger::ERR(CODE, "%d is bad request!", port);
            AhatLogger::stop();
            return 0;
        }
        AhatLogger::DEBUG(CODE, "Server port %d : %d", i, port[idx]);

        idx++;
    }
    
    AhatLogger::INFO(CODE, "AhatDummyServer start success");
    std::cout << "AhatDummyServer start success\n";

    DummyServer* server = new DummyServer();
    server->start(port_size, port);
    
    AhatLogger::stop();
    return 0;
}