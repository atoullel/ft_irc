#include "libraries.hpp"
#include "Client.hpp"
#include "Channel.hpp"

Client::Client(void)
	: _clientFd(0), _idMessage(0), _messageRecv(""), _message(""), _username(""), _realname(""), _hostname("localhost"), _servername(""), _isOperator(false), _isAuthentified(false)
{
	bzero(&_socketServer, sizeof(_socketServer));
}

Client::Client(t_socket &socketServer)
	: _clientFd(0), _idMessage(0), _messageRecv(""), _message(""), _username(""), _realname(""), _hostname("localhost"), _servername(""), _isOperator(false), _isAuthentified(false)
{
	this->_socketServer = socketServer;
}

Client::~Client(void) throw()
{
	if (!_joinedChannels.empty())
	{
		for (size_t i = 0; i < _joinedChannels.size(); i++)
		{
			if (_joinedChannels[i]->isOperator(this))
				_joinedChannels[i]->removeOperator(this);
			_joinedChannels[i]->removeClient(this);
		}
	}
	std::cout << MAGENTA << "Client " << _clientFd << " disconnected" << RESET << std::endl;
	close(_clientFd);
}

// GETTERS
int			Client::getClientFd(void) const
{
	return (this->_clientFd);
}

size_t		Client::getIdMessage(void) const
{
	return (this->_idMessage);
}

std::string	Client::getMessage(void) const
{
	return (this->_message);
}

std::string Client::getNickName(void) const
{
	return(_nickname);
}

std::string	Client::getUserName(void) const
{
	return (this->_username);
}

std::string	Client::getRealName(void) const
{
	return (this->_realname);
}

std::string	Client::getHostName(void) const
{
	return (this->_hostname);
}

std::string	Client::getServerName(void) const
{
	return (this->_servername);
}

std::vector<Channel *> Client::getJoinedChannels(void) const
{
	return (this->_joinedChannels);
}

bool		Client::isOperator(void) const
{
	return (this->_isOperator);
}

bool		Client::isAuthentified(void) const
{
	return (this->_isAuthentified);
}

std::vector<std::string> Client::getInvitedChannels(void) const
{
	return (this->_invitedChannels);
}

// SETTERS
void		Client::setSocketServer(t_socket &socketServer)
{
	this->_socketServer = socketServer;
}

void		Client::incrementIdMessage(void)
{
	this->_idMessage++;
}

void		Client::setNickName(std::string &nick)
{
	this->_nickname = nick;
}

void		Client::setUserName(std::string &username)
{
	this->_username = username;
}

void		Client::setRealName(std::string &realname)
{
	this->_realname = realname;
}

void		Client::setHostName(std::string &hostname)
{
	this->_hostname = hostname;
}

void		Client::setServerName(std::string &servername)
{
	this->_servername = servername;
}

void		Client::setOperator(bool &op)
{
	this->_isOperator = op;
}

void		Client::setAuthentified(void)
{
	this->_isAuthentified = true;
}

void		Client::setInvitedChannel(std::string &channel)
{
	if (find(this->_invitedChannels.begin(), this->_invitedChannels.end(), channel) != this->_invitedChannels.end())
		return ;
	this->_invitedChannels.push_back(channel);
}

// METHODS
void		Client::acceptClient(void)
{
	std::ostringstream	oss;
	std::string			username;

	if (_socketServer.fd == 0)
	{
		std::cerr << RED << "Server socket not set" << RESET << std::endl;
		return;
	}
	this->_clientFd = accept(_socketServer.fd, (s_sockaddr *)&_socketServer.addresse, &_socketServer.addresse_len);
	if (this->_clientFd < 0)
		errorFunction("accept: ");
	oss << "Client " << this->_clientFd;
	username = oss.str();
	this->setUserName(username);
}

void		Client::sendMessage(void)
{
	int			ret;
	char		buffer[BUFFER_SIZE];

	bzero(buffer, BUFFER_SIZE);
	ret = recv(this->getClientFd(), buffer, BUFFER_SIZE, 0);
	if (ret < 0)
	{
		errorFunction("recv: ");
	}
	else if (ret == 0)
	{
		this->_messageRecv.clear();
		this->_message = "QUIT :leaving";
	}
	else if (strstr(buffer, "\r\n") == NULL)
	{
		this->_messageRecv.append(buffer);
	}
	else
	{
		this->_messageRecv.append(buffer);
		this->_message = this->_messageRecv;
		this->_message = this->_message.substr(0, this->_message.size() - 2);
		this->_messageRecv.clear();
	}
}

void		Client::addJoinedChannel(Channel *channel)
{
	if (find(this->_joinedChannels.begin(), this->_joinedChannels.end(), channel) != this->_joinedChannels.end())
		return ;
	this->_joinedChannels.push_back(channel);
}

void		Client::removeJoinedChannel(Channel *channel)
{
	std::vector<Channel *>::iterator it;

	it = find(this->_joinedChannels.begin(), this->_joinedChannels.end(), channel);
	if (it != this->_joinedChannels.end())
		this->_joinedChannels.erase(it);
}

bool		Client::isInvitedChannel(std::string const &channel)
{
	if (find(this->_invitedChannels.begin(), this->_invitedChannels.end(), channel) != this->_invitedChannels.end())
		return (true);
	return (false);
}
