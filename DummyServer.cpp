#include "DummyServer.h"
#include "HTTPMessage.h"

int DummyServer(int port) 
{
	std::cout<<port <<" thread is start!\n";
	int retval = 0;
	int client_sock = 0;  
	struct sockaddr_in clientaddr;

	socklen_t addrlen;
	int listen_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_sock == -1) 
	{
		std::cout << port << " port socket error\n";
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
		std::cout << port << " port socket error\n";
		return 0;
	}

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == -1) 
	{
		std::cout << port << " port listen error\n";
		return 0;
	}

	addrlen = sizeof(clientaddr);
	while (1)
	{
		client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
		if (client_sock == -1)  
		{
			std::cout << port << " port connect error\n";
			return 0;
		}
		
		std::thread t(client_connect, client_sock, port);
		t.detach();
	}

	close(listen_sock);
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

int client_connect(int client_sock, int port)
{
	char buf[4096];
	
	int re = recv(client_sock, buf, 4096, 0);
	buf[re] = '\0';	
		
	std::cout << port << " port data request\n" << buf << "\n\n";
	std::string result = makeResult(buf, port);
	std::cout << "response \n" << result << "\n";
	send(client_sock, result.c_str(), result.length(), 0);
	close(client_sock);
	
	return 0;
}

std::string makeResult(char* msg, int port)
{	
	std::string result;
	std::string header;
	std::string body;
	
	char *saveptr1;
	std::string pro;
	std::string api;
	std::string value;
	
	char* tok;
	bool isHttp = false;
	
	std::string path;
	char buf[256];
	int len = readlink("/proc/self/exe", buf, 256);
	buf[len] = '\0';
	
	path = buf;
	path += "API";	
	
	tok = strtok_r(msg, " ", &saveptr1);
	
	if( !tok )
	{
        return "";
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
	
	tok = strtok_r(NULL, "? \n", &saveptr1);
	if( !tok )
	{
        return "";
    }
	api = tok;			//API 주소
	
	path += api;
	
	tok = strtok_r(NULL, "? \n", &saveptr1);
	if( !tok )
	{
        return "";
    }
	value = tok;		//GET 형식의 리퀘스트 파라미터
						//지금은 body 파싱은 하지 않겠음
	
//	result = makeHeader(getFileData(path));	//파일에 헤더정보까지 직접 쓰도록
	result = getFileData(path, port);
	return result;
}

std::string getFileData(std::string filepath, int port)
{
	int fd;
	char buf[129];
	int num;
	std::string data = "";
	std::string body = "";
	fd = open(filepath.c_str(), O_RDONLY);
	if(fd == -1)
	{
		std::cout<<filepath<<" file not found!\n";
		return data;
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
		return std::string();
	}

	if(line.compare("#script") != 0)
	{
		return data;
	}

	bool in = false;
	HTTPMessage message;
	while(std::getline(ss, line, '\n'))
	{
		if(line.find("//") != std::string::npos)
		{
			line = line.substr(0, line.find("//"));
		}
		line = trim(line);

		if(line.find("#") != std::string::npos)
		{
			line = line.substr(0, line.find("#"));
		}

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
		else if(line.find("#end") != std::string::npos)
		{
			in = false;
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

			}
			else if(name.compare("#header-content-type") == 0)
			{

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
			else if(name.compare("#body-file") == 0)
			{
				message.setBodyFile(value);
			}
			else if(name.compare("#body-function") == 0)
			{
				message.setBodyFunction(value);
			}
			else if(name.compare("#body-function-param_num") == 0)
			{
		//		message.setBodyFunctionParamNum(value);
			}
			else if(name.find("#body-function-param") != std::string::npos)
			{
				message.setBodyFunctionParam(value);
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
		
		data = message.getMessage();
	}

	return data;
}

/*
std::string makeHeader(std::string body)
{
	std::string result;
	std::string header;
	
	header += "HTTP/1.1 200 OK\r\n";
	header += "Accept: *\r\n";
	header += "Connection: close\r\n";
	header += "Content-Type: application/json\r\n";
	char buf[66289];
	sprintf(buf, "Content-Length:%d\r\n\r\n%s", body.length(), body.c_str());
	result = header;
	result += buf;
	
	return result;
}
*/
