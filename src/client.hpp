#include <iostream>
#include <string>
#include <cstring>
//cstring: strerror
#include <cerrno>
//cerrno: errno
#include <cstdlib>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
//fcntl.h: fcntl
#include <unistd.h>
//unistd: close
using namespace std;

#include "def.hpp"

class Client{
public:
	Client( string _host, int _port );
	Client( string _host, int _port, string _nick, string _user );
	~Client();
	bool ReturnInit();
	bool Send();
private:
	bool Init;
	int Error;
	//Connect to server
	int Port;
	string Host;
	int Socket;
	bool Connected;
	void SetPort( int &_port );
	void SetHost( string &_host );
	bool StartConnection();
	void Disconnect();
	//User info
	string Nick, User;
	void SetNick( string &_nick );
	void SetUser( string &_user );
};
