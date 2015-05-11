#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
//cstring: strerror
#include <cerrno>
//cerrno: errno
#include <cstdlib>
#include <ctime>
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
	bool ReturnConnected();
	bool Send( string &_input );
	void ReveiceData();
	void StopClient();
	void SetDebug( bool _input );
	string ReturnTime_();
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
	//Send, Reveice
	int ErrorSend, ErrorReveice;
	char Buffer[MAX_RECV_BUFFER];
	string ReveiceText;
	istringstream Word;
	string Line, CopyLine;
	size_t Find;
	bool Reveice();
	void Parse();
	//Message
	string Prefix;
	string Command;
	vector <string> Param;
	string Message;
	void Action();
	//User info
	string Nick, User;
	void SetNick( string &_nick );
	void SetUser( string &_user );
	//time
	time_t Time;
	struct tm * TimeInfo;
	char TimeBuffer[16];
	string ReturnTime();
	//other
	bool Debug, Advance;
	size_t Find1, Find2;
	string Ping;
	string TMP;
	vector <string>::iterator It;
};
