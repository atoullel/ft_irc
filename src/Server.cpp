#include "../include/Server.hpp"

bool Server::Signal = false;

Server::Server(){
	return ;
}

Server::Server(Server const &src){
	*this = src;
	return ;
}

Server::Server(int port, std::string password) : _port(port), _password(password){
	std::cout << "Server constructor" << std::endl;
	return ;
}

Server::~Server(){
	std::cout << "The Server Closed!" << std::endl;
	return ;
}

Server &Server::operator=(Server const &src){
	if (this != &src) {
								// TO BE CODED
	}
	return (*this);
}

void Server::initSocket()
{
	int optval = 1;

	_serv_socket = socket(AF_INET, SOCK_STREAM, 0);											//Creating socket IPv4, TCP
	if(_serv_socket == -1)
		throw(std::runtime_error("socket creation failure"));

	if(setsockopt(_serv_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) 	// Making the socket reusable
		throw(std::runtime_error("setsockopt error during server socket init"));
	if (fcntl(_serv_socket, F_SETFL, O_NONBLOCK) == -1)										//Setting fd to be non-blocking
		throw(std::runtime_error("fcntl error during server socket init"));
																							//Initialising server socket addr struct
	_serv_addr.sin_family = AF_INET;														//Setting to IPV4
	_serv_addr.sin_port = htons(port);														//Setting NBO to big-endian
	_serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_serv_socket, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)) == -1)		//Binding server addr to socket
		throw(std::runtime_error("bind error during server socket init"));
	if (listen(_serv_socket, SOMAXCONN) == -1)												//Listening for incoming connections
		throw(std::runtime_error("listen error during server socket init"));
}

void Server::launch(int port, std::string pass)
{
	initSocket();
	std::cout << "Server is now waiting for events" << std::endl;

	_num_socket = 1;
	while (Server::Signal == false)
	{
		_num_events = epoll_wait(_epfd, _events, _num_socket, -1);
		for (size_t i = 0; i < _num_events; i++)
		{
			if (events[i].data.fd == _server_socket && events[i].events == EPOLLIN);
				newClient(_server_socket, this->_rc, &num_socket, events[i]);
			else
				newData(...); 															// I need to rework newClient & NewDate because they weren't working
		}
	}
	// close file descriptors
}

//----------------- Getters -----------------//
//TO BE CODED
