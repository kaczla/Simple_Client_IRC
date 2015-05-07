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

Client::~Client(){
	this->Disconnect();
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
	this->Connected = false;
	if( this->Init ){
		this->Socket = -1;
		//CREATIN SOCKET
		cout<<"WAIT: Creating socket";
		this->Socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( this->Socket < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" Creating socket (socket):"<<this->Socket<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		cout<<"\r[1/6] Created socket: "<<this->Socket<<"\n";
	
		//FOR BIND IP AND PORT WITH SOCKET ( WHEN IP AND PORT IS USED )
		cout<<"WAIT: setsockopt";
		int _tmp = 1;
		this->Error = setsockopt( this->Socket, SOL_SOCKET, SO_REUSEADDR, (char const*)&_tmp, sizeof( _tmp ) );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" Creating socket (setsockopt):"<<this->Error<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		cout<<"\r[2/6] setsockopt\n";
		
		//NONBLOCK SOCKET
		cout<<"WAIT: fcntl O_NONBLOCK";
		this->Error = fcntl( this->Socket, F_SETFL, O_NONBLOCK );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" fcntl O_NONBLOCK:"<<this->Error<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		cout<<"\r[3/6] fcntl O_NONBLOCK\n";
		
		//ASYNC
		cout<<"WAIT: fcntl O_ASYNC";
		this->Error = fcntl( this->Socket, F_SETFL, O_ASYNC );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" fcntl O_ASYNC:"<<this->Error<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		cout<<"\r[4/6] fcntl O_ASYNC\n";
		
		//GETHOSTBTNAME
		cout<<"WAIT: gethostbyname";
		hostent* _host = NULL;
		_host = gethostbyname( this->Host.c_str() );
		if( _host == NULL ){
			this->Init = false;
			cout<<RED_ERROR<<" Host = "<<this->Host<<"\n"<<RED_ERROR;
			herror( " h_errno: " );
			cout<<"\n\n"<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		cout<<"\r[5/6] gethostbyname\n";
		
		//Connecting with host
		cout<<"WAIT: Conneting "<<this->Host<<":"<<this->Port;
		sockaddr_in _server;
		_server.sin_family = AF_INET;
		_server.sin_port = htons( this->Port );
		_server.sin_addr = *( ( const in_addr* ) _host->h_addr );
		//memset( &_server.sin_zero, '\0', 8 );
		this->Error = connect( this->Socket, (sockaddr*)&_server, sizeof(_server) );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" connect: "<<strerror( errno )<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		cout<<"\r[6/6] Connected with "<<this->Host<<":"<<this->Port<<"\n";
		this->Error = 0;
		this->Connected = true;
		
		return true;
	}
	return false;
}

void Client::Disconnect(){
	if( this->Connected ){
		close( this->Socket );
		this->Connected = false;
	}
}

bool Client::Reveice(){
	if( this->Connected ){
		this->ErrorReveice = recv( this->Socket, Buffer, MAX_RECV_BUFFER-1, 0 );
		if( this->ErrorReveice > 0 ){
			this->ReveiceText = Buffer;
			return true;
		}
		else{
			return false;
		}		
	}
	return false;
}

bool Client::Send( string &_input ){
	if( this->Connected ){
		this->ErrorSend = send( this->Socket, _input.c_str(), _input.size(), 0 );
		if( this->ErrorSend < 0 ){
			return false;
		}
		else{
			return true;
		}
	}
	return false;
}

