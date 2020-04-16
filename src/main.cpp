#include <iostream>

#include <sstream>
#include <string>
#include <vector>
#include <thread>

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

	AhatLogger::setting("", "AhatDummyServer", 0);
	AhatLogger::start();
	
	if(argc < 2)
	{
		AhatLogger::INFO(CODE, "insert port number");
		std::cout << "insert port number\n";
		AhatLogger::stop();
		return 0;
	}
	
	std::vector<std::thread> threads;
	
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
		threads.push_back(std::thread(DummyServer, a));
	}
	AhatLogger::INFO(CODE, "AhatDummyServer start success");
	std::cout << "AhatDummyServer start success\n";
	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	AhatLogger::stop();
	return 0;
}
