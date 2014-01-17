C++ implementation of the RCON/Query mechanism.
===========

So as I was working on a C++ project, I needed to use the RCON mechanism of a SA-MP server to run some commands, so to avoid "reinventing the wheel", I went and searched around for a C++ implementation of this mechanism, sadly the only one that I found had all broken links and likely no chance for them to be fixed, so I started writing up this!

How to use it.
===========

It's quite simple actually, I've broken it up into a couple classes and here's the gist of it...

Sending an "echo" RCON command to a server on localhost (127.0.0.1), port 7777 with password changeme, then recieving the response from the server and printing it into the console:
```cpp
RCON rcon("127.0.0.1", 7777, "changeme");
rcon.Send("echo Hello there!");
std::string recvval = rcon.Recv();
std::cout << "RCON Response: " << recvval << "\n";
```

Sending a ping request to a server on localhost (127.0.0.1), port 7777 then recieving the response and printing it into the console:
```cpp
Query query("127.0.0.1", 7777);
std::string recvval = query.Ping("5256");
std::cout << "Ping Response: " << recvval << "\n";
```

Pretty simple, right? For a more detailed example see "example.cpp".

The functions.
===========

RCON:
```cpp
int Send(std::string command); // Send an RCON command to the IP/port used during construction.
std::string Recv(); // Recieve responses from the IP/port used during construction.

RCON(std::string ip, const short port, std::string password); // Construct the RCON class with the specified IP, port and password.
~RCON(); // Destruct the RCON class.
```

Query:
```cpp
std::string Information(); // Get information of the server on the IP/port used during construction.
std::string Rules(); // Get the rules of the server on the IP/port used during construction.
std::string ClientList(); // Get the client list of the server on the IP/port used during construction.
std::string DetailedPlayerInfo(); // Get detailed client list of the server on the IP/port used during construction.
std::string Ping(std::string data); // Send a ping request with the data (4 psuedo-random characters) to the server on the IP/port used during construction.

int Send(const char opcode, std::string data); // Send a request to the server on the IP/port specified during construction.
std::string Recv(); // Get responses from the server on the IP/port specified during construction.

Query(std::string ip, const short port); // Construct the Query class with the IP/port specified.
~Query(); // Destruct the Query class.
```

The end.
===========

Go ahead and check it out, if you see any flaws/bugs or room for improvement either make an issue on the repository or even fork the repository, make the changes and then fire off a pull request, I'll review the changes asap.
