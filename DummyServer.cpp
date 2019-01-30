#include "DummyServer.h"

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

int client_connect(int client_sock, int port)
{
	char buf[4096];
	
	int re = recv(client_sock, buf, 4096, 0);
	buf[re] = '\0';	
		
	std::cout << port << " port data request\n" << buf << "\n\n";
	std::string result = makeResult(buf);
	std::cout << "response \n" << result << "\n";
	send(client_sock, result.c_str(), result.length(), 0);
	close(client_sock);
	
	return 0;
}

std::string makeResult(char* msg)
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
	result = getFileData(path);
	return result;
}

std::string getFileData(std::string filepath)
{
	int fd;
	char buf[129];
	int num;
	std::string data = "";
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
