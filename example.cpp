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
	RCON rcon(EXAMPLE_IP, EXAMPLE_PORT, EXAMPLE_PASSWORD);
	rcon.Send(EXAMPLE_COMMAND);
	data.assign(rcon.Recv());
	std::cout << "RCON response: " << data << "\n";

	// Query example.
	Query query(EXAMPLE_IP, EXAMPLE_PORT);
	std::string pingdata;
	std::stringstream tostring;
	tostring << ((rand() % 8999) + 1000);
	data.assign(query.Ping(tostring.str()));
	std::cout << "Query response: " << data << "\n";
	return 1;
}