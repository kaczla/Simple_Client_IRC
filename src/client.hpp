#include <iostream>
#include <string>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
using namespace std;

#include "def.hpp"

class Client{
public:
	Client( string _host, int _port );
	Client( string _host, int _port, string _nick, string _user );
	bool ReturnInit();
	bool Send();
private:
	bool Init;
	int Error;
	//Connect to server
	int Port;
	string Host;
	int Socket;
	void SetPort( int &_port );
	void SetHost( string &_host );
	bool StartConnection();
	//User info
	string Nick, User;
	void SetNick( string &_nick );
	void SetUser( string &_user );
};
