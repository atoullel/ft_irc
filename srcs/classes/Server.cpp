#include "libraries.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Response.hpp"
#include "Channel.hpp"

// Constructor / Destructor

Server::Server(unsigned short &port, std::string &password)
	: _serverName("ircServer"), _hostName(IP), _password(password)
{
	// Initialize attributes
	bzero(&(this->_socket), sizeof(_socket));
	this->_socket.addresse.sin_family = AF_INET;
	this->_socket.addresse.sin_addr.s_addr = inet_addr(IP);
	this->_socket.addresse.sin_port = htons(port);
	this->_socket.addresse_len = sizeof(this->_socket.addresse);
	this->_fds.max = 0;
	this->_maxChannelPerUser = 10;
	// Try to create the socket
	try
	{
		_createSocket();
		std::cout << GREEN << "Success: " << RESET << "socket was created (" << this->_socket.fd << ")" << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

Server::~Server(void) throw()
{
	if (this->_clients.size() > 0)
	{
		for (size_t i = 0; i < this->_clients.size(); i++)
			delete this->_clients[i];
	}
	if (this->_channels.size() > 0)
	{
		for (size_t i = 0; i < this->_channels.size(); i++)
			delete this->_channels[i];
	}
	close(this->_socket.fd);
	std::cout << MAGENTA << "Server destroyed" << RESET << std::endl;
}

// Getters

std::string	Server::getServerName(void) const
{
	return (this->_serverName);
}

std::string Server::getHostName(void) const
{
	return (this->_hostName);
}

std::string	Server::getPassword(void) const
{
	return (this->_password);
}

std::vector<Client *> Server::getClients(void)
{
	return (_clients);
}

int Server::getMaxChannelsPerUser(void) const
{
	return (this->_maxChannelPerUser);
}

Channel	*Server::getChannelByName(std::string &name) const
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i]->getName() == name)
			return (this->_channels[i]);
	}
	return (NULL);
}

Client *Server::getClientByNickName(std::string &nickname) const
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i] == NULL)
			continue ;
		if (this->_clients[i]->getNickName() == nickname)
			return (this->_clients[i]);
	}
	return (NULL);
}

// Private Methods

void Server::_createSocket(void)
{
	int ret = 0;
	int opt = 1;

	this->_socket.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket.fd == -1)
		errorFunction("socket: ");
	this->_fds.max = this->_socket.fd;
	ret = setsockopt(this->_socket.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (ret < 0)
		errorFunction("setsockopt: ");
	ret = bind(this->_socket.fd, (const s_sockaddr *)(&this->_socket.addresse), this->_socket.addresse_len);
	if (ret < 0)
		errorFunction("bind: ");
	ret = listen(this->_socket.fd, MAX_CLIENTS);
	if (ret < 0)
		errorFunction("listen: ");
}

void Server::_addClient(void)
{
	if (FD_ISSET(this->_socket.fd, &(this->_fds.read)))
	{
		std::cout << BLUE << "Try to create new client..." << RESET << std::endl;
		Client *newClient = new Client(this->_socket);
		this->_clients.push_back(newClient);
		newClient->acceptClient();
		FD_SET(newClient->getClientFd(), &(this->_fds.read));
		if (this->_fds.max < newClient->getClientFd())
			this->_fds.max = newClient->getClientFd();
		std::cout << GREEN << "Success: " << RESET << "new client created (" << newClient->getClientFd() << ")" << std::endl;
		this->_broadcastMessage(*newClient);
	}
}

void Server::deleteClient(Client &client)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
    {
        this->_channels[i]->removeClient(&client);
    }

	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i] == NULL)
			continue ;
		if (this->_clients[i]->getClientFd() == client.getClientFd())
		{
			FD_CLR(client.getClientFd(), &(this->_fds.read));
			delete this->_clients[i];
			this->_clients[i] = NULL;
			this->_clients.erase(this->_clients.begin() + i);
			break ;
		}
	}
}

void Server::_recvMessage(Client &client)
{
	client.sendMessage();
}

void Server::_sendMessage(Client &client)
{
	std::string					messageClient;
	std::vector<std::string>	messages;
	size_t 						startPos;
	size_t 						endPos;
	Response					response;

	startPos = 0;
	messageClient = client.getMessage();
	endPos = messageClient.find("\r\n");
	while (endPos != std::string::npos)
	{
		messages.push_back(messageClient.substr(startPos, endPos - startPos));
		startPos = endPos + 2;
		endPos = messageClient.find("\r\n", startPos);
	}
	messages.push_back(messageClient.substr(startPos));
	for (size_t	i = 0; i < messages.size(); i++)
	{
		std::cout << YELLOW << "Client " << client.getClientFd() << " : `" << messages[i] << "`" << RESET << std::endl;
		client.incrementIdMessage();
		if (client.getIdMessage() == 2 && messages[i].substr(0, 4) != "PASS")
		{
			client.sendErrorMessages(client, "No password given: connection refused");
			this->deleteClient(client);
			break;
		}
		else if (messages[i].substr(0, 4) == "PASS" && messages[i].substr(5) != this->_password)
		{
			client.sendErrorMessages(client, "Wrong password: connection refused");
			this->deleteClient(client);
			break;
		}
		response.sendResponse(client, (*this), messages[i]);
	}
}

void Server::_broadcastMessage(Client &client)
{
	int 		ret = 0;
	std::string	message = "New client connected\r\n";

	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i] == NULL)
			continue ;
		if (this->_clients[i]->getClientFd() == client.getClientFd())
			continue ;
		ret = send(this->_clients[i]->getClientFd(), message.c_str(), message.size(), 0);
		if (ret < 0)
			errorFunction("send: ");
	}
	ret = send(client.getClientFd(), message.c_str(), message.size(), 0);
	if (ret < 0)
		errorFunction("send: ");
}

// Methods

void Server::start(void)
{
	int ret = 0;
	int	maxFd = 0;

	FD_ZERO(&(this->_fds.read));
	FD_SET(this->_socket.fd, &(this->_fds.read));

	maxFd = this->_socket.fd;
	std::cout << GREY;
	std::cout << "nb client: " << this->_clients.size() << std::endl;
	std::cout << RESET;
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i] == NULL)
			continue ;
		FD_SET(this->_clients[i]->getClientFd(), &(this->_fds.read));
		if (this->_clients[i]->getClientFd() > maxFd)
			maxFd = this->_clients[i]->getClientFd();
	}
	this->_fds.max = maxFd;


	ret = select(this->_fds.max + 1, &(this->_fds.read), NULL, NULL, NULL);
	if (ret < 0)
		errorFunction("select: ");

	_addClient();

	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i] == NULL)
			continue ;
		if (FD_ISSET(this->_clients[i]->getClientFd(), &(this->_fds.read)))
		{
			this->_recvMessage(*(this->_clients[i]));
			if (this->_clients[i]->getMessage().empty())
				continue ;
			this->_sendMessage(*(this->_clients[i]));
		}
	}
}

bool Server::isRegistered(std::string &username) const
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i] == NULL)
			continue ;
		if (this->_clients[i]->getUserName() == username)
			return (true);
	}
	return (false);
}

void Server::addChannel(Channel *channel)
{
	this->_channels.push_back(channel);
}

std::vector<Channel *> Server::getAllChannels(void) const
{
	for (size_t i = 0; i < this->_channels.size(); i++)
		std::cout << GREEN << "Channel " << this->_channels[i]->getName() << RESET << std::endl;
	return (this->_channels);
}

void Server::sendJoinMessage(Client &client, Server &server, Channel &channel)
{
	int ret = 0;
	ret = send(client.getClientFd(), "\r\n", 2, 0);
	std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + server.getHostName() + " JOIN " + channel.getName() + "\r\n";
    ret = send(client.getClientFd(), message.c_str(), message.size(), 0);
	if (ret < 0)
		errorFunction("send: ");
	else
		std::cout << GREEN << "Success: " << RESET << "JOIN message sent to client " << client.getClientFd() << std::endl;
}

void Server::sendUsersList(Client &client, Server &server, Channel &channel)
{
	std::vector<Client *> users = channel.getClients();
	int ret = 0;
	ret = send(client.getClientFd(), "\r\n", 2, 0);
	for (std::vector<Client *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		Client *user = *it;
		std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + server.getHostName() + " RPL_NAMREPLY 353 " + channel.getName() + " :" + user->getNickName() + "\r\n";
		ret = send(client.getClientFd(), message.c_str(), message.size(), 0);
		if (ret < 0)
			errorFunction("send: ");
		else
			std::cout << GREEN << "Success: " << RESET << "RPL_NAMREPLY message sent to client " << client.getClientFd() << std::endl;
	}
	std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + server.getHostName() + " " + "366 " + client.getNickName() + " " + channel.getName() + " :End of /NAMES list\r\n";
	ret = send(client.getClientFd(), message.c_str(), message.size(), 0);
	if (ret < 0)
		errorFunction("send: ");
	else
		std::cout << GREEN << "Success: " << RESET << "RPL_ENDOFNAMES message sent to client " << client.getClientFd() << std::endl;
}

// todo
void Server::sendTopicMessage(Client &client, Server &server, Channel &channel)
{
	if (channel.getTopic().empty())
		return ;
	int ret = 0;
	ret = send(client.getClientFd(), "\r\n", 2, 0);
	std::string message = ":" + client.getNickName() + "!" + client.getUserName() + "@" + server.getHostName() + " RPL_TOPIC 332 " + channel.getName() + " :" + channel.getTopic() + "\r\n";
	ret = send(client.getClientFd(), message.c_str(), message.size(), 0);
	if (ret < 0)
		errorFunction("send: ");
	else
		std::cout << GREEN << "Success: " << RESET << "RPL_TOPIC message sent to client " << client.getClientFd() << std::endl;
}

void Server::setMaxChannelsPerUser(int max)
{
	this->_maxChannelPerUser = max;
}

