#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include "client.hpp"
using namespace std;

#include "def.hpp"

#include <signal.h>


Client * Pointer = NULL;
bool running = true;
string ToSend;
string Nick;
string Channel;

struct InputMessage_{
	string Command;
	vector <string> Args;
	int Argc;
	int Find;
	void Clear(){
		this->Command.clear();
		this->Args.clear();
		this->Find = 0;
		this->Argc = 0;
	}
	void Up(){
		transform( this->Command.begin(), this->Command.end(), this->Command.begin(), towupper );
	}
} InputMessage;

void ParseInput( string &_input );

void* Input( void* _input );

//Signal SIGPIPE
void _Singal( int _signum ){
	cout<<"Caught singal SIGPIPE "<<_signum<<"\n";
}

int main( int argc, char* argv[] ){
	signal( SIGPIPE, _Singal );
	if( argc < 3 ){
		cout<<RED_COLOR<<BOLD_TEXT<<"NON ARGUMENT: HOST PORT [NICK] [USER]\n"<<DEFAULT_COLOR
			<<"\tHOST - server name\n"
			<<"\tPORT - server port\n"
			<<"\tNICK - your nick \t(default: "<<DEFAULT_NICK<<")\n"
			<<"\tUSER - your user name \t(default: "<<DEFAULT_USER_NAME<<")\n";
		return 1;
	}
	string Host = argv[1];
	int Port = atoi( argv[2] );
	Nick = DEFAULT_NICK;
	string User( DEFAULT_USER_NAME );
	if( argc >= 4 ){
		Nick = argv[3];
		User = Nick;
	}
	if( argc >= 5 ){
		User = argv[4];
	}
	bool Debug = false;
	
	int Error;
	pthread_t PID_Input;
	
	//connect
	Client client( Host, Port, Nick, User );
	Pointer = &client;	
	if( ! client.ReturnInit() ){
		return 2;
	}
	client.SetDebug( Debug );
	
	//Input thread
	Error = pthread_create( &PID_Input, NULL, Input, NULL );		
	if( Error ){
		cout<<"pthread_create \'Input\' failed: "<<Error<<"\n";
		return 2;
	}
	
	//Reveice data
	while( client.ReturnConnected() and running ){
		client.ReveiceData();
	}
	pthread_join( PID_Input, NULL );
	return 0;
}

void* Input( void* _input ){
	string input;
	if( Pointer == NULL ){
		cout<<"Problem with connect!\n";
		running = false;
		pthread_exit( NULL );
	}
	Channel.clear();
	do{
		getline( cin, input );
		if( input == "" ){
			continue;
		}
		
		if( input[0] == '/' or input[0] == '\\' ){
			//do cmd
			ParseInput( input );
		}
		else{
			//send msg
			if( Channel.empty() ){
				cout<<LINE_UP;
				cout<<Pointer->ReturnTime_()<<RED_COLOR<<"Join to channel!\n"<<DEFAULT_COLOR;
			}
			else{
				cout<<LINE_UP;
				input = "MSG " + Channel + ' ' + input;
				ParseInput( input );
			}				
		}
	}while( running );
	running = false;
	pthread_exit( NULL );
}

void ParseInput( string &_input ){
	if( _input[0] == '/' or _input[0] == '\\' ){
		_input = _input.substr( 1 );
	}
	InputMessage.Clear();
	InputMessage.Find = _input.find( ' ' );
	InputMessage.Command = _input.substr( 0, InputMessage.Find );
	InputMessage.Up();
	InputMessage.Find = _input.find( ' ' );
	_input = _input.substr( InputMessage.Find + 1 );
	InputMessage.Argc = count( _input.begin(), _input.end(), ' ' ) + 1;
	if( InputMessage.Command == "MSG" ){
		if( InputMessage.Argc < 2 ){
			cout<<RED_COLOR<<"Not enough arguments!\n"<<DEFAULT_COLOR;
		}
		else{
			InputMessage.Find = _input.find( ' ' );
			InputMessage.Args.push_back( _input.substr( 0, InputMessage.Find ) );
			InputMessage.Find = _input.find( ' ' );
			InputMessage.Args.push_back( _input.substr( InputMessage.Find + 1 ) );
			if( InputMessage.Args[0][0] == '#' ){
				cout<<Pointer->ReturnTime_()<<BOLD_TEXT<<Nick<<DEFAULT_COLOR<<": "<<InputMessage.Args.at( 1 )<<"\n";
			}
			else{
				cout<<Pointer->ReturnTime_()<<BOLD_TEXT<<Nick<<DEFAULT_COLOR<<"->"<<InputMessage.Args.at( 0 )<<": "<<InputMessage.Args.at( 1 )<<"\n";
			}			
			ToSend = "PRIVMSG " + InputMessage.Args.at( 0 ) + " :"  + InputMessage.Args.at( 1 );
			Pointer->Send( ToSend );
		}
	}
	else if( InputMessage.Command == "JOIN" ){
		if( InputMessage.Argc < 1 ){
			cout<<RED_COLOR<<"Not enough arguments!\n"<<DEFAULT_COLOR;
		}
		else{
			if( _input[0] != '#' ){
				if( ! Channel.empty() ){
					ToSend = "PART " + Channel + " :change channel";
					Pointer->Send( ToSend );
				}
				
				Channel = '#' + _input;
				ToSend = "JOIN " + Channel;
				Pointer->Send( ToSend );
			}
		}
	}
	else if( InputMessage.Command == "PART" ){
		if( InputMessage.Argc < 1 ){
			cout<<RED_COLOR<<"Not enough arguments!\n"<<DEFAULT_COLOR;
		}
		else{
			InputMessage.Find = _input.find( ' ' );
			InputMessage.Args.push_back( _input.substr( 0, InputMessage.Find ) );
			ToSend = "PART ";
			if( InputMessage.Args[0][0] != '#' ){
				ToSend += '#';
			}
			ToSend += InputMessage.Args[0] + " :leaving";
			Pointer->Send( ToSend );
			if( Channel == InputMessage.Args[0] or Channel == '#' + InputMessage.Args[0] ){
				Channel.clear();
			}
		}
	}
	else if( InputMessage.Command == "QUIT" ){
		running = false;
		if( Pointer != NULL ){
			Pointer->StopClient();
		}
	}
	else{
		cout<<RED_COLOR<<"Unknown command: "<<InputMessage.Command<<"\n"<<DEFAULT_COLOR;
		//cout<<"COMMAND="<<InputMessage.Command<<"\nARGC="<<InputMessage.Argc<<"\n";
	}
}
