#include "HTTPMessage.h"
#include <errno.h>
#include "DummyServer.h"

#ifdef _WIN32
#include <winsock2.h> 
#elif __linux__
#include <sys/epoll.h>
#define EPOLL_SIZE 20
#endif

std::string apiPath = "";

rdata::rdata()
{
	item = new InReqItem();
}

rdata::~rdata()
{
	delete item;
}

int DummyServer::start(int port_size, int* port) 
{
    int* listen_sock = new int[port_size];

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
    int client_sock;

    int eventn;
    int epollfd;
    struct epoll_event ev, *events;

    events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    if ((epollfd = epoll_create(EPOLL_SIZE)) == -1)
    {
        std::cout << "epoll_create Error!\n";
        AhatLogger::ERR(CODE, "epoll_create Error!");
    }
#endif
	
    for (int i = 0; i < port_size; i++)
    {

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
            return 0;
        }

		int opt = 1;
		setsockopt(listen_sock[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); 
		
#endif
        memset((void*)&serveraddr, 0x00, sizeof(serveraddr));

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port[i]);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(listen_sock[i], (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1)
        {
            AhatLogger::ERR(CODE, "%d port bind error", port[i]);
            return 0;
        }

        if(listen(listen_sock[i], SOMAXCONN) == -1)
        {
            AhatLogger::ERR(CODE, "%d port listen error", port[i]);
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

    while (1)
    {
#ifdef _WIN32
        old_fds = new_fds;
        fd_num = select(0, &old_fds, NULL, NULL, NULL);
#elif __linux__    
        eventn = epoll_wait(epollfd, events, EPOLL_SIZE, -1);
#endif

		bool process = false;
		for (int i = 0; i < eventn; i++)
		{
			for (int j = 0; j < port_size; j++)
			{
				if (events[i].data.fd == listen_sock[j])    // 듣기 소켓에서 이벤트가 발생함
				{
					addrlen = sizeof(clientaddr);
					client_sock = accept(listen_sock[j], (struct sockaddr*) & clientaddr, &addrlen);
					if (client_sock < 0)
					{
						continue;
					}

					rdata* request_data = new rdata();
					request_data->item->in_req_ip = inet_ntoa(clientaddr.sin_addr);
					request_data->item->in_req_port = std::to_string(port[j]);
					request_data->fd = client_sock;

					ev.data.ptr = request_data;
        			ev.events = EPOLLIN;
					epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sock, &ev);

					process = true;
					break;
				}
			}
			if(!process)
			{		
				auto request_data = (rdata *)events[i].data.ptr;

				if(client_connect(request_data) < 0) 
				{
					epoll_ctl(epollfd, EPOLL_CTL_DEL, client_sock, NULL);
				}
			}
			else 
			{
				continue;;
			}
		}
	}

	return 0;
}

int DummyServer::client_connect(rdata* request_data)
{
	char buf[4096];
	HTTPMessage message;
	
	int ret = 0;	
	int err;
	ret = recv(request_data->fd, buf, 9000, 0);
	err = errno; // save off errno, because because the printf statement might reset it
	if (ret <= 0)
	{
		if ((err == EAGAIN) || (err == EWOULDBLOCK))
		{
			AhatLogger::ERR(CODE, "non-blocking operation returned EAGAIN or EWOULDBLOCK");
		}
		else
		{
			AhatLogger::ERR(CODE, "recv returned unrecoverable error(errno=%d)", err);
		}

		if(!request_data)
		{
			delete request_data;
		}
		return -1;
	}
	buf[ret] = '\0';	

	std::stringstream ss(request_data->item->in_req_port);
	int port;
	ss >> port;
	
	request_data->item->in_req_body = std::string(buf);
	std::string result = makeResult(buf, port, message, *request_data->item);
	send(request_data->fd, result.c_str(), result.length(), 0);
	socketClose(request_data->fd);
    AhatLogger::IN_REQ_DEBUG(CODE, *request_data->item, result);

	delete request_data;

	return 0;
}

std::string DummyServer::makeResult(char* msg, int port, HTTPMessage message, InReqItem& reqitem)
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

	std::string path = "";	//API 주소
	path += apiPath;
	path += tok;
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
#ifdef _WIN32
	path = path.replace(path.find("/"), 1, "\\");
#endif
	result = getFileData(path, port, message);
	return result;
}

std::string DummyServer::getFileData(std::string filepath, int port, HTTPMessage message)
{
	int fd;
	char buf[129];
	int num;
	std::string data = "";
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

	if(line.find("\r") != std::string::npos)
		line = line.replace(line.find("\r"), 1, "");

	if(line.compare("#script") != 0)
	{
		message.addBodyText(data);
		message.setFilePath(apiPath);
		return message.getMessage();
	}

	bool in = false;
	while(std::getline(ss, line, '\n'))
	{
		if (line.find("\r") != std::string::npos)
			line = line.replace(line.find("\r"), 1, "");
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

	message.setFilePath(apiPath);
	return message.getMessage();
}

int socketClose(int socket)
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
	if (n != std::string::npos)
	{
		str.replace(0, n, "");
	}
	n = str.find_last_not_of(" \t");
	if (n != std::string::npos)
	{
		str.replace(n + 1, str.length() - n, "");
	}
	return str;
}