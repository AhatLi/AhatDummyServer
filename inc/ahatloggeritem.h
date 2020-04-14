#ifndef AHATLOGGERITEM_H_
#define AHATLOGGERITEM_H_

#include <iostream>
#include <sstream>
#include <thread>
#include <iomanip>

#ifdef __linux__
#define localtime_s(X, Y) localtime_r(Y, X)
#else
#endif

std::string getCurTime();

class AhatLogItem
{
public:
	std::string src_file;
	int log_type;

	std::string message() {};
};	

class AhatLogItemInfo : public AhatLogItem
{
public:
	std::string body;

	AhatLogItemInfo(std::string src_file, std::string body);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << body;
	
		return buf.str();
	};
};
class AhatLogItemError : public AhatLogItem
{
public:
	std::string body;

	AhatLogItemError(std::string src_file, std::string body);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << body;
	
		return buf.str();
	};
};

class AhatLogItemRequest : public AhatLogItem
{
public:
	std::string req_ip;
	std::string req_uri;
	
	std::string req_body;

	AhatLogItemRequest(std::string src_file, std::string req_ip, std::string req_uri, std::string req_body);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << req_ip << "," << req_uri << "," << req_body;
	
		return buf.str();
	};
};

class AhatLogItemResponse : public AhatLogItem
{
public:
	std::string res_body;
	std::string res_code;

	AhatLogItemResponse(std::string src_file, std::string res_body, std::string res_code);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << res_code << "," << res_body;
	
		return buf.str();
	};
};

class InDBtem
{
public:
	std::string db_req_time;
	std::string db_req_func;
	std::string db_req_body;
	
	InDBtem(std::string db_req_func, std::string db_req_body);
	InDBtem(std::string db_req_func);
	InDBtem();
};

class AhatLogItemDB : public AhatLogItem
{
public:
	InDBtem db_req_item;
	std::string db_res_time;
	std::string db_res_body;

	AhatLogItemDB(std::string src_file, InDBtem db_req_item, std::string db_res_body);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << db_req_item.db_req_time << "," << db_res_time 
			<< "," << db_req_item.db_req_func << "," << db_req_item.db_req_body << "," << db_res_body;
	
		return buf.str();
	};
};

class AhatLogItemCustom : public AhatLogItem
{
public:
	std::string body;

	AhatLogItemCustom(std::string src_file, std::string body);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << body;
	
		return buf.str();
	};
};

class InReqItem
{
public:
	std::string in_req_ip;
	std::string in_req_port;
	std::string in_req_url;
	std::string in_req_time;
	std::string in_req_body;
	
	InReqItem(std::string in_req_ip, std::string in_req_port, std::string in_req_url, std::string in_req_body);
	InReqItem(std::string in_req_ip, std::string in_req_port, std::string in_req_url);
	InReqItem();
};

class AhatLogItemInReq : public AhatLogItem
{
public:
	InReqItem in_req_item;
	std::string in_res_time;
	std::string in_res_body;

	AhatLogItemInReq(std::string src_file, InReqItem in_req_item, std::string in_res_body);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << in_req_item.in_req_ip << "," << in_req_item.in_req_port 
			<< "," << in_req_item.in_req_url << "," << in_req_item.in_req_time << "," << in_res_time << "," << in_req_item.in_req_body << "," << in_res_body;
	
		return buf.str();
	};
};

class AhatLogItemDebug : public AhatLogItem
{
public:
	std::string body;

	AhatLogItemDebug(std::string src_file, std::string body);
	std::string message()
	{
		std::stringstream buf;
		buf << "," << src_file << "," << std::this_thread::get_id() << "," << body;
	
		return buf.str();
	};
};

#endif