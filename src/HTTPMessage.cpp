#include "HTTPMessage.h"
#include "ahatlogger.h"

HTTPMessage::HTTPMessage()
{
	header_code = "200";
	header_title = "";
	header_contentType = "";
	header_contentLength = 0;

    body_type = "raw";
    body_file = "";
    body_param = "";
    body_text = "";
}

void HTTPMessage::setHeaderCode(std::string value)
{
	header_code = value;
}

void HTTPMessage::setHeaderTitle(std::string value)
{
	header_title = value;
}

void HTTPMessage::setHeaderContentType(std::string value)
{
	header_contentType = value;
}

void HTTPMessage::setHeaderContentLength(int value)
{
	header_contentLength = value;
}

void HTTPMessage::setBodyType(std::string value)
{
	body_type = value;
}

void HTTPMessage::setBodyFile(std::string value)
{
	body_file = value;
}

void HTTPMessage::setBodyParam(std::string value)
{
	body_param = value;
}

std::string HTTPMessage::getBodyParamSpace()
{
	std::string tmp_text = body_param;
	size_t start_pos = 0; 
    while((start_pos = tmp_text.find("&", start_pos)) != std::string::npos)
    {
        tmp_text.replace(start_pos, 1, " ");
        start_pos += 1;
    }
    return tmp_text;
}

void HTTPMessage::addBodyText(std::string value)
{
	if(!body_text.empty())
	{
		body_text += "\n";
	}
	body_text += value;
}

std::string HTTPMessage::getMessage()
{
	std::string message = "";
	if(body_type.compare("python") == 0)
	{
		if(body_file.empty())
		{
			header_code = "404";
		}
		else
		{
			getMessagePython();
		}
	}
#ifdef __linux__
	else if (body_type.compare("shell") == 0)
	{
		if (body_file.empty() && body_text.empty())
		{
			header_code = "404";
		}
		else if (body_file.empty() && !body_text.empty())
		{
			getMessageShellText();
		}
		else
		{
			getMessageShell();
		}
	}
#elif _WIN32
	else if (body_type.compare("batch") == 0)
	{
		if (body_file.empty() && body_text.empty())
		{
			header_code = "404";
		}
		else if (body_file.empty() && !body_text.empty())
		{
			getMessageBatchText();
		}
		else
		{
			getMessageBatch();
		}
	}
#endif
	else
	{
		/* raw */
	}
	
	message += getHeader(body_text.length());
	message += body_text;
	
	return message;
}

std::string HTTPMessage::getHeader(int bodyLength)
{
	std::string header = "HTTP/1.1 ";
	header += header_code;

	if(!header_title.empty())
	{
		header += " ";
		header += header_title;
	}
	else if(!header_code.compare("200"))
	{
		header += " OK";
	}
	else if(!header_code.compare("400"))
	{
		header += " Bad Request";
	}
	else if(!header_code.compare("404"))
	{
		header += " File Not Found";
	}

	header += "\r\nAccept: *\r\nConnection: close\r\nContent-Type: ";
	if(header_contentType.empty())
	{
		header += "text/html;charset=UTF-8";
	}
	else
	{
		header += header_contentType;
	}
	header += "\r\nConnection: close";
	header += "\r\nCache-Control: no-cache";
	
	header += "\r\nContent-Length: ";
	header += std::to_string(bodyLength);
	header += "\r\n\r\n";

	return header;
}

#ifdef __linux__
bool HTTPMessage::getMessageShell()
{
	int fd = 0;
	FILE* fp;
	char buf[128];
	int num = 0;
	std::string file_data = "";
	std::string data = "";

	file_data += "./";
	file_data += body_file;
	file_data += " ";
	file_data += getBodyParamSpace();

	fp = popen(file_data.c_str(), "r");
	if (NULL == fp)
	{
		AhatLogger::ERR(CODE, "%s shell file error", file_data);
	}

	while (fgets(buf, 127, fp))
		data += buf;

	pclose(fp);

	body_text = data;

	return true;
}

bool HTTPMessage::getMessageShellText()
{
	FILE* fp;
	char buf[128];
	int num;
	std::string data = "";

	fp = popen(body_text.c_str(), "r");
	if (NULL == fp)
	{
		AhatLogger::ERR(CODE, "%s shell text error", body_file);
	}

	while (fgets(buf, 127, fp))
		data += buf;

	pclose(fp);

	body_text = data;

	return true;
}
#elif _WIN32
bool HTTPMessage::getMessageBatch()
{
	int fd = 0;
	FILE* fp;
	char buf[1030];
	int num = 0;
	std::string file_data = "";
	std::string data = "";

	wchar_t tmp[260];
	int len = GetModuleFileName(NULL, tmp, MAX_PATH);
	std::wstring ws(tmp);
	std::string path(ws.begin(), ws.end());
	path = path.substr(0, path.find_last_of("."));

	file_data = path + "API";

	file_data += "\\";
	file_data += body_file;
	file_data += " ";
	file_data += getBodyParamSpace();

	fp = _popen(file_data.c_str(), "r");
	if (NULL == fp)
	{
		AhatLogger::ERR(CODE, "%s batch file error", file_data);
	}

	int i = 0;
	while (fgets(buf, 1024, fp))
	{
		if (i++ < 2) continue;
		data += buf;
	}

	_pclose(fp);

	body_text = data;

	return true;
}

bool HTTPMessage::getMessageBatchText()
{
	FILE* fp;
	char buf[1030];
	int num;
	std::string data = "";

	fp = _popen(body_text.c_str(), "r");
	if (NULL == fp)
	{
		AhatLogger::ERR(CODE, "%s batch text error", body_file);
	}

	int i = 0;
	while (fgets(buf, 1024, fp))
	{
		if (i++ < 2) continue;
		data += buf;
	}

	_pclose(fp);

	body_text = data;

	return true;
}
#endif

bool HTTPMessage::getMessagePython()
{
	FILE *fp;
	char buf[1030];
	std::string file_data = "";
	std::string data = "";

	file_data = "python ";
	file_data += body_file;
	file_data += " ";
	file_data += getBodyParamSpace();

#ifdef __linux__
	fp = popen(file_data.c_str(), "r");
#elif _WIN32
	fp = _popen(file_data.c_str(), "r");
#endif
	if ( NULL == fp)
	{
		AhatLogger::ERR(CODE, "%s script error", body_file);
	}

	while(fgets(buf, 1024, fp))
		data += buf;

#ifdef __linux__
	pclose(fp);
#elif _WIN32
	_pclose(fp);
#endif

	body_text = data;

	return true;
}