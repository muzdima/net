#pragma once
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>

class NetApi
{
protected:
	WSADATA wsaData;
	static const unsigned short PORT=80;
	SOCKET mysocket;
	bool is_connect;
	bool is_auto_connect;
	std::string SERVERADDR;
	void DisconnectSys();
	bool ConnectSys();
public:
	bool getAutoConnect()const{return is_auto_connect;}
	void setAutoConnect(bool val){is_auto_connect=val;}
	bool ReConnect(const std::string& new_SERVERADDR);
	void Disconnect();
	NetApi();
	~NetApi();
	int Send(const std::string& text);
	std::string Receive(bool decodeUTF, bool printRes);
	char* ReceiveC(bool printRes, unsigned int &n);
	std::string SendAndReceive(std::string message, bool continue_to_succeed, bool decodeUTF, bool printRes);
	char* SendAndReceiveC(std::string message, bool continue_to_succeed,bool printRes, unsigned int &n);
};


class HttpApiExtended:public NetApi
{
public:
	static bool DownloadFile(std::string url, std::string file_name,bool continue_to_succeed);
	std::string GetPage(std::string host, std::string page, bool decodeUTF, bool printRes, std::string cookie);
};


class WebCameraKNU:public NetApi
{
protected:
	char* old_image_map;
	unsigned int old_image_size;
public:
	bool isCompareImageMap(const char* image_map,unsigned int image_size);
	WebCameraKNU();
	bool SaveWebCameraKNU(std::string file_name);
	~WebCameraKNU();
};


class VkApi:public NetApi
{
public:
	typedef std::vector<std::pair<std::string,std::string>> Tparam;
protected:
	std::string APP_ID()const{return "2988572";}
	std::string MY_ID()const{return "34559561";}
	std::string SECRET()const{return "d2fa335b7c41807cdb";}
	std::string TOKEN()const{return "91a6859405ed40cb6d78dd3a7f4781e09356435a600028c5f4a68cf19a0274fde4d51265f0e64098f672d";}
public:
	std::string Method(const std::string& name, const Tparam& param, bool continue_to_succeed);
};


class CommonWebApi:public HttpApiExtended,public VkApi,public WebCameraKNU{};