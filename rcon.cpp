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
#include "rcon.h"

#ifndef WIN32
unsigned long long int RCON::GetTickCount()
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

int RCON::Send(std::string command)
{
	if(sock < 1) return -1;

	std::string packet = Assemble(command);
	if(packet.length() < 4) return -1;

	return sendto(sock, packet.c_str(), packet.length(), 0, (sockaddr*)&server, sizeof(sockaddr_in));
}

std::string RCON::Recv(int timeout)
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
		if(recvbytes > 13)
		{
			if(packet.length() > 0) packet.append("\n");
			for(int i = 0; i < 13; i++)
			{
				if(cbuffer[i] == 0) cbuffer[i]++;
			}
			packet.append(cbuffer, 13, strlen(cbuffer));
			starttime = GetTickCount();
			memset(cbuffer, '\0', 512);
		}
	}
	return packet;
}

std::string RCON::Assemble(std::string data)
{
	if(sip.length() < 4 || sport < 1) return NULL;

	std::string packet("SAMP");

	// Split the IP into 4 bytes and append it to the packet.
	std::stringstream it(sip);
	std::string part;
	while(std::getline(it, part, '.')) 
	{
		packet += (BYTE)atoi(part.c_str());
	}

	// Split the port into 2 bytes and append it to the packet.
	packet += (BYTE)(sport & 0xFF);
	packet += (BYTE)(sport >> 8 & 0xFF);

	// Append the RCON opcode to the packet.
	packet += 'x';

	// Split the password length into 2 bytes and append it to the packet.
	packet += (BYTE)(spass.length() & 0xFF);
	packet += (BYTE)(spass.length() >> 8 & 0xFF);

	// Append the password to the packet.
	packet.append(spass);

	// Split the data length into 2 bytes and append it to the packet.
	packet += (BYTE)(data.length() & 0xFF);
	packet += (BYTE)(data.length() >> 8 & 0xFF);

	// Append the data on to the packet.
	packet.append(data);

	return packet;
}

RCON::RCON(std::string ip, const short port, std::string password)
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
	spass.assign(password);
	sport = port;
	
	Send("echo RCON admin connected to the server.");
}

RCON::~RCON()
{
	closesocket(sock);
}