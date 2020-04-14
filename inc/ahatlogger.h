#ifndef AHATLOGGER_H_
#define AHATLOGGER_H_

/*
Ahat Logger Version 1.0.1.QR2
*/

#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <stdarg.h>

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ahatloggeritem.h"

#ifdef _WIN32
#include <tchar.h>
#define strncpy(X, Y, Z) strncpy_s(X, Y, Z)
#define __FILENAME__    strrchr(__FILE__, '\\') +1
#elif __linux__
#include <dirent.h>
#define vsprintf_s(W, X, Y, Z) vsprintf(W, Y, Z)
int _vscprintf (const char * format, va_list pargs);
#define __FILENAME__    __FILE__
#endif

#define CODE code(__FILENAME__, __FUNCTION__, __LINE__)

std::string code(std::string file, std::string func, int line);

class AhatLogger
{
private:
	static std::string path;

	static std::string name;
	static std::string version;
	static std::string host;
	
	static std::queue< std::pair<std::string, std::string> > *q;
	static bool isStarted;
	static bool isFinished;
	static int level;

	static void run();
	static std::string getDate();

	static int makeDirectory(const char *full_path);
	static bool existDirectory(const char *path);
	
	static void logWrite();
public:
	static std::mutex mutex;

	static void setting(std::string path, std::string name, int level);
	static void start();
	static void stop();
	
	static void INFO(std::string src_file, const char* body, ...);
	static void ERR(std::string src_file, const char* body, ...);
	static void DEBUG(std::string src_file, const char* body, ...);

	static void CUSTOM(std::string src_file, std::string custom, const char* body, ...);

	static void REQUEST(std::string src_file, std::string req_ip, std::string req_uri);
	static void RESPONSE(std::string src_file, std::string res_body, std::string res_code);

	static void DB(std::string src_file, InDBtem db_req_item, std::string db_res_body);
	static void DB_ERR(std::string src_file, InDBtem db_req_item, std::string db_res_body);
	static void DB_DEBUG(std::string src_file, InDBtem db_req_item, std::string db_res_body);
	static void DB_ERR_DEBUG(std::string src_file, InDBtem db_req_item, std::string db_res_body);
	
	static void IN_REQ(std::string src_file, InReqItem in_req_item, std::string in_res_body);
	static void IN_REQ_ERR(std::string src_file, InReqItem in_req_item, std::string in_res_body);
	static void IN_REQ_DEBUG(std::string src_file, InReqItem in_req_item, std::string in_res_body);
	static void IN_REQ_ERR_DEBUG(std::string src_file, InReqItem in_req_item, std::string in_res_body);
};




#endif
