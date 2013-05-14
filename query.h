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

#define QUERY_DEFAULT_TIMEOUT 100

class Query
{
public:
	std::string Information(int timeout = QUERY_DEFAULT_TIMEOUT);
	std::string Rules(int timeout = QUERY_DEFAULT_TIMEOUT);
	std::string ClientList(int timeout = QUERY_DEFAULT_TIMEOUT);
	std::string DetailedPlayerInfo(int timeout = QUERY_DEFAULT_TIMEOUT);
	std::string Ping(std::string data, int timeout = QUERY_DEFAULT_TIMEOUT);

	// Shouldn't really be required, but here if you need it.
	int Send(const char opcode, std::string data = NULL);
	std::string Recv(int timeout = QUERY_DEFAULT_TIMEOUT);

	Query(std::string ip, const short port);
	~Query();

private:
	int sock;
	struct sockaddr_in server;

	std::string Assemble(const char opcode, std::string data);
	std::string sip;
	short sport;

#ifndef WIN32
	unsigned long long int GetTickCount();
#endif
};