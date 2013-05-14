#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")

typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/timeb.h>

#include <stdlib.h>
#include <fcntl.h>

typedef unsigned char BYTE;
#define closesocket close
#endif


#define RCON_DEFAULT_TIMEOUT 100

class RCON
{
public:	
	int Send(std::string command);
	std::string Recv(int timeout = RCON_DEFAULT_TIMEOUT);

	RCON(std::string ip, const short port, std::string password);
	~RCON();

private:
	int sock;
	struct sockaddr_in server;

	std::string Assemble(std::string data);
	std::string sip;
	std::string spass;
	short sport;

#ifndef WIN32
	unsigned long long int GetTickCount();
#endif
};