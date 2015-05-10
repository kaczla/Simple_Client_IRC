#include <iostream>
#include <cstdlib>
#include <string>
#include <pthread.h>
#include "client.hpp"
using namespace std;

#include "def.hpp"

#include <signal.h>


Client * Pointer = NULL;
bool running = true;

void* Input( void* _input ){
	string input;
	do{
		getline( cin, input );
		if( input == "" ){
			continue;
		}
		
		if( input[0] == '/' or input[0] == '\\' ){
			//do cmd
		}
		else{
			//stop client
			if( input == "quit" ){
				if( Pointer != NULL ){
					running = false;
					Pointer->StopClient();
				}								
			}
			else{
				//send msg			
			}
		}
	}while( running );
	running = false;
	pthread_exit( NULL );
}

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
	string Nick( DEFAULT_NICK ), User( DEFAULT_USER_NAME );
	if( argc >= 4 ){
		Nick = argv[3];
		User = Nick;
	}
	if( argc >= 5 ){
		User = argv[4];
	}	
	
	int Error;
	pthread_t PID_Input;
	
	//connect
	Client client( Host, Port, Nick, User );
	Pointer = &client;	
	if( ! client.ReturnInit() ){
		return 2;
	}
	client.SetDebug( true );
	
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
