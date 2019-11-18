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
	body_file = value;
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
	else if(body_type.compare("shell") == 0)
	{
		if(body_file.empty() && body_text.empty())
		{
			header_code = "404";
		}
		else if(body_file.empty() && !body_text.empty())
		{
			getMessageShellText();
		}
		else
		{
			getMessageShell();
		}
	}
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
	
	header += "\r\nContent-Length: ";
	header += std::to_string(bodyLength);
	header += "\r\n\r\n";

	return header;
}

bool HTTPMessage::getMessageShell()
{
	int fd = 0;
	FILE *fp;
	char buf[128];
	int num = 0;
	std::string shell_data = "";
	std::string data = "";
	fd = open(body_file.c_str(), O_RDONLY);
	if(fd == -1)
	{
		AhatLogger::ERROR(CODE, "%s script file not found!", body_file);
    }
	while((num = read(fd, buf, 127)) > 0) 
	{
		buf[num] = '\0';
		shell_data += buf;
	}

	close(fd);

	fp = popen(shell_data.c_str(), "r");
	if ( NULL == fp)
	{
		AhatLogger::ERROR(CODE, "%s script error", body_file);
	}

	while(fgets(buf, 127, fp))
		data += buf;

	pclose( fp);

	body_text = data;
}

bool HTTPMessage::getMessageShellText()
{
	FILE *fp;
	char buf[128];
	int num;
	std::string data = "";

	fp = popen(body_text.c_str(), "r");
	if ( NULL == fp)
	{
		AhatLogger::ERROR(CODE, "%s script error", body_file);
	}

	while(fgets(buf, 127, fp))
		data += buf;

	pclose( fp);

	body_text = data;
}

bool HTTPMessage::getMessagePython()
{
	FILE *fp;
	char buf[128];
	std::string file_data = "";
	std::string data = "";

	file_data = "python ";
	file_data += body_file;
	file_data += " ";
	file_data += body_param;

	fp = popen(file_data.c_str(), "r");
	if ( NULL == fp)
	{
		AhatLogger::ERROR(CODE, "%s script error", body_file);
	}

	while(fgets(buf, 127, fp))
		data += buf;

	pclose(fp);

	body_text = data;

	return true;
}