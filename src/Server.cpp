#include "../include/Server.hpp"
#include <sstream>

bool Server::Signal = false;

Server::Server(){
	return ;
}

Server::Server(Server const &src){
	*this = src;
	return ;
}

Server::Server(int port, std::string _password) : _port(_port), _password(_password){
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

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal caught" << std::endl;
	Server::Signal = true;
}

void Server::Init_server()
{
	int optval = 1;
	struct epoll_event	serv_event;

	_serv_socket = socket(AF_INET, SOCK_STREAM, 0);											//Creating socket IPv4, TCP
	if(_serv_socket == -1)
		throw(std::runtime_error("socket creation failure"));

	if(setsockopt(_serv_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) 	// Making the socket reusable
		throw(std::runtime_error("setsockopt error during server socket init"));
	if (fcntl(_serv_socket, F_SETFL, O_NONBLOCK) == -1)										//Setting fd to be non-blocking
		throw(std::runtime_error("fcntl error during server socket init"));
																							//Initialising server socket addr struct
	_serv_addr.sin_family = AF_INET;														//Setting to IPV4
	_serv_addr.sin_port = htons(_port);														//Setting NBO to big-endian
	_serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_serv_socket, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)) == -1)		//Binding server addr to socket
		throw(std::runtime_error("bind error during server socket init"));
	if (listen(_serv_socket, SOMAXCONN) == -1)												//Listening for incoming connections
		throw(std::runtime_error("listen error during server socket init"));
	
	_epfd = epoll_create(10); //Creation of the epoll instance. The argument helps the kernel to decide the size of the epoll instance
	if (_epfd == -1)
		throw(std::runtime_error("epoll instance creation failure"));
	serv_event.events = EPOLLIN;
	serv_event.data.fd = _serv_socket;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _serv_socket, &serv_event) == -1)
		throw(std::runtime_error("init epoll_ctl failure"));

	
}

void Server::Launch()
{
	int	n_events;

	Init_server();
	std::cout << "Server is now waiting for events" << std::endl;

	n_events = 0;
	while (Server::Signal == false)
	{
		n_events = epoll_wait(_epfd, _evlist, MAX_EVENTS, -1);
		for (size_t i = 0; i < n_events; i++)
		{
			if (_evlist[i].data.fd == _serv_socket)
				newClient();
			else if (_evlist[i].events & EPOLLIN)
				newData(_evlist[i].data.fd);

			if (_evlist[i].events & (EPOLLRDHUP | EPOLLHUP)){
				epoll_ctl(_epfd, EPOLL_CTL_DEL, _evlist[i].data.fd, NULL);
				//add Method to remove the client object from the clients vector and somthing to remove Salons
				close(_evlist[i].data.fd);
			}									// I need to rework newClient & NewDate because they weren't working
		}
	}
	// Normally closing the epoll instance should close all associated FDs
	close(_epfd);
}

void	Server::newClient(){
	Client cl_new;
	struct sockaddr_in cl_addr;
	struct epoll_event	cl_event;
	int					cl_socket;

	memset(&cl_addr, 0, sizeof(cl_addr));
	socklen_t len = sizeof(cl_addr);

	int cl_socket = accept(_serv_socket, (sockaddr *)&(cl_addr), &len);
	if (cl_socket == -1){
		ft_error("newClient accept failure");
		return;
		}
	if (fcntl(cl_socket, F_SETFL, O_NONBLOCK) == -1){
		ft_error("newClient fcntl failure");
		return;
	}

	cl_event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	cl_event.data.fd = cl_socket;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, cl_socket, &cl_event) == -1){
		ft_error("newClient epoll_ctl failure");
		return;
	}

	// cl_new.setFd(cl_socket);												  ICI NE PAS OUBLIER
	// cl_new.setIp(inet_ntoa((cl_addr.sin_addr)));                           ICI NE PAS OUBLIER
	_clients.push_back(cl_new);
	return ;
}

void	Server::newData(int	fd){

	return ;
}

void	Server::ft_error(std::string reason){
	std::string error(strerror(errno));
	std::cerr << RED << reason << " : " << error << WHITE << std::endl;
	return ;
}


//Remove
void Server::RemoveClient(int fd){
	for (size_t i = 0; i < this->_clients.size(); i++){
		if (this->_clients[i].GetFd() == fd)
			{this->_clients.erase(this->_clients.begin() + i); return;}
	}
}

void Server::RemoveSalon(std::string name){
	for (size_t i = 0; i < this->_salon.size(); i++){
		if (this->_salon[i].GetName() == name)
			{this->_salon.erase(this->_salon.begin() + i); return;}
	}
}

void	Server::RmSalons(int fd){
	for (size_t i = 0; i < this->_salon.size(); i++){
		int flag = 0;
		if (_salon[i].get_client(fd))
			{_salon[i].remove_client(fd); flag = 1;}
		else if (_salon[i].get_admin(fd))
			{_salon[i].remove_admin(fd); flag = 1;}
		if (_salon[i].GetClientsNumber() == 0)
			{_salon.erase(_salon.begin() + i); i--; continue;}
		if (flag){
			std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT Quit\r\n";
			_salon[i].sendTo_all(rpl);
		}
	}
}

//---------------//Send
void Server::senderror(int code, std::string clientname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << msg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() faild" << std::endl;
}

void Server::senderror(int code, std::string clientname, std::string salonname, int fd, std::string msg)
{
	std::stringstream ss;
	ss << ":localhost " << code << " " << clientname << " " << salonname << msg;
	std::string resp = ss.str();
	if(send(fd, resp.c_str(), resp.size(),0) == -1)
		std::cerr << "send() faild" << std::endl;
}

void Server::_sendResponse(std::string response, int fd)
{
	if(send(fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Response send() faild" << std::endl;
}

//---------------//Close and Signal
bool Server::Signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}


//---------------// Parsing
std::vector<std::string> Server::split_recivedBuffer(std::string str)
{
	std::vector<std::string> vec;
	std::istringstream stm(str);
	std::string line;
	while(std::getline(stm, line))
	{
		size_t pos = line.find_first_of("\r\n");
		if(pos != std::string::npos)
			line = line.substr(0, pos);
		vec.push_back(line);
	}
	return vec;
}

std::vector<std::string> Server::split_cmd(std::string& cmd)
{
	std::vector<std::string> vec;
	std::istringstream stm(cmd);
	std::string token;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return vec;
}

bool Server::notregistered(int fd)
{
	if (!GetClient(fd) || GetClient(fd)->GetNickName().empty() || GetClient(fd)->GetUserName().empty() || GetClient(fd)->GetNickName() == "*"  || !GetClient(fd)->GetLogedIn())
		return false;
	return true;
}