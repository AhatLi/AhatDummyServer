#include <iostream>

#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <sys/resource.h>

#include "DummyServer.h"
#include "ahatlogger.h"

int main(int argc, char *argv[]) 
{
	struct rlimit lim;
	getrlimit(RLIMIT_CORE, &lim);
	lim.rlim_cur = lim.rlim_max;
	setrlimit(RLIMIT_CORE, &lim);

	AhatLogger::setting("", "AhatDummyServer", 0);
	AhatLogger::start();

	
	if(argc < 2)
	{
		AhatLogger::INFO(CODE, "insert port number");
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
			AhatLogger::ERROR(CODE, "%d is bad request!", a);
			return 0;
		}
		//추후 스레드 관리를 위하여 vector에 저장
		threads.push_back(std::thread(DummyServer, a));
	}
	
	AhatLogger::INFO(CODE, "AhatDummyServer start success");
	while(1)
	{
		;
	}
	
	return 0;
}
