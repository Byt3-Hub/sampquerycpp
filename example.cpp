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
#include "query.h"

#define RCON_EXAMPLE // Comment (//) out this line if you do not want the RCON example.
#define QUERY_EXAMPLE // Comment (//) out this line if you do not want the Query example. 

#define EXAMPLE_IP "127.0.0.1" // The IP of the server.
#define EXAMPLE_PORT 7777 // The port of the server.
#define EXAMPLE_PASSWORD "changeme" // The RCON password of the server.
#define EXAMPLE_COMMAND "cmdlist" // The RCON command you want to execute on the above server.

int main()
{
	std::string data;
	
	// RCON Example.
	#ifdef RCON_EXAMPLE
	RCON rcon(EXAMPLE_IP, EXAMPLE_PORT, EXAMPLE_PASSWORD);
	rcon.Send(EXAMPLE_COMMAND);
	data.assign(rcon.Recv());
	std::cout << "RCON response: " << data << "\n";
	#endif
	
	#ifdef QUERY_EXAMPLE
	// Query example.
	Query query(EXAMPLE_IP, EXAMPLE_PORT);
	std::string pingdata;
	std::stringstream tostring;
	tostring << ((rand() % 8999) + 1000);
	data.assign(query.Ping(tostring.str()));
	std::cout << "Query response: " << data << "\n";
	#endif
	return 1;
}