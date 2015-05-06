#include "client.hpp"

Client::Client( string _host, int _port ){
	this->Init = true;
	this->SetPort( _port );
	this->SetHost( _host );
	this->Nick = DEFAULT_NICK;
	this->User = DEFAULT_USER_NAME;
	this->Init = this->StartConnection();
}

Client::Client( string _host, int _port, string _nick, string _user ){
	this->Init = true;
	this->SetPort( _port );
	this->SetHost( _host );
	this->SetNick( _nick );
	this->SetUser( _user );
	this->Init = this->StartConnection();
}

void Client::SetPort( int &_port ){
	if( _port > 1024 and _port <= 65535 ){
		this->Port = _port;
	}
	else{
		this->Init = false;
		cout<<RED_ERROR<<" Wrong PORT: "<<_port<<"\n";
	}
	
}

void Client::SetHost( string &_host ){
	if( _host.empty() ){
		this->Init = false;
		cout<<RED_ERROR<<" HOST is empty!\n";
	}
	else{
		this->Host = _host;
	}	
}

bool Client::ReturnInit(){
	return this->Init;
}

void Client::SetNick( string &_nick ){
	this->Nick = _nick;
}

void Client::SetUser( string &_user ){
	this->User = _user;
}

bool Client::StartConnection(){
	if( this->Init ){
		this->Socket = -1;
		//CREATIN SOCKET
		this->Socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( this->Socket < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" Creating socket (socket):"<<this->Socket<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		int _tmp = 1;
		//FOR BIND IP AND PORT WITH SOCKET ( WHEN IP AND PORT IS USED )
		this->Error = setsockopt( this->Socket, SOL_SOCKET, SO_REUSEADDR, (char const*)&_tmp, sizeof( _tmp ) );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" Creating socket (setsockopt):"<<this->Error<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		//NONBLOCK SOCKET
		this->Error = fcntl( this->Socket, F_SETFL, O_NONBLOCK );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" fcntl O_NONBLOCK:"<<this->Error<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		//ASYNC
		this->Error = fcntl( this->Socket, F_SETFL, O_ASYNC );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" fcntl O_ASYNC:"<<this->Error<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n"; 
			return false;
		}
		this->Error = 0;
		return true;
	}
	return false;
}
