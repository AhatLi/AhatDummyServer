#include "DummyServer.h"
#include "HTTPMessage.h"

std::string apiPath = "";

int DummyServer::start() 
{
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
		if (!q.empty())
		{
			auto data = Dequeue();
			client_connect(data.first, data.second);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	return 0;
}

int DummyServer::client_connect(int client_sock, InReqItem reqitem)
{
	char buf[4096];
	HTTPMessage message;
	
	int ret = recv(client_sock, buf, 4096, 0);
	if (ret <= 0)
	{
		closeOsSocket(client_sock);
		return 0;
	}
	buf[ret] = '\0';	

	std::stringstream ss(reqitem.in_req_port);
	int port;
	ss >> port;
	
	reqitem.in_req_body = std::string(buf);
	std::string result = makeResult(buf, port, message, reqitem);
	send(client_sock, result.c_str(), result.length(), 0);
	closeOsSocket(client_sock);
    AhatLogger::IN_REQ_DEBUG(CODE, reqitem, result);

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

	return message.getMessage();
}


void DummyServer::Enqueue(int client_sock, InReqItem reqitem)
{
	std::pair<int, InReqItem> p;
	p.first = client_sock;
	p.second = reqitem;

	q.push(p);
}

std::pair<int, InReqItem> DummyServer::Dequeue()
{
	auto socket = q.back();
	q.pop();

	return socket;
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