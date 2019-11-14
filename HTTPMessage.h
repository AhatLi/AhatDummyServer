#ifndef __AHAT_HTTPMESSAGE_H__
#define __AHAT_HTTPMESSAGE_H__

#include <iostream>

class HTTPMessage
{
private:
    std::string header_code;
    std::string header_title;
    std::string header_contentType;
    int header_contentLength;
    
    std::string body_type;
    std::string body_file;
    std::string body_function;
    int body_function_param_num;
    std::string body_function_param;

    std::string body_text;
public:
    HTTPMessage();
	void setHeaderCode(std::string value);
	void setHeaderTitle(std::string value);
	void setHeaderContentType(std::string value);
	void setHeaderContentLength(int value);

	void setBodyType(std::string value);
	void setBodyFile(std::string value);
	void setBodyFunction(std::string value);
	void setBodyFunctionParamNum(int value);
	void setBodyFunctionParam(std::string value);
	void addBodyText(std::string value);

	std::string getMessage();
	std::string getHeader(int bodyLength);
};

#endif