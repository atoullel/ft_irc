#include "Channel.hpp"
#include "Client.hpp"
#include "libraries.hpp"

Channel::Channel(std::string name)
	: _name(name), _max_clients(-1), _protected_topic_mode(true), _invite_only_mode(false), _key_protected_mode(false)
{
	std::cout << GREEN << "Channel " << _name << " created" << RESET << std::endl;
}

Channel::~Channel(void) throw()
{
	std::cout << MAGENTA << "Channel " << _name << " destroyed" << RESET << std::endl;
}

// Getters

std::string		Channel::getName(void) const
{
	return (_name);
}

std::vector<Client *>	Channel::getClients(void) const
{
	return (_clients);
}

Client			*Channel::getClientByNickName(std::string const &nickname) const
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getNickName() == nickname)
			return (_clients[i]);
	}
	return (NULL);
}

std::vector<Client *>			const&Channel::getOperators(void) const
{
	return (_operators);
}

std::string		Channel::getTopic(void) const
{
	return (_topic);
}

std::string		Channel::getKey(void) const
{
	return (_key);
}

int	Channel::getOperatorIdByName(std::string name) const
{
	for (size_t i = 0; i < this->_operators.size(); i++)
	{
		if (this->_operators[i]->getNickName() == name)
			return (i);
	}
	return (-1);
}

Client	*Channel::getOperatorByName(std::string name) const
{
	for (size_t i = 0; i < this->_operators.size(); i++)
	{
		if (this->_operators[i]->getNickName() == name)
			return (this->_operators[i]);
	}
	return (NULL);
}

 int	Channel::getMaxClients(void)
 {
	return (_max_clients);
 }

// Setters

void			Channel::setOperator(Client *client)
{
	_operators.push_back(client);
	std ::cout << GREEN << "Client " << client->getNickName() << " is now operator of channel " << _name << RESET << std::endl;
}

void			Channel::setTopic(std::string topic)
{
	_topic = topic;
	std::cout << GREEN << "Topic of channel " << _name << " is now " << _topic << RESET << std::endl;
}

void		Channel::setKey(std::string key)
{
	_key = key;
	std::cout << GREEN << "Key of channel " << _name << " is now " << _key << RESET << std::endl;
}

void			Channel::setInviteOnlyMode(bool state)
{
	_invite_only_mode = state;
	std::cout << GREEN << "Invite mode status = " << _invite_only_mode << " for Channel " << _name << RESET << std::endl;
}

void			Channel::setProtectedTopicMode(bool state)
{
	_protected_topic_mode = state;
	std::cout << GREEN << "Protected Topic mode status = " << _protected_topic_mode << " for Channel " << _name << RESET << std::endl;
}

void			Channel::setKeyProtectedMode(bool state, std::string key)
{
	_key_protected_mode = state;
	_key = key;
	std::cout << GREEN << "key protected status = " << _key_protected_mode << " for Channel " << _name << " With key:" << _key << RESET << std::endl;
}

void 			Channel::setMaxClients(int max)
{
	_max_clients = max;
	std::cout << GREEN << "Maximum clients limit set to: " << _max_clients << RESET << std::endl;
}
// Methods

void			Channel::addClient(Client *client)
{
	std::cout << GREEN << "Client " << client->getNickName() << " joined channel " << _name << RESET << std::endl;
	_clients.push_back(client);
	if (_operators.size() == 0)
		setOperator(client);
}

void			Channel::removeOperator(Client *client)
{
	std::vector<Client *>::iterator it;

	it = std::find(_operators.begin(), _operators.end(), client);
	if (it != _operators.end())
		_operators.erase(it);
	std::cout << GREEN << "Client " << client->getNickName() << " is no longer operator of channel " << _name << RESET << std::endl;
}

void 			Channel::removeClient(Client *client)
{
	std::vector<Client *>::iterator it;

	it = std::find(_clients.begin(), _clients.end(), client);
	if (it != _clients.end())
		_clients.erase(it);
	std::cout << GREEN << "Client " << client->getNickName() << " left channel " << _name << RESET << std::endl;
}

bool			Channel::isProtectedTopic()
{
	if (_protected_topic_mode)
		return (true);
	return (false);
}

bool			Channel::isInviteOnly()
{
	if (_invite_only_mode)
		return (true);
	return (false);
}

bool			Channel::isKeyProtected()
{
	if (_key_protected_mode)
		return (true);
	return (false);
}

bool			Channel::isOperator(Client *client)
{
	if (std::find(_operators.begin(), _operators.end(), client) != _operators.end())
	{
		std::cout << GREEN << "Client " << client->getNickName() << " is operator of channel " << _name << RESET << std::endl;
		return (true);
	}
	else
		std::cout << GREEN << "Client " << client->getNickName() << " is not operator of channel " << _name << RESET << std::endl;
	return (false);
}
