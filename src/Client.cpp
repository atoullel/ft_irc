#include "../include/Client.hpp"

Client::Client(){
	return ;
}

Client::Client(Client const &src){
	*this = src;
	return ;
}

Client::~Client(){
	return ;
}

Client &Client::operator=(Client const &src){
	if (this != &src) {
							// TO BE CODED
	}
	return (*this);
}


//----------------- Getters -----------------//
//TO BE CODED
//=
void	Client::setFd(int fd){
	this->_fd = fd;
	return;
}

void	Client::setIp(std::string ip){
	this->_ipaddr = ip;
	return ;
}

int			Client::getFd(){
	return (this->_fd);
}

std::string	Client::setBuff(std::string src){
	_buffer += src;
	return ;
}

std::string	Client::getBuff(){
	return (_buffer);
}

void		Client::clearBuff(){
	_buffer.clear();
}