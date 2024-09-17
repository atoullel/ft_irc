#include "Response.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// Constructor / Destructor

Response::Response(void)
{
	this->_response["CAP"] = &Response::_cap;
	this->_response["PASS"] = &Response::_pass;
	this->_response["JOIN"] = &Response::_join;
	this->_response["NICK"] = &Response::_nick;
	this->_response["PING"] = &Response::_ping;
	this->_response["PONG"] = &Response::_pong;
	this->_response["MODE"] = &Response::_mode;
	this->_response["USER"] = &Response::_user;
	this->_response["PRIVMSG"] = &Response::_privmsg;
	this->_response["QUIT"] = &Response::_quit;
	this->_response["KICK"] = &Response::_kick;
	this->_response["INVITE"] = &Response::_invite;
	this->_response["TOPIC"] = &Response::_topic;
}

Response::~Response(void) throw() {}

// parsing Message

std::vector<std::string> splitString(const std::string& str)
{
	std::istringstream iss(str);
	return std::vector<std::string>((std::istream_iterator<std::string>(iss)),
										std::istream_iterator<std::string>());
}

t_request	Response::_parseMessage(std::string &message)
{
	t_request	request;

	request.raw = message;
	if (message[0] == ':')
	{
		request.prefix = message.substr(1, message.find(' ') - 1);
		message = message.substr(message.find(' ') + 1, message.size() - message.find(' ') - 1);
	}
	request.cmd = message.substr(0, message.find(' '));
	request.args = message.substr(message.find(' ') + 1, message.size() - message.find(' ') - 1);
	request.params = splitString(request.args);
	return (request);
}

// Methods

void	Response::sendResponse(Client &client, Server &server, std::string &message)
{
	int			ret = 0;
	t_request	req;

	req = this->_parseMessage(message);
	if (_response.find(req.cmd) != _response.end())
	{
		ret = (this->*_response[req.cmd])(client, server, req);
		if (ret != 0)
			errorRequest(ret, client, server, req);
	}
	else
	{
		errorRequest(ERR_UNKNOWNCOMMAND, client, server, req);
	}
}

void Response::sendAnyMsg(Client &client, std::string &message)
{
	int	ret = 0;

	ret = send(client.getClientFd(), "\r\n", 2, 0);
	if (ret < 0)
		errorFunction("send: ");
	ret = send(client.getClientFd(), message.c_str(), message.size(), 0);
	if (ret < 0)
		errorFunction("send: ");
}

// Private Methods

int	Response::_cap(Client &client, Server &server, t_request &req)
{
	std::string	message_ls = "CAP * LS :multi-prefix sasl\r\n";
	std::string	message_req = "CAP * ACK multi-prefix\r\n";
	int			ret = 0;

	(void)server;
	if (req.args == "LS")
	{
		ret = send(client.getClientFd(), message_ls.c_str(), message_ls.size(), 0);
		if (ret < 0)
			errorFunction("send: ");
		std::cout << CYAN << "Server: `" << message_ls << "`" << RESET << std::endl;
	}
	else if (req.args == "REQ :multi-prefix")
	{
		ret = send(client.getClientFd(), message_req.c_str(), message_req.size(), 0);
		if (ret < 0)
			errorFunction("send: ");
		std::cout << CYAN << "Server: `" << message_req << "`" << RESET << std::endl;
	}
	else if (req.args == "END")
	{
		std::ostringstream oss;
		oss << "001 " << client.getNickName() << " :Welcome to the Internet Relay Network " << client.getNickName() << "\r\n";
		std::string message_end = oss.str();
		ret = send(client.getClientFd(), message_end.c_str(), message_end.size(), 0);
		if (ret < 0)
			errorFunction("send: ");
		std::cout << CYAN << "Server: `" << message_end << "`" << RESET << std::endl;
	}
	return (EXIT_SUCCESS);
}

int	Response::_pass(Client &client, Server &server, t_request &req)
{
	std::string	password = server.getPassword();

	if (req.args.empty())
		return (ERR_NEEDMOREPARAMS);
	if (req.args != password)
		return(ERR_PASSWDMISMATCH);
	client.setAuthentified();
	return (EXIT_SUCCESS);
}

int parseJoin(std::string &channelName)
{
	if (channelName.empty())
		return (ERR_NEEDMOREPARAMS);
	if (channelName[0] != '#')
		return (ERR_NOSUCHCHANNEL);
	if (channelName.find(' ') != std::string::npos || channelName.find('\x07') != std::string::npos || channelName.find(',') != std::string::npos)
		return (ERR_UNKNOWNERROR); // TODO find the good error
	if (channelName.length() > CHANNELLEN)
		channelName = channelName.substr(0, CHANNELLEN);
	return (EXIT_SUCCESS);
}

std::vector<std::string> split(const std::string &s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

int Response::_isValidJoinRequest(t_request &req, Client &client, Server &server, std::vector<std::string> &channels)
{
	int totalChannels = static_cast<int>(channels.size()) + static_cast<int>(client.getJoinedChannels().size());
	int channelsSize = static_cast<int>(channels.size());
	std::string channelName = getChannelName(req.params);

	std::cout << "channelName: " << channelName << std::endl;
	if (req.params.empty())
		return (ERR_NEEDMOREPARAMS);
	if (channelsSize > server.getMaxChannelsPerUser() || totalChannels > server.getMaxChannelsPerUser())
		return (ERR_TOOMANYCHANNELS);
	for (size_t i = 0; server.getClients().size() && i < channels.size(); i++)
	{
		if (channelName.substr(1) == server.getClients()[i]->getNickName())
		{
			sendErrorMessages(client, "You can't join a channel with a nickname of another client");
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

int Response::_processChannel(Client &client, Server &server, std::string channelName, std::string key)
{
	Channel *channel;

	channel = server.getChannelByName(channelName);
	if (channel == NULL)
	{
		channel = new Channel(channelName);
		server.addChannel(channel);
	}
	if (channel->isInviteOnly() && !client.isInvitedChannel(channel->getName()))
		return (ERR_INVITEONLYCHAN);
	if (channel->getKey() != key)
		return (ERR_BADCHANNELKEY);
	if (channel->getMaxClients() != -1 && channel->getClients().size() >= static_cast<size_t>(channel->getMaxClients()))
		return (ERR_CHANNELISFULL);
	channel->addClient(&client);
	client.addJoinedChannel(channel);
	server.sendJoinMessage(client, server, *channel);
	server.sendTopicMessage(client, server, *channel);
	server.sendUsersList(client, server, *channel);

	return (EXIT_SUCCESS);
}

int Response::_join(Client &client, Server &server, t_request &req)
{
	std::vector<std::string>	channels;
	std::vector<std::string>	keys;
	std::string					channelName;
	int							ret;
	int							result;

	channels = split(req.params[0], ',');
	keys = req.params.size() > 1 ? split(req.params[1], ',') : std::vector<std::string>();
	ret = _isValidJoinRequest(req, client, server, channels);
	if (ret != EXIT_SUCCESS)
		return (ret);
	for (size_t i = 0; i < channels.size(); i++)
	{
		std::string key = i < keys.size() ? keys[i] : "";
		result = _processChannel(client, server, channels[i], key);
		if (result != EXIT_SUCCESS)
			return (result);
	}
	return (EXIT_SUCCESS);
}

// todo : probleme avec nick le changement ne se fait pas correctement cote client voir pourquoi
int	Response::_nick(Client &client, Server &server, t_request &req)
{
	if (req.args.empty())
		return (ERR_NONICKNAMEGIVEN);

	std::string newNick = req.args;
	if (newNick[0] == '#' || newNick[0] == ':' || newNick[0] == ' ' || !std::isalpha(newNick[0]))
		return (ERR_ERRONEUSNICKNAME);

	for(size_t i = 0; i < server.getClients().size(); i++)
	{
		if (server.getClients()[i]->getNickName() == newNick)
			return (ERR_NICKNAMEINUSE);
	}

	std::string oldNick = client.getNickName();
	client.setNickName(newNick);

	std::string nickChangeMsg = ":" + oldNick + "!" + client.getUserName() + "@" + client.getHostName() + " NICK " + newNick + "\r\n";
	for(size_t i = 0; i < server.getClients().size(); i++)
	{
		if (server.getClients()[i]->getNickName() != oldNick)
			sendAnyMsg(*server.getClients()[i], nickChangeMsg);
	}

	std::cout << BLUE << "Client " << client.getClientFd() << ": `" << client.getNickName() << "`" << RESET << std::endl;
	return (EXIT_SUCCESS);
}

int Response::_user(Client &client, Server &server, t_request &req)
{
	t_user_info		user_info;
	int				parseResult;

	parseResult = parseUserCmd(&user_info, req, client, server);
	if (parseResult != EXIT_SUCCESS)
    	return (parseResult);

	client.setUserName(user_info.username);
	client.setRealName(user_info.realname);
	client.setHostName(user_info.hostname);
	client.setServerName(user_info.servername);

	std::cout << BLUE << "Client " << client.getClientFd() <<
	": Username : `" << client.getUserName() <<
	"` HostName :`" << server.getHostName() <<
	"` ServerName :`" << server.getServerName() <<
	"` RealName :`" << client.getRealName() <<
	"`" << RESET << std::endl;

	return (EXIT_SUCCESS);
}

int Response::_ping(Client &client, Server &server, t_request &req)
{
	std::ostringstream	oss;
	int 				ret = 0;

	if (req.args.empty())
		return (ERR_NOORIGIN);
	oss << "PONG " << server.getServerName() << " " << req.args;
	std::string message = oss.str();
	ret = send(client.getClientFd(), message.c_str(), message.size(), 0);
	if (ret < 0)
		errorFunction("send: ");
	std::cout << CYAN << "Server: `" << message << "`" << RESET << std::endl;
	return (EXIT_SUCCESS);
}

int	Response::_pong(Client &client, Server &server, t_request &req)
{
	(void)client;
	(void)server;
	(void)req;
	return (EXIT_SUCCESS);
}

int Response::_mode(Client &client, Server &server, t_request &req)
{
	std::string chanName = req.params[0];
	std::string param = req.params[1];
	bool state;

	if (param.empty())
		return (EXIT_SUCCESS);
	if (server.getClientByNickName(chanName) != NULL)
		return (EXIT_SUCCESS);
	if (server.getChannelByName(chanName) == NULL)
		return (ERR_NOSUCHCHANNEL);
	if (server.getChannelByName(chanName)->getOperatorByName(client.getNickName()) == NULL)
		return (ERR_CHANOPRIVSNEEDED);
	if (param == "+i" || param == "-i")
	{
		if (param[0] == '-')
		{
			state = false;
			server.getChannelByName(chanName)->setInviteOnlyMode(state);
		}
		else
		{
			state = true;
			server.getChannelByName(chanName)->setInviteOnlyMode(state);
		}
	}
	else if (param == "+t" || param == "-t")
	{
		if (param[0] == '-')
		{
			state = false;
			server.getChannelByName(chanName)->setProtectedTopicMode(state);
		}
		else
		{
			state = true;
			server.getChannelByName(chanName)->setProtectedTopicMode(state);
		}
	}
	else if (param == "+k" || param == "-k")
	{
		if (req.params.size() != 3)
			return (EXIT_SUCCESS);
		if (param[0] == '-')
		{
			state = false;
			server.getChannelByName(chanName)->setKeyProtectedMode(state, req.params[2]);
		}
		else
		{
			state = true;
			server.getChannelByName(chanName)->setKeyProtectedMode(state, req.params[2]);
		}
	}
	else if (param == "+o" || param == "-o")
	{
		if (req.params.size() != 3)
			return (EXIT_SUCCESS);
		int i = server.getChannelByName(chanName)->getOperatorIdByName(req.params[2]);
		if (param[0] == '-')
		{
			if (i != -1)
			{
				server.getChannelByName(chanName)->removeOperator(server.getClientByNickName(req.params[2]));
				std::cout << GREEN << "Removed " << req.params[2] << " from " << chanName << "'s operators"<< RESET << std::endl;
			}
		}
		else
		{
			if (server.getClientByNickName(req.params[2]) != NULL)
			{
				server.getChannelByName(chanName)->setOperator(server.getClientByNickName(req.params[2]));
				std::cout << GREEN << "Added " << req.params[2] << " in " << chanName << "'s operators list"<< RESET << std::endl;
			}
		}
	}
	else if (param == "+l" || param == "-l")
	{
		int i = -1;
		if (param[0] == '-')
			server.getChannelByName(chanName)->setMaxClients(-1);
		else
		{
			if (req.params.size() != 3)
				return (EXIT_SUCCESS);
			std::istringstream(req.params[2]) >> i;
			server.getChannelByName(chanName)->setMaxClients(i);
		}
	}
	return (EXIT_SUCCESS);
}

void	Response::sendPrivMessage(Client &client, Server &server, std::string &message, std::string &recipientNickname)
{
	std::string msg = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + recipientNickname + " :" + message.substr(1) + "\r\n";
	for(size_t i = 0; i < server.getClients().size(); i++)
	{
		if (server.getClients()[i]->getNickName() != client.getNickName())
		{
			sendAnyMsg(*server.getClients()[i], msg);
		}
	}
}

int Response::_privmsg(Client &client, Server &server, t_request &req)
{
	std::string				fullMessage;
	std::string				recipientNickname;
	std::string				message;
	Client					*recipientClient;
	std::string				privmsg;
	Channel					*channel;
	std::vector<Client *>	users;

	if (req.args.empty())
		return (ERR_NORECIPIENT);
	recipientNickname = req.args.substr(0, req.args.find(' '));
	if (recipientNickname.empty())
		return (ERR_NORECIPIENT);
	message = req.args.substr(req.args.find(' ') + 1);
	if (recipientNickname[0] != '#')
	{
		recipientClient = server.getClientByNickName(recipientNickname);
		if (recipientClient == NULL)
			return (ERR_NOSUCHNICK);
		sendPrivMessage(client, server, message, recipientNickname);
	}
	else
	{
		channel = server.getChannelByName(recipientNickname);
		if (channel == NULL)
			return (ERR_NOSUCHCHANNEL);
		if (channel->getClientByNickName(client.getNickName()) == NULL)
			return (ERR_CANNOTSENDTOCHAN);
		users = channel->getClients();
		for (std::vector<Client *>::iterator it = users.begin(); it != users.end(); ++it)
		{
			Client *user = *it;
			if (user->getNickName() == client.getNickName())
				continue;
			sendPrivMessage(client, server, message, recipientNickname);
		}
	}

	return (EXIT_SUCCESS);
}

int Response::_quit(Client &client, Server &server, t_request &req)
{
	(void)req;
	server.deleteClient(client);
	return (EXIT_SUCCESS);
}

int Response::_kick(Client &client, Server &server, t_request &req)
{
	std::string		channelName;
	std::string		userToKick;
	std::string		kickReason;
	Channel			*channel;
	Client			*kickedClient;
	std::string		kickMsg;

	if (req.params.size() < 2)
		return (ERR_NEEDMOREPARAMS);
	channelName = req.params[0];
	userToKick = req.params[1];
	kickReason = req.params.size() > 2 ? req.params[2] : "No reason specified";
	if (req.params.size() > 3)
	{
		for (size_t i = 3; i < req.params.size(); i++)
			kickReason += " " + req.params[i];
	}
	channel = server.getChannelByName(channelName);
	if (channel == NULL)
		return (ERR_NOSUCHCHANNEL);
	if (channel->getClientByNickName(client.getNickName()) == NULL)
		return (ERR_NOTONCHANNEL);
	if (server.getChannelByName(channelName)->getOperatorByName(client.getNickName()) == NULL)
		return (ERR_CHANOPRIVSNEEDED);
	kickedClient = server.getClientByNickName(userToKick);
	if (kickedClient == NULL || channel->getClientByNickName(userToKick) == NULL)
		return (ERR_USERNOTINCHANNEL);
	if (kickedClient->getNickName() == client.getNickName())
	{
		sendErrorMessages(client, "You can't kick yourself");
		return (EXIT_SUCCESS);
	}
	if (channel->isOperator(kickedClient))
	{
		sendErrorMessages(client, "You can't kick an operator");
		return (EXIT_SUCCESS);
	}

	channel->removeClient(kickedClient);
	client.removeJoinedChannel(channel);

	kickMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " KICK " + channelName + " " + userToKick + " :" + kickReason + "\r\n";
	sendAnyMsg(*kickedClient, kickMsg);
	for(size_t i = 0; i < channel->getClients().size(); i++)
		sendAnyMsg(*channel->getClients()[i], kickMsg);
	return (EXIT_SUCCESS);
}

int Response::_invite(Client &client, Server &server, t_request &req)
{
	std::vector<std::string>	invitedChannels;
	std::string					inviteListMsg;
	std::string					endOfInviteListMsg;
	std::string					nickname;
	std::string					channelName;
	Channel						*channel;
	Client						*invitedClient;
	std::string					inviteMsg;

	if (req.params.size() < 2)
		return (ERR_NEEDMOREPARAMS);

	nickname = req.params[0];
	channelName = req.params[1];
	channel = server.getChannelByName(channelName);
	if (channel == NULL)
		return (ERR_NOSUCHCHANNEL);
	if (channel->getClientByNickName(client.getNickName()) == NULL)
		return (ERR_NOTONCHANNEL);
	invitedClient = server.getClientByNickName(nickname);
	if (invitedClient == NULL)
		return (ERR_NOSUCHNICK);
	if (channel->getClientByNickName(nickname) != NULL)
		return (ERR_USERONCHANNEL);

	inviteMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " INVITE " + nickname + " :" + channelName + "\r\n";
	sendAnyMsg(*invitedClient, inviteMsg);
	invitedClient->setInvitedChannel(channelName);
	return (EXIT_SUCCESS);
}

int Response::_topic(Client &client, Server &server, t_request &req)
{
	std::string		channelName;
	Channel			*channel;
	std::string		topic;
	std::string		topicMsg;

	if (req.params.size() < 1)
		return (ERR_NEEDMOREPARAMS);
	channelName = req.params[0];
	if (channelName[0] != '#')
		return (ERR_NOSUCHCHANNEL);
	channel = server.getChannelByName(channelName);
	if (channel == NULL)
		return (ERR_NOSUCHCHANNEL);
	if (channel->getClientByNickName(client.getNickName()) == NULL)
		return (ERR_NOTONCHANNEL);
	if (channel->isProtectedTopic() && channel->isOperator(&client) == false)
		return (ERR_CHANOPRIVSNEEDED);
	if (req.params.size() > 1)
	{
		topic = req.params[1];
		for (size_t i = 2; i < req.params.size(); i++)
			topic += " " + req.params[i];
		channel->setTopic(topic.substr(1));
	}
	else
		channel->setTopic("");

	topicMsg = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " TOPIC " + channelName + " :" + channel->getTopic() + "\r\n";
	for (size_t i = 0; i < channel->getClients().size(); i++)
		sendAnyMsg(*channel->getClients()[i], topicMsg);

	return (EXIT_SUCCESS);
}
