#pragma comment (lib, "WSock32.lib")
#pragma comment (lib, "WS2_32.lib")
#pragma comment (lib, "Urlmon.lib")
#include <stdio.h>
#include "sock.h"
#include "md5.h"
using namespace std;


bool NetApi::ReConnect(const std::string& new_SERVERADDR)
{
	bool res=1;
	SERVERADDR=new_SERVERADDR;
	do res=ConnectSys(); while (res && is_auto_connect);
	return res;
}


bool NetApi::ConnectSys()
{
	if (is_connect) Disconnect();
	printf("NetApi::(INFO) Установка соединения с %s\n",SERVERADDR.c_str());
	if (WSAStartup(0x0202,&wsaData))
	{
		printf("NetApi::(ERROR) Ошибка запуска Windows Sockets API %d\n",WSAGetLastError());
		return 1;
	}
	if ((mysocket=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("NetApi::(ERROR) Ошибка создания сокета %d\n",WSAGetLastError());
		WSACleanup();
		return 1;
	}
	sockaddr_in dest_addr;
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_port=htons(PORT);
	HOSTENT *hst;
	if (inet_addr(SERVERADDR.c_str())!=INADDR_NONE)
		dest_addr.sin_addr.s_addr=inet_addr(SERVERADDR.c_str());
	else
		if (hst=gethostbyname(SERVERADDR.c_str()))
			((unsigned long *)&dest_addr.sin_addr)[0]=((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("NetApi::(ERROR) Неверный адрес %s\n",SERVERADDR.c_str());
			closesocket(mysocket);
			WSACleanup();
			return 1;
		}
	if (connect(mysocket,(sockaddr *)&dest_addr,sizeof(dest_addr)))
	{
		printf("NetApi::(ERROR) Ошибка соединения %d\n",WSAGetLastError());
		return 1;
	}
	printf("NetApi::(INFO) Соединение с %s успешно установлено\n",SERVERADDR.c_str());
	is_connect=true;
	return 0;
}


void NetApi::DisconnectSys()
{
	if (is_connect)
	{
		printf("NetApi::(INFO) Разрыв cоединения\n");
		is_connect=false;
		closesocket(mysocket);
		WSACleanup();
	}
	if (is_auto_connect) ReConnect(SERVERADDR);
}


void NetApi::Disconnect()
{
	if (is_connect)
	{
		printf("NetApi::(INFO) Разрыв cоединения\n");
		is_connect=false;
		closesocket(mysocket);
		WSACleanup();
	}
}


NetApi::NetApi()
{
	SERVERADDR="";
	is_auto_connect=true;
	is_connect=false;
}


NetApi::~NetApi()
{
	is_auto_connect=false;
	Disconnect();
}


int NetApi::Send(const std::string& text)
{
	if (is_connect && text.size())
	{
		printf("NetApi::(INFO) Отправка сообщения \n\"\n%s\n\"\n",text.c_str());
		return send(mysocket,text.c_str(),text.size(),0);
	}
	return 0;
}


string NetApi::Receive(bool decodeUTF, bool printRes)
{
	if (printRes) printf("NetApi::(INFO) Ожидание приема сообщения\n");
	int nsize=1;
	char* buff=new char[1024];
	string buffer="";
	while (nsize && nsize!=SOCKET_ERROR)
	{
		nsize=recv(mysocket,&buff[0],sizeof(char*),0);
		buff[nsize]='\0';
		buffer+=buff;
	}
	delete [] buff;
	if (buffer.size()==0) return "";
	if(nsize!=SOCKET_ERROR)
	{
		//utf8 to cp1251(windows-1251)
		string res;
		if (decodeUTF)
		{
			int result_u, result_c;
			result_u = MultiByteToWideChar(CP_UTF8,0,buffer.c_str(),-1,0,0);
			if (!result_u)
			{
				if (printRes) printf("NetApi::(ERROR) Ошибка конвертации формата сообщения(utf8->cp1251)\n");
				return "";
			}
			wchar_t *ures = new wchar_t[result_u];
			if(!MultiByteToWideChar(CP_UTF8,0,buffer.c_str(),-1,ures,result_u))
			{
				delete[] ures;
				if (printRes) printf("NetApi::(ERROR) Ошибка конвертации формата сообщения(utf8->cp1251)\n");
				return "";
			}
			result_c = WideCharToMultiByte(1251,0,ures,-1,0,0,0, 0);
			if(!result_c)
			{
				delete[] ures;
				if (printRes) printf("NetApi::(ERROR) Ошибка конвертации формата сообщения(utf8->cp1251)\n");
				return "";
			}
			char *cres = new char[result_c];
			if(!WideCharToMultiByte(1251,0,ures,-1,cres,result_c,0, 0))
			{
				delete[] ures;
				delete[] cres;
				if (printRes) printf("NetApi::(ERROR) Ошибка конвертации формата сообщения(utf8->cp1251)\n");
				return "";
			}
			delete[] ures;
			res.append(cres);
			delete[] cres;
		}else res=buffer;
		if (printRes) printf("NetApi::(INFO) Прием сообщения \n\"\n%s\n\"\n",res.c_str());
		return res;
	}
	if (printRes) printf("NetApi::(ERROR) Ошибка приема сообщения %d\n",WSAGetLastError());
	DisconnectSys();
	return "";
}


const unsigned int size=1024;
char* NetApi::ReceiveC(bool printRes, unsigned int &n)
{
	if (printRes) printf("NetApi::(INFO) Ожидание приема сообщения\n");
	n=0;
	char* res=new char[n];
	int nsize=0;
	char* buff=new char[size];
	do
	{
		{
			char* p=res;
			res=new char[n+nsize];
			for (unsigned int i=0;i<n;i++)
				res[i]=p[i];
			for (unsigned int i=0;i<nsize;i++)
				res[i+n]=buff[i];
			delete [] p;
			n+=nsize;
		}
		nsize=recv(mysocket,&buff[0],size,0);
	}while (nsize && nsize!=SOCKET_ERROR);
	delete [] buff;

	if (n==0) return NULL;
	if(nsize!=SOCKET_ERROR)
	{
		
		if (printRes) printf("NetApi::(INFO) Прием сообщения \n");
		return res;
	}
	if (printRes) printf("NetApi::(ERROR) Ошибка приема сообщения %d\n",WSAGetLastError());
	DisconnectSys();
	return NULL;
}


string NetApi::SendAndReceive(string message, bool continue_to_succeed, bool decodeUTF, bool printRes)
{
	string res;
	while (!Send(message) && continue_to_succeed);
	while (!((res=Receive(decodeUTF,printRes)).size()) && continue_to_succeed);
	return res;
}


char* NetApi::SendAndReceiveC(string message, bool continue_to_succeed,bool printRes, unsigned int &n)
{
	char* res;
	while (!Send(message) && continue_to_succeed);
	while ((!(res=ReceiveC(printRes,n)) || !n) && continue_to_succeed);
	return res;
}


bool HttpApiExtended::DownloadFile(string url, string file_name,bool continue_to_succeed)
{
	printf("HttpApiExtended::(INFO) Загрузка \"%s\" в \"%s\"\n",url.c_str(),file_name.c_str());
	HRESULT res;
	while ((res=URLDownloadToFileA(NULL,url.c_str(),file_name.c_str(),0,NULL))!=S_OK && continue_to_succeed)
	{
		if (res==S_OK) printf("HttpApiExtended::(INFO) Загрузка успешно запущена\n");
		if (res==E_OUTOFMEMORY) printf("HttpApiExtended::(ERROR) Недостаточно памяти для завершения загрузки\n");
		if (res==INET_E_DOWNLOAD_FAILURE) printf("HttpApiExtended::(ERROR) Неверный адрес %s\n",url.c_str());
	}
	return res==S_OK;
}


string HttpApiExtended::GetPage(string host, string page, bool decodeUTF, bool printRes, string cookie)
{
	ReConnect(host);
	string ms="GET ";
	ms+=page;
	ms+=" HTTP/1.0\nHost: ";
	ms+=host;
	ms+="\nUser-agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\n";
	ms+=cookie;
	ms+="Accept: */*\n\n";
	string res=SendAndReceive(ms,true,decodeUTF,printRes);
	Disconnect();
	return res;
}


bool WebCameraKNU::isCompareImageMap(const char* image_map,unsigned int image_size)
{
	if (image_size!=old_image_size) return false;
	for (unsigned int i=0;i<image_size;i++)
	if (image_map[i]!=old_image_map[i]) return false;
	return true;
}


WebCameraKNU::WebCameraKNU()
{
	setAutoConnect(true);
	old_image_size=1;
	old_image_map=new char[old_image_size];
	*old_image_map='\0';
}


bool WebCameraKNU::SaveWebCameraKNU(std::string file_name)
{
	setAutoConnect(true);
	ReConnect("195.68.211.7");
	std::string msg="GET /unicyb/GetImage.cgi HTTP/1.0\nHost: 195.68.211.7\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.64 Safari/537.31\n\
Accept-Encoding: gzip,deflate,sdch\n\
Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\n\
Accept-Charset: windows-1251,utf-8;q=0.7,*;q=0.3\n\
Cookie: AlarmSoundOff=TRUE; record_osd=true\n\n";
	unsigned int n,i=0;
	char* ans=SendAndReceiveC(msg,true,false,n);
	Disconnect();
	while ((ans[i]!='\r' || ans[i+1]!='\n' || ans[i+2]!='\r' || ans[i+3]!='\n')&& i<n-4)i++;
	i+=4;
	if ((ans[i]=='G' && ans[i+1]=='e' && ans[i+2]=='t') || isCompareImageMap(ans+i,n-i))
	{
		delete [] ans;
		return false;
	}
	delete [] old_image_map;
	old_image_map=new char[n-i];
	for (unsigned int k=0;k<n-i;k++)
		old_image_map[k]=(ans+i)[k];
	old_image_size=n-i;
	FILE* f=fopen(file_name.c_str(),"wb");
	fwrite(ans+i,1,n-i,f);
	fclose(f);
	delete [] ans;
	return true;
}


WebCameraKNU::~WebCameraKNU()
{
	setAutoConnect(false);
	Disconnect();
	old_image_size=0;
	delete [] old_image_map;
}


string VkApi::Method(const std::string& name, const Tparam& param, bool continue_to_succeed )
{
	ReConnect("api.vk.com");
	string ms="/method/"+name+"?";
	for (unsigned int i=0;i<param.size();i++)
		ms+=param.at(i).first+"="+param.at(i).second+"&";
	ms+="access_token="+TOKEN();
	ms+="&sig="+MD5(ms+SECRET())+" HTTP/1.0\nHost: api.vk.com\nUser-agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\nAccept: */*\n\n";
	ms="GET "+ms;
	string res=SendAndReceive(ms,continue_to_succeed,true,true);
	Disconnect();
	return res;
}

//string res="GET / HTTP/1.0\nHost: www.codenet.ru\nUser-agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\nAccept: */*\n\n";
//printf("%s",res.c_str());
//send(mysocket,res.c_str(),res.size(),0);
