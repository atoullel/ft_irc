#include "../include/Client.hpp"

Client::Client()
{
	this->_nickname = "";
	this->_username = "";
	this->_fd = -1;
	this->_operator= false;
	this->_registered = false;
	this->_buffer = "";
	this->_ipadd = "";
	this->_logedin = false;
}

Client::Client(std::string _nickname, std::string _username, int _fd): _fd(_fd), _username(_username), _nickname(_nickname) {}
Client::~Client(){}
Client::Client(Client const &src){*this = src;}
Client &Client::operator=(Client const &src){
	if (this != &src){
		this->_nickname = src._nickname;
		this->_username = src._username;
		this->_fd = src._fd;
		this->ChannelsInvite = src.ChannelsInvite;
		this->_buffer = src._buffer;
		this->_registered = src._registered;
		this->_ipadd = src._ipadd;
		this->_logedin = src._logedin;
	}
	return *this;
}

//---------------//Getters
int Client::GetFd(){return this->_fd;}
bool Client::getRegistered(){return _registered;}
bool Client::GetInviteSalon(std::string &ChName){
	for (size_t i = 0; i < this->ChannelsInvite.size(); i++){
		if (this->ChannelsInvite[i] == ChName)
			return true;
	}
	return false;
}
std::string Client::GetNickName(){return this->_nickname;}
bool Client::GetLogedIn(){return this->_logedin;}
std::string Client::GetUserName(){return this->_username;}
std::string Client::getBuffer(){return _buffer;}
std::string Client::getIpAdd(){return _ipadd;}
std::string Client::getHostname(){
	std::string hostname = this->GetNickName() + "!" + this->GetUserName();
	return hostname;
}

//---------------//Setters
void Client::SetFd(int _fd){this->_fd = _fd;}
void Client::SetNickname(std::string& _nickname){this->_nickname = _nickname;}
void Client::setLogedin(bool value){this->_logedin = value;}
void Client::SetUsername(std::string& _username){this->_username = _username;}
void Client::setBuffer(std::string recived){_buffer += recived;}
void Client::setRegistered(bool value){_registered = value;}
void Client::setIpAdd(std::string ipadd){this->_ipadd = ipadd;}

//---------------//Methods
void Client::clearBuffer(){_buffer.clear();}
void Client::AddSalonlInvite(std::string &chname){
	ChannelsInvite.push_back(chname);
}
void Client::RmSalonInvite(std::string &chname){
	for (size_t i = 0; i < this->ChannelsInvite.size(); i++){
		if (this->ChannelsInvite[i] == chname)
			{this->ChannelsInvite.erase(this->ChannelsInvite.begin() + i); return;}
	}
}

void	Client::SetIp(std::string ip){
	this->_ipaddr = ip;
	return ;
}
