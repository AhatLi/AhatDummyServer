#ifndef __AHAT_HTTPMESSAGE_H__
#define __AHAT_HTTPMESSAGE_H__


#include <iostream>
#include <fcntl.h>
#ifdef _WIN32
#elif __linux__
#include <unistd.h>
#endif

class HTTPMessage
{
private:
    std::string header_code;
    std::string header_title;
    std::string header_contentType;
    int header_contentLength;
    
    std::string body_type;
    std::string body_file;
    std::string body_param;

    std::string body_text;
	std::string file_path;

#ifdef __linux__
	bool getMessageShell();
	bool getMessageShellText();
#elif _WIN32
	bool getMessageBatch();
	bool getMessageBatchText();
#endif

	bool getMessagePython();
	std::string getBodyParamSpace();
public:
    HTTPMessage();
	void setHeaderCode(std::string value);
	void setHeaderTitle(std::string value);
	void setHeaderContentType(std::string value);
	void setHeaderContentLength(int value);

	void setBodyType(std::string value);
	void setBodyFile(std::string value);
	void setBodyParam(std::string value);
	void setFilePath(std::string value);
	void addBodyText(std::string value);

	std::string getMessage();
	std::string getHeader(int bodyLength);
};

#endif
