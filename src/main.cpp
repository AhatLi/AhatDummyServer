#include <iostream>

#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <sys/resource.h>
#include <python3.8/Python.h>

#include "DummyServer.h"

int main(int argc, char *argv[]) 
{
    Py_Initialize();
    PyRun_SimpleString("print ('Hello, world!')");

    Py_Finalize();


	struct rlimit lim;
	getrlimit(RLIMIT_CORE, &lim);
	lim.rlim_cur = lim.rlim_max;
	setrlimit(RLIMIT_CORE, &lim);
	
	if(argc < 2)
	{
		std::cout<<"insert port number\n";
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
			std::cout<<a<<" is bad request!\n";
			return 0;
		}
		//추후 스레드 관리를 위하여 vector에 저장
		threads.push_back(std::thread(DummyServer, a));
	}
	
	while(1)
	{
		;
	}
	
	return 0;
}
