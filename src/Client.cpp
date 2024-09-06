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

Client &Client::operator=(Server const &src){
	if (this != &src) {
							// TO BE CODED
	}
	return (*this);
}

//----------------- Getters -----------------//
//TO BE CODED