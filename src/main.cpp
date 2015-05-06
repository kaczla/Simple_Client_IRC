#include <iostream>
#include <cstdlib>
#include <string>
#include <pthread.h>
#include "client.hpp"
using namespace std;

#include "def.hpp"

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
			if( input == "quit" ){
				//stop client
				break;
			}
			else{
				//send msg
			}
		}
	}while( true );
	pthread_exit( NULL );
}

int main( int argc, char* argv[] ){
	if( argc < 3 ){
		cout<<RED_COLOR<<BOLD_TEXT<<"NON ARGUMENT: HOST PORT [NICK] [USER]\n"<<DEFAULT_COLOR
			<<"\tHOST - name or IP server\n"
			<<"\tPORT - port server\n"
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
	
	/*
	int Error;
	pthread_t PID;
	Error = pthread_create( &PID, NULL, Input, NULL );
	
	if( Error ){
		cout<<"pthread_create \'Input\' failed: "<<Error<<"\n";
		return 2;
	}*/
	
	//connect
	Client client( Host, Port, Nick, User );
	if( ! client.ReturnInit() ){
		return 2;
	}
	while( true );
	return 0;
}
