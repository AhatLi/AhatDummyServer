#include "HTTPMessage.h"

HTTPMessage::HTTPMessage()
{
	header_code = "200";
	header_title = "";
	header_contentType = "";
	header_contentLength = 0;

    body_type = "raw";
    body_file = "";
    body_function = "";
    body_function_param_num = 0;
    body_function_param = "";
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

void HTTPMessage::setBodyFunction(std::string value)
{
	body_function = value;
}

void HTTPMessage::setBodyFunctionParamNum(int value)
{
	body_function_param_num = value;
}

void HTTPMessage::setBodyFunctionParam(std::string value)
{
	body_function_param = value;
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
	if(body_type.compare("python3.8") == 0)
	{

	}
	else if(body_type.compare("python3.7") == 0)
	{

	}
	else
	{
		/* raw */
	}
	







	std::string message = "HTTP/1.1 ";
	message += header_code;

	if(!header_title.empty())
	{
		message += " ";
		message += header_title;
	}
	else if(!header_code.compare("200"))
	{
		message += " OK";
	}
	else if(!header_code.compare("400"))
	{
		message += " Bad Request";
	}

	message += "\r\nAccept: *\r\nConnection: close\r\nContent-Type: ";
	if(header_contentType.compare("") == 0)
	{
		message += "text/html;charset=UTF-8";
	}
	else
	{
		message += header_contentType;
	}
	
	message += "\r\nContent-Length: ";
	message += std::to_string(body_text.length());
	message += "\r\n\r\n";

	message += body_text;

	return message;
}