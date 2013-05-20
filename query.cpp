/*
	C++ Implementation of the SA-MP RCON/Query mechanism.
	
    Copyright (C) 2013 Cody Cunningham

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "query.h"

#ifndef WIN32
unsigned long long int Query::GetTickCount()
{
	std::ifstream uptime("/proc/uptime");
	if(uptime)
	{
		char *uptimebuf = new char[128];
		uptime.read(uptimebuf, 128);

		for(int i = 0; i < 128; i++)
		{
			if(uptimebuf[i] == ' ' || (uptimebuf[i - 1] == '\0' && i > 1))
			{
				uptimebuf[i] = '\0';
			}
		}
		double time = atof(uptimebuf);

		delete[] uptimebuf;
		return (time * 1000);
	}
	else
	{
		struct timeb tp;
		ftime(&tp);
		return ((tp.time * 1000) + tp.millitm);
	}
}
#endif

std::string Query::Information(int timeout)
{
	if(Send('i') < 0) return NULL;
	return Recv(timeout);
}

std::string Query::Rules(int timeout)
{
	if(Send('r') < 0) return NULL;
	return Recv(timeout);
}

std::string Query::ClientList(int timeout)
{
	if(Send('c') < 0) return NULL;
	return Recv(timeout);
}

std::string Query::DetailedPlayerInfo(int timeout)
{
	if(Send('d') < 0) return NULL;
	return Recv(timeout);
}

std::string Query::Ping(std::string data, int timeout)
{
	if(Send('p', data) < 0) return NULL;
	return Recv(timeout);
}

int Query::Send(const char opcode, std::string data)
{
	if(sock < 1) return -1;

	std::string packet = Assemble(opcode, data);
	if(packet.length() < 4) return -1;

	return sendto(sock, packet.c_str(), packet.length(), 0, (sockaddr*)&server, sizeof(sockaddr_in));
}

std::string Query::Recv(int timeout)
{
	if(sock < 1) return NULL;

	std::string packet;
	char cbuffer[512];
	memset(cbuffer, '\0', 512);

	sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	int recvbytes = 0;
	unsigned long long int starttime = GetTickCount();

	while(GetTickCount() - starttime < (unsigned long long int)timeout)
	{
		recvbytes = recvfrom(sock, cbuffer, 512, 0, (sockaddr*)&from, &fromlen);
		if(recvbytes > 11)
		{
			if(packet.length() > 0) packet.append("\n");

			int state = 0;
			for(int i = 11; i < recvbytes; i++)
			{
				if(!(cbuffer[i] >= 32 && cbuffer[i] <= 126) && state == 0)
				{
					state = 1;
					packet.append("\n");
				}
				else if((cbuffer[i] >= 32 && cbuffer[i] <= 126))
				{
					state = 0;
					packet += cbuffer[i];
				}
			}

			starttime = GetTickCount();
			memset(cbuffer, '\0', 512);
		}
	}
	return packet;
}

std::string Query::Assemble(const char opcode, std::string data)
{
	if(sip.length() < 4 || sport < 1) return NULL;

	std::string packet("SAMP");

	// Split the IP into 4 bytes.
	std::stringstream it(sip);
	std::string part;
	while(std::getline(it, part, '.')) 
	{
		packet += (BYTE)atoi(part.c_str());
	}

	// Split the port into 2 bytes.
	packet += (BYTE)(sport & 0xFF);
	packet += (BYTE)(sport >> 8 & 0xFF);

	packet += opcode;

	if(data.length() > 0)
	{
		packet.append(data);
	}
	return packet;
}

Query::Query(std::string ip, const short port)
{
#ifdef WIN32
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa)) return; // Failed to start up WSA.
#endif

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock < 1) // Failed to create socket.
	{
		std::cerr << "Failed to create socket.\n";
		sock = 0;
		#ifdef WIN32
		WSACleanup();
		#endif
		return;
	}

#ifdef WIN32
	DWORD toggle = 1;
	if (ioctlsocket(sock, FIONBIO, &toggle) < 0)
	{
		std::cerr << "Failed to set socket to non-blocking.\n";
		closesocket(sock);
		sock = 0;
		#ifdef WIN32
		WSACleanup();
		#endif
		return;
	}
#else
	int toggle = 1;
	if (fcntl(sock, F_SETFL, O_NONBLOCK, toggle) < 0)
	{
		std::cerr << "Failed to set socket to non-blocking.\n";
		closesocket(sock);
		sock = 0;
		#ifdef WIN32
		WSACleanup();
		#endif
		return;
	}
#endif

	memset((void*)&server, '\0', sizeof(sockaddr_in));

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip.c_str());

	// For the sake of Assemble
	sip.assign(ip);
	sport = port;
}

Query::~Query()
{
	closesocket(sock);
}