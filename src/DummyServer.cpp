#include "DummyServer.h"
#include "HTTPMessage.h"

std::string apiPath = "";

int DummyServer(int port) 
{
	AhatLogger::INFO(CODE, "%d thread is start!", port);
	std::cout << port << "thread is start!\n";
	int retval = 0;
	int client_sock = 0;  

#ifdef _WIN32
	WSADATA wsaData;
	int addrlen;
	SOCKADDR_IN clientaddr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("winsock error!\n");
		exit(1);
	}
	SOCKET listen_sock = socket(PF_INET, SOCK_STREAM, 0);
#elif __linux__
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	int listen_sock = socket(PF_INET, SOCK_STREAM, 0);
#endif

	if (listen_sock == -1) 
	{
		AhatLogger::ERR(CODE, "%d port socket error", port);
		return 0;
	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	auto l = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == -1) 
	{
		AhatLogger::ERR(CODE, "%d port socket error", port);
		return 0;
	}

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == -1) 
	{
		AhatLogger::ERR(CODE, "%d port listen error", port);
		return 0;
	}

#ifdef _WIN32
	wchar_t tmp[260];
	int len = GetModuleFileName(NULL, tmp, MAX_PATH);
	std::wstring ws(tmp);
	std::string buf(ws.begin(), ws.end());
	buf = buf.substr(0, buf.find_last_of("."));
#elif __linux__
	char buf[256];
	int len = readlink("/proc/self/exe", buf, 256);
	buf[len] = '\0';
#endif
	apiPath = buf;
	apiPath += "API";

	addrlen = sizeof(clientaddr);
	while (1)
	{
		client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
		if (client_sock == -1)  
		{
			AhatLogger::ERR(CODE, "%d port connect error", port);
			return 0;
		}

		std::thread t(client_connect, client_sock, inet_ntoa(clientaddr.sin_addr), port);
		t.detach();
	}

	closeOsSocket(listen_sock);
}

int closeOsSocket(int socket)
{
#ifdef _WIN32
	return closesocket(socket);
#elif __linux__
	return close(socket);
#endif
}

char* strtok_all(char* _String, const char* _Delimiter, char** _Context)
{
#ifdef _WIN32
	return strtok_s(_String, _Delimiter, _Context);
#elif __linux__
	return strtok_r(_String, _Delimiter, _Context);
#endif
}

std::string trim(std::string str)
{
	int n; 
	n = str.find_first_not_of(" \t"); 
	if ( n != std::string::npos ) 
	{
		str.replace(0, n,""); 
	}
	n = str.find_last_not_of(" \t"); 
	if ( n != std::string::npos )
	{
		str.replace(n+1, str.length()-n,""); 
	}
	return str;
}

int client_connect(int client_sock, char* ip, int port)
{
	char buf[4096];
	HTTPMessage message;
	
	int re = recv(client_sock, buf, 4096, 0);
	buf[re] = '\0';	
	InReqItem reqitem(ip, std::to_string(port), "", buf);
		
	std::string result = makeResult(buf, port, message, reqitem);
	send(client_sock, result.c_str(), result.length(), 0);
	closeOsSocket(client_sock);
    AhatLogger::IN_REQ(CODE, reqitem, result);

	return 0;
}

std::string makeResult(char* msg, int port, HTTPMessage message, InReqItem& reqitem)
{
	std::string result;
	std::string header;
	std::string body;

	char* saveptr1;
	std::string pro;
	std::string api;
	std::string value;

	char* tok;
	bool isHttp = false;
	tok = strtok_all(msg, " ", &saveptr1);

	if (!tok)
	{
		message.setHeaderCode("404");
		return message.getMessage();
	}
	pro = tok;
	
	if	//HTTP 프로토콜인지를 확인함 현재는 사용되지 않음
	(
		pro.compare("GET") == 0 ||
		pro.compare("POST") == 0 ||
		pro.compare("HEAD") == 0 ||
		pro.compare("PUT") == 0 ||
		pro.compare("DELETE") == 0 ||
		pro.compare("OPTIONS") == 0 ||
		pro.compare("TRACE") == 0 ||
		pro.compare("CONNECT") == 0
	)
	{
		isHttp = true;
	}
	
	tok = strtok_all(NULL, "? \n", &saveptr1);
	if( !tok )
	{
		message.setHeaderCode("404");
		return message.getMessage();
    }

	std::string path = apiPath + tok;	//API 주소
	reqitem.in_req_url = std::string(tok);

	if(pro.compare("GET") == 0)
	{
		tok = strtok_all(NULL, "? \n/", &saveptr1);
		if(strcmp(tok, "HTTP") != 0)
		{
			message.setBodyParam(std::string(tok));
		}
	}
	else if(pro.compare("POST") == 0)
	{
		tok = strtok_all(NULL, "? \n/", &saveptr1);
		if(strcmp(tok, "HTTP") != 0)
		{
			char* body = strstr(msg, "\r\n\r\n");
			if(body != NULL && strlen(body) > 2)
			{
				message.setBodyParam(std::string(body + 2));
			}
		}
	}
	
	if( !tok )
	{
		message.setHeaderCode("404");
		return message.getMessage();
    }
	value = tok;		//GET 형식의 리퀘스트 파라미터
						//지금은 body 파싱은 하지 않겠음
	
//	result = makeHeader(getFileData(path));	//파일에 헤더정보까지 직접 쓰도록
	path = path.replace(path.find("/"), 1, "\\");
	result = getFileData(path, port, message);
	return result;
}

std::string getFileData(std::string filepath, int port, HTTPMessage message)
{
	int fd;
	char buf[129];
	int num;
	std::string data = "";
	std::string body = "";
#ifdef _WIN32
	_sopen_s(&fd, filepath.c_str(), _O_RDONLY, _SH_DENYNO, 0);
#elif __linux__
	fd = open(filepath.c_str(), O_RDONLY);
#endif
	if(fd == -1)
	{
		AhatLogger::ERR(CODE, "%s  file not found!", filepath.c_str());
		message.setHeaderCode("404");
		return message.getMessage();
    }
	while((num = read(fd, buf, 128)) > 0) 
	{
		buf[num] = '\0';
		data += buf;
	}

	close(fd);

	std::istringstream ss(data);
	std::string line;

	if(!std::getline(ss, line, '\n'))
	{
		return message.getMessage();
	}

	if(line.compare("#script") != 0)
	{
		message.addBodyText(data);
		return message.getMessage();
	}

	bool in = false;
	while(std::getline(ss, line, '\n'))
	{
		if(line.find("//") != std::string::npos)
		{
			line = line.substr(0, line.find("//"));
		}
		line = trim(line);

		if(in == false && line.find("#if") != std::string::npos)
		{
			std::istringstream sss(line);
			std::string tmp = "";
			std::getline(sss, tmp, ' ');
			std::getline(sss, tmp, ' ');
			if(tmp.compare("port") == 0)
			{
				std::getline(sss, tmp, ' ');
				if(tmp.compare("all") == 0 || tmp.compare(std::to_string(port)) == 0)
				{
					in = true;
				}
			}
		}
		else if(in == false)
		{
			continue;
		}
		else if(in == true && line.find("#end") != std::string::npos)
		{
			in = false;
			break;
		}
		else if(line.find("#header") != std::string::npos && line.find("=") != std::string::npos)
		{
			std::istringstream sss(line);
			std::string name = "";
			std::string value = "";

			std::getline(sss, name, '=');
			std::getline(sss, value, '=');

			if(name.compare("#header-code") == 0)
			{
				message.setHeaderCode(value);
			}
			else if(name.compare("#header-content-type") == 0)
			{
				message.setHeaderContentType(value);
			}
		}
		else if(line.find("#body") != std::string::npos && line.find("=") != std::string::npos)
		{
			std::istringstream sss(line);
			std::string name = "";
			std::string value = "";

			std::getline(sss, name, '=');
			std::getline(sss, value, '=');

			if(name.compare("#body-type") == 0)
			{
				message.setBodyType(value);
			}
			else if(name.find("#body-param") != std::string::npos)
			{
				message.setBodyParam(value);
			}
			else if(name.compare("#body-file") == 0)
			{
				message.setBodyFile(value);
			}
		}
		else if(line.find("#") == std::string::npos)
		{
			message.addBodyText(line);
		}
		else
		{
			/* error */
		}
		
	}

	return message.getMessage();
}