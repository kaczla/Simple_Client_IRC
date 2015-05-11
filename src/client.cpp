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
	this->Debug = false;
	this->Advance = false;
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
		memset( &_server.sin_zero, '\0', 8 );
		this->Error = connect( this->Socket, (sockaddr*)&_server, sizeof(_server) );
		if( this->Error < 0 ){
			this->Init = false;
			cout<<RED_ERROR<<" connect: "<<strerror( errno )<<"\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		cout<<"\r[6/6] Connected with "<<this->Host<<":"<<this->Port<<"\n";
		this->Connected = true;
		
		//LOG IN
		this->TMP = "NICK " + this->Nick;
		if( this->Send( this->TMP ) ){
			this->TMP = "USER " + this->User + " " + this->User + " * :Simple Client IRC";
			if( this->Send( this->TMP ) ){
				cout<<this->ReturnTime()<<"Logged.\n";
			}
			else{
				cout<<RED_ERROR<<" Problem with connection!\n\n"
					<<RED_COLOR<<"### APPLICATION ABORTED ###"
					<<DEFAULT_COLOR<<"\n";
				return false;
			}
		}
		else{
			cout<<RED_ERROR<<" Problem with connection!\n\n"
				<<RED_COLOR<<"### APPLICATION ABORTED ###"
				<<DEFAULT_COLOR<<"\n";
			return false;
		}
		
		this->Error = 0;
		return true;
	}
	return false;
}

void Client::Disconnect(){
	if( this->Connected ){
		this->TMP = "QUIT :Leaving";
		this->Send( this->TMP );
		cout<<this->ReturnTime()<<"Disconnected\n";
		close( this->Socket );
		this->Connected = false;
	}
}

bool Client::Reveice(){
	if( this->Connected ){
		memset( this->Buffer, 0, MAX_RECV_BUFFER );
		this->ErrorReveice = recv( this->Socket, Buffer, MAX_RECV_BUFFER-1, 0 );
		if( this->ErrorReveice > 0 ){
			this->ReveiceText = this->Buffer;
			if( this->Debug ){
				cout<<MAGENTA_COLOR<<">> "<<this->ReveiceText<<DEFAULT_COLOR<<"\n";
			}
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
		if( _input.back() != '\n' ){
			_input += '\n';
		}
		this->ErrorSend = send( this->Socket, _input.c_str(), _input.size(), 0 );
		if( this->ErrorSend < 0 ){
			return false;
		}
		else{
			if( this->Debug ){
				cout<<BLUE_COLOR<<"<< "<<_input<<DEFAULT_COLOR<<"\n";
			}
			return true;
		}
	}
	return false;
}

bool Client::ReturnConnected(){
	return this->Connected;
}

void Client::SetDebug( bool _input ){
	this->Debug = _input;
}

void Client::StopClient(){
	this->Disconnect();
	cout<<this->ReturnTime()<<RED_COLOR<<"STOP CLIENT"<<DEFAULT_COLOR<<"\n";
}

void Client::ReveiceData(){
	if( this->Reveice() ){
		this->Word.clear();
		this->Word.str( this->ReveiceText );
		while( getline( this->Word, this->Line ) ){
			if( this->Line.find( '\r' ) != string::npos ){
				this->Line.substr( 0, this->Line.size() -1 );
			}
			this->Parse();
		}
	}
}

void Client::Parse(){
	this->CopyLine = this->Line;
	//Prefix
	if( this->Line[0] == ':' ){
		this->Find = this->Line.find( ' ' );
		this->Prefix = this->Line.substr( 1, this->Find - 1 ); 
		this->Find = this->Line.find( ' ' );
		this->Line = this->Line.substr( this->Find + 1 );
	}
	else{
		this->Prefix.clear();
	}
	
	//Command
	this->Find = this->Line.find( ' ' );
	this->Command = this->Line.substr( 0,  this->Find );
	transform( this->Command.begin(), this->Command.end(), this->Command.begin(), towupper );	
	this->Find = this->Line.find( ' ' );
	if( this->Find != string::npos ){
		this->Line = this->Line.substr( this->Find + 1 );
	}
	else{
		this->Line.clear();
	}
	
	//Param + Message
	this->Param.clear();
	this->Message.clear();
	if( ! this->Line.empty() ){
		if( this->Line[0] == ':' ){
			this->Message = this->Line.substr( 1 );
			while( this->Message.back() == '\r' or this->Message.back() == ' ' ){
				this->Message.pop_back();
			}
			this->Param.push_back( this->Message );
		}
		else{
			this->Find1 = 0;
			while( ( this->Find2 = this->Line.find( ' ', this->Find1 ) ) != string::npos ){
				this->Param.push_back( this->Line.substr( this->Find1, this->Find2 - this->Find1 ) );
				this->Find1 = this->Find2 + 1;
				if( this->Line[this->Find1] == ':' ){
					this->Message = this->Line.substr( this->Find1 + 1 );
					while( this->Message.back() == '\r' or this->Message.back() == ' ' ){
						this->Message.pop_back();
					}
					this->Param.push_back( this->Message );
					break;
				}
			}
			while( this->Line[0] == ' ' ){
				this->Line.erase( 0, 1 );
			}
			if( ! this->Line.empty() ){
				if( this->Param.empty() ){
					this->Param.push_back( this->Line );
				}
			}			
		}
	}
	this->Line.clear();
	
	if( this->Advance ){
		cout<<GREEN_COLOR<<">> "<<this->CopyLine<<DEFAULT_COLOR<<"\n";
		cout<<"Prefix: "<<CYAN_COLOR<<this->Prefix<<DEFAULT_COLOR;
		cout<<"\nCommand: "<<CYAN_COLOR<<this->Command<<DEFAULT_COLOR;
		cout<<"\nParam: ";
		if( ! this->Param.empty() ){
			for( this->It = this->Param.begin(); this->It != this->Param.end(); ++this->It ){
				cout<<CYAN_COLOR<<*this->It<<" "<<DEFAULT_COLOR;
			}
		}
		cout<<"\nMessage: "<<CYAN_COLOR<<this->Message<<DEFAULT_COLOR<<"\n";
	}
	this->Action();
}

void Client::Action(){
	if( this->Command.empty() ){
		return;
	}
	else{
		if( this->Command == "PRIVMSG" ){
			if( this->Param[0][0] == '#' ){
				cout<<this->ReturnTime()<<this->Prefix<<"@"<<this->Param[0]<<": "<<this->Message<<"\n";
			}
			else{
				cout<<this->ReturnTime()<<this->Prefix<<": "<<this->Message<<"\n";
			}
		}
		else if( this->Command == "PING" ){
			this->Ping = "PONG " + this->Param[0];
			this->Send( this->Ping );
		}
		else if( this->Command == "PONG" ){
			
		}
		else if( this->Command == "NOTICE" ){
			cout<<this->ReturnTime()<<CYAN_COLOR<<"-"<<this->Prefix<<"- "<<this->Message<<DEFAULT_COLOR<<"\n";
		}
		else if( this->Command == "JOIN" ){
			cout<<this->ReturnTime()<<GREEN_COLOR<<this->Prefix<<" join "<<this->Param.at( 0 )<<DEFAULT_COLOR<<"\n";
		}
		else if( this->Command == "PART" ){
			cout<<this->ReturnTime()<<GREEN_COLOR<<this->Prefix<<" leave "<<this->Param.at( 0 )<<DEFAULT_COLOR<<"\n";
		}
		else if( this->Command == "NICK" ){
			cout<<this->ReturnTime()<<GREEN_COLOR<<this->Prefix<<" changed nick to "<<this->Param.at( 0 )<<DEFAULT_COLOR<<"\n";
		}		
		else if( this->Command == "QUIT" ){
			cout<<this->ReturnTime()<<GREEN_COLOR<<this->Prefix<<" quits ( "<<this->Message<<" )\n"<<DEFAULT_COLOR;
		}
		else if( this->Command == "353" ){
			cout<<this->ReturnTime()<<CYAN_COLOR<<"People on "<<this->Param.at( 2 )<<": "<<DEFAULT_COLOR<<this->Param.at( 3 )<<"\n";
		}
		else if( this->Command == "433" ){
			cout<<this->ReturnTime()<<this->Param.at( 1 )<<": "<<RED_COLOR<<this->Message<<DEFAULT_COLOR<<"\n";
		}	
		else if( this->Command == "001" or this->Command == "002"	or this->Command ==	"003"	
			or this->Command == "004"	or this->Command == "005"	or this->Command == "250"	
			or this->Command == "251"	or this->Command == "252"	or this->Command == "253"	
			or this->Command == "254"	or this->Command == "255"	or this->Command == "265"	
			or this->Command == "266"	or this->Command == "366"	or this->Command == "372"	
			or this->Command == "375"	or this->Command == "376"	or this->Command == "439" )
		{
			cout<<this->ReturnTime()<<CYAN_COLOR;
			this->It = this->Param.begin();
			++this->It;
			for( ; this->It != this->Param.end(); ++It ){
				cout<<*this->It<<" ";
			}
			cout<<DEFAULT_COLOR<<"\n";
		}
		else{
			//Unknown command
			if( this->Debug ){
				cout<<RED_ERROR<<" Unknown command: "<<RED_COLOR<<this->Command<<DEFAULT_COLOR<<"\n";
			}
		}
	}
}

string Client::ReturnTime(){
	time( &this->Time );
	TimeInfo = localtime( &this->Time );
	strftime( this->TimeBuffer, 16, "[%T] ", this->TimeInfo );
	return this->TimeBuffer ;
}

string Client::ReturnTime_(){
	return this->ReturnTime();
}
