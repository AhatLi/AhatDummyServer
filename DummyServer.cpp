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


void* client_connect(void *arg)
{
	int client_sock = *((int*)&arg);
	
	char buf[1024];
	int re = 1;
	int c = 1;

	re = recv(client_sock, buf, 1024, 0);
	buf[re] = '\0';	

	string result = makeResult(buf);
	if(result.length() > 5)
	{
		send(client_sock, result.c_str(), result.length(), 0);
	}
	close(client_sock);
	
	return 0;
}


string makeResult(char* msg)
{
	string result = "";
	string header;
	string body;
	
	char *saveptr1;
	string pro;
	string api;
	string value;
	
	char* tok;
	tok = strtok_r(msg, " ", &saveptr1);
	
	if( !tok )
	{
        return "";
    }
	pro = tok;
	
	tok = strtok_r(NULL, "? \n", &saveptr1);
	if( !tok )
	{
        return "";
    }
	api = tok;
	
	tok = strtok_r(NULL, "? \n", &saveptr1);
	if( !tok )
	{
        return "";
    }
	value = tok;
	
	printf("%s %s %s\n", pro.c_str(), api.c_str(), value.c_str());
	
	if(api == "/info/hitCount/current/size")
	{		
		body = makeBody_HitcountSize(value);
	}
	else if(api == "/info/hitCount/current/index")
	{
		char buf2[128];
		sprintf(buf2, "%s", value.c_str());
		tok = strtok_r(buf2, "& ", &saveptr1);
		if( !tok )
		{
			return "";
		}
		tok = strtok_r(NULL, "& ", &saveptr1);
		tok = strtok_r(NULL, "& \n", &saveptr1);
		string group = tok;
		
		body = makeBody_HitcountList(group);
	}
	else if(api == "/info/file")
	{
		body = makeBody_FileInfo(value);
		sleep(55);
		printf("1111111111111111111\n");
	}
	else if(api == "/command/dist.json")
	{
		body = makeBody_Post();
	}
	else if(api == "/command/delete.json")
	{
		body = makeBody_Post();
	}
	else if(api == "/mediafile/info/relation")
	{
		body = makeBody_Post();
	}
	else
	{
		body = "{\n";
		body += "   \"errorString\" : \"Service Unavailable\",\n";
		body += "   \"resultCode\" : 203\n";
		body += "}";	
	}
	return makeHeader(body);
}

string makeBody_HitcountSize(string group)
{
	string body;
	string hitCountSize;
	
	if(group == "group=CDN1")
		hitCountSize += "   \"hitCountSize\" : 1,\n";
	else if(group == "group=CDN2")
		hitCountSize += "   \"hitCountSize\" : 2,\n";
	else if(group == "group=FILE")
		hitCountSize += "   \"hitCountSize\" : 1,\n";
	else if(group == "group=OLDFILE1")
		hitCountSize += "   \"hitCountSize\" : 2,\n";
	else if(group == "group=OLDFILE2")
		hitCountSize += "   \"hitCountSize\" : 151,\n";
	else
		hitCountSize += "   \"hitCountSize\" : 6,\n";
		
	body = "{\n";
	body += "   \"errorString\" : \"\",\n";
	body += hitCountSize;
	body += "   \"resultCode\" : 100\n";
	body += "}";
	
	return body;
}

string makeBody_HitcountList(string group)
{
	string body;
	string body2;
	int fname = 0;
	
	
	if(group == "group=CDN1")
	{
		body += "         \"fileName\" : \"1.mpg\",\n";
		body += "         \"filePath\" : \"/data\",\n";
		body += "         \"fileSize\" : 100000000,\n";
		body += "         \"groupList\": [\"CDN1\"],\n";
		body += "         \"hitCount\" : 0,\n";
		body += "         \"registerTime\" : 1445566361\n";
	}
	else if(group == "group=CDN2")
	{
		body += "         \"fileName\" : \"2.mpg\",\n";
		body += "         \"filePath\" : \"/data\",\n";
		body += "         \"fileSize\" : 100000000,\n";
		body += "         \"groupList\": [\"CDN2\"],\n";
		body += "         \"hitCount\" : 0,\n";
		body += "         \"registerTime\" : 1445566361\n";
		body += "      },\n";
		body += "      {\n";
		body += "         \"fileName\" : \"3.mpg\",\n";
		body += "         \"filePath\" : \"/\",\n";
		body += "         \"fileSize\" : 100000000,\n";
		body += "         \"groupList\": [\"CDN2\"],\n";
		body += "         \"hitCount\" : 0,\n";
		body += "         \"registerTime\" : 1445566360\n";
	}
	else if(group == "group=FILE")
	{
		body += "         \"fileName\" : \"4.mpg\",\n";
		body += "         \"filePath\" : \"/\",\n";
		body += "         \"fileSize\" : 100000000,\n";
		body += "         \"groupList\": [\"FILE\"],\n";
		body += "         \"hitCount\" : 0,\n";
		body += "         \"registerTime\" : 1445566360\n";
	}
	else if(group == "group=OLDFILE1")
	{
		body += "         \"fileName\" : \"5.mpg\",\n";
		body += "         \"filePath\" : \"/data\",\n";
		body += "         \"fileSize\" : 100000000,\n";
		body += "         \"groupList\": [\"OLDFILE1\"],\n";
		body += "         \"hitCount\" : 0,\n";
		body += "         \"registerTime\" : 1445566361\n";
		for(int i = 0; i<150; i++)
		{
			body += "      },\n";
			body += "      {\n";
			body += "         \"fileName\" : \"6.mpg\",\n";
			body += "         \"filePath\" : \"/\",\n";
			body += "         \"fileSize\" : 100000000,\n";
			body += "         \"groupList\": [\"OLDFILE2\"],\n";
			body += "         \"hitCount\" : 0,\n";
			body += "         \"registerTime\" : 1445566360\n";
		}
	}
	else if(group == "group=OLDFILE2")
	{
		body += "         \"fileName\" : \"7.mpg\",\n";
		body += "         \"filePath\" : \"/data\",\n";
		body += "         \"fileSize\" : 100000000,\n";
		body += "         \"groupList\": [\"OLDFILE2\"],\n";
		body += "         \"hitCount\" : 0,\n";
		body += "         \"registerTime\" : 1445566361\n";
		for(int i = 0; i<150; i++)
		{
			body += "      },\n";
			body += "      {\n";
			body += "         \"fileName\" : \"8.mpg\",\n";
			body += "         \"filePath\" : \"/\",\n";
			body += "         \"fileSize\" : 100000000,\n";
			body += "         \"groupList\": [\"OLDFILE2\"],\n";
			body += "         \"hitCount\" : 0,\n";
			body += "         \"registerTime\" : 1445566360\n";
		}
	}
	if(body.length() > 5)
	{
		body2  = "{\n";
		body2 += "   \"hitCountList\" : [\n";
		body2 += "      {\n";
		body2 += body;
		body2 += "      }\n";
		body2 += "   ],\n";
		body2 += "   \"resultCode\" : 100\n";
		body2 += "}";
		
	}
	
	return body2;
}

string makeBody_FileInfo(string filename)
{
	string body;
	
	body  = "{\n";
    body += "\"filePath\" : \"/lim/1001/\",\n";
    body += "\"fileSize\" : 100000000,\n";
    body += "\"groupList\" : [ \"CDN2\" ],\n";
    body += "\"hitCount\" : 0,\n";
    body += "\"registerTime\" : 1486950243,\n";
    body += "\"resultCode\" : 100\n";
	body += "}";
	
	return body;
}

string makeBody_Post()
{
	string body;
	body = "{\n";
	body += "   \"resultCode\" : 100\n";
	body += "}";
	
	return body;
}

*/
