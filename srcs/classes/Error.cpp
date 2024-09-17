#include "Error.hpp"
#include "Server.hpp"
#include "Client.hpp"

// Constructor / Destructor

Error::Error(const char *str) throw()
	: _errorMessage(str)
{
	_error[ERR_UNKNOWNERROR] = &Error::_errUnknownError;
	_error[ERR_NOSUCHNICK] = &Error::_errNoSuchNick;
	_error[ERR_NOSUCHSERVER] = &Error::_errNoSuchServer;
	_error[ERR_NOSUCHCHANNEL] = &Error::_errNoSuchChannel;
	_error[ERR_CANNOTSENDTOCHAN] = &Error::_errCannotSendToChan;
	_error[ERR_TOOMANYCHANNELS] = &Error::_errTooManyChannels;
	_error[ERR_WASNOSUCHNICK] = &Error::_errWasNoSuchNick;
	_error[ERR_NOORIGIN] = &Error::_errNoOrigin;
	_error[ERR_NORECIPIENT] = &Error::_errNoRecipient;
	_error[ERR_NOTEXTTOSEND] = &Error::_errNoTextToSend;
	_error[ERR_INPUTTOOLONG] = &Error::_errInputTooLong;
	_error[ERR_UNKNOWNCOMMAND] = &Error::_errUnknownCommand;
	_error[ERR_NOMOTD] = &Error::_errNoMotd;
	_error[ERR_NONICKNAMEGIVEN] = &Error::_errNoNicknameGiven;
	_error[ERR_ERRONEUSNICKNAME] = &Error::_errErroneusNickname;
	_error[ERR_NICKNAMEINUSE] = &Error::_errNicknameInUse;
	_error[ERR_NICKCOLLISION] = &Error::_errNickCollision;
	_error[ERR_USERNOTINCHANNEL] = &Error::_errUserNotInChannel;
	_error[ERR_NOTONCHANNEL] = &Error::_errNotOnChannel;
	_error[ERR_USERONCHANNEL] = &Error::_errUserOnChannel;
	_error[ERR_NOTREGISTERED] = &Error::_errNotRegistered;
	_error[ERR_NEEDMOREPARAMS] = &Error::_errNeedMoreParams;
	_error[ERR_ALREADYREGISTERED] = &Error::_errAlreadyRegistered;
	_error[ERR_PASSWDMISMATCH] = &Error::_errPasswdMismatch;
	_error[ERR_YOUREBANNEDCREEP] = &Error::_errYoureBannedCreep;
	_error[ERR_CHANNELISFULL] = &Error::_errChannelIsFull;
	_error[ERR_UNKNOWNMODE] = &Error::_errUnknownMode;
	_error[ERR_INVITEONLYCHAN] = &Error::_errInviteOnlyChan;
	_error[ERR_BANNEDFROMCHAN] = &Error::_errBannedFromChan;
	_error[ERR_BADCHANNELKEY] = &Error::_errBadChannelKey;
	_error[ERR_BADCHANMASK] = &Error::_errBadChanMask;
	_error[ERR_NOPRIVILEGES] = &Error::_errNoPrivileges;
	_error[ERR_CHANOPRIVSNEEDED] = &Error::_errChanOpPrivsNeeded;
	_error[ERR_CANTKILLSERVER] = &Error::_errCantKillServer;
	_error[ERR_NOOPERHOST] = &Error::_errNoOperHost;
	_error[ERR_UMODEUNKNOWNFLAG] = &Error::_errUModeUnknownFlag;
	_error[ERR_USERSDONTMATCH] = &Error::_errUsersDontMatch;
	_error[ERR_HELPNOTFOUND] = &Error::_errHelpNotFound;
	_error[ERR_INVALIDKEY] = &Error::_errInvalidKey;
	_error[ERR_INVALIDMODEPARAM] = &Error::_errInvalidModeParam;
	_error[ERR_NOPRIVS] = &Error::_errNoPrivs;
	_error[ERR_NICKLOCKED] = &Error::_errNickLocked;
}


Error::~Error(void) throw()
{
	_errorMessage.clear();
}

void	Error::errorFunction(const char *str)
{
	_errorMessage = RED;
	_errorMessage += "Failure: ";
	_errorMessage += RESET;
	_errorMessage += str;
	_errorMessage += std::strerror(errno);
	throw *this;
}

void	Error::sendErrorMessages(Client &client, const char *message)
{
	std::string	errorMessage;
	int			ret;

	errorMessage = "\r\n";
	errorMessage += "ERROR : ";
	errorMessage += message;
	errorMessage += "\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << RED << "Error: " << RESET << message << std::endl;
}

void	Error::errorRequest(int error, Client &client, Server &server, t_request &message)
{
	if (_error.find(error) != _error.end())
		(this->*_error[error])(client, server, message);
	else
		(this->*_error[ERR_UNKNOWNERROR])(client, server, message);
}

const char* Error::what() const throw()
{
	return (_errorMessage.c_str());
}

void Error::_errUnknownError(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int	ret;

	std::cout << RED << "Error: " << RESET << "unknown error" << std::endl;

	errorMessage = ":" + server.getServerName() + " 400 " + client.getNickName() + " " + request.cmd + " :" + request.args + "\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoSuchNick(Client &client, Server &server, t_request &request)
{
	std::string	errorMessage;
	int	ret;

	std::cout << RED << "Error: " << RESET << "No such nick" << std::endl;

	errorMessage = ":" + server.getServerName() + " 401 " + client.getNickName() + " " + request.args + " :No such nick\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoSuchServer(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "No such server" << std::endl;

	errorMessage = ":" + server.getServerName() + " 402 " + client.getNickName() + " " + server.getServerName() + " :No such server\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoSuchChannel(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "No such channel" << std::endl;

	errorMessage = ":" + server.getServerName() + " 403 " + client.getNickName() + " " + getChannelName(request.params) + " :No such channel\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errCannotSendToChan(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Cannot send to channel" << std::endl;

	errorMessage = ":" + server.getServerName() + " 404 " + client.getNickName() + " " + getChannelName(request.params) + " :Cannot send to channel\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errTooManyChannels(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "You have joined too many channels" << std::endl;

	errorMessage = ":" + server.getServerName() + " 405 " + client.getNickName() + " " + getChannelName(request.params) + " :You have joined too many channels\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errWasNoSuchNick(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "There was no such nickname" << std::endl;

	errorMessage = ":" + server.getServerName() + " 406 " + client.getNickName() + " " + request.args + " :There was no such nickname\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoOrigin(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "No origin specified" << std::endl;

	errorMessage = ":" + server.getServerName() + " 409 " + client.getNickName() + " :No origin specified\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoRecipient(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "No recipient given" << std::endl;

	errorMessage = ":" + server.getServerName() + " 411 " + client.getNickName() + " :" + request.cmd + " :No recipient given\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoTextToSend(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "No text to send" << std::endl;

	errorMessage = ":" + server.getServerName() + " 412 " + client.getNickName() + " :No text to send\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errInputTooLong(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Input line was too long" << std::endl;

	errorMessage = ":" + server.getServerName() + " 417 " + client.getNickName() + " :Input line was too long\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errUnknownCommand(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int	ret;

	std::cout << RED << "Error: " << RESET << "unknown command" << std::endl;

	errorMessage = ":" + server.getServerName() + " 421 " + client.getNickName() + " " + request.cmd + " :Unknown command\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoMotd(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "MOTD File is missing" << std::endl;

	errorMessage = ":" + server.getServerName() + " 422 " + client.getNickName() + " :MOTD File is missing\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoNicknameGiven(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "No nickname given" << std::endl;

	errorMessage = ":" + server.getServerName() + " 431 " + client.getNickName() + " :No nickname given\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errErroneusNickname(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Erroneus nickname" << std::endl;

	errorMessage = ":" + server.getServerName() + " 432 " + client.getNickName() + " " + request.params[0] + " :Erroneus nickname\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNicknameInUse(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Nickname is already in use" << std::endl;

	errorMessage = ":" + server.getServerName() + " 433 " + client.getNickName() + " " + request.params[0] + " :Nickname is already in use\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNickCollision(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Nickname collision" << std::endl;

	errorMessage = ":" + server.getServerName() + " 436 " + client.getNickName() + " " + request.params[0] + " :Nickname collision KILL from " + client.getUserName() + "@" + server.getHostName() + "\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errUserNotInChannel(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "They aren't on that channel" << std::endl;

	errorMessage = ":" + server.getServerName() + " 441 " + client.getNickName() + " " + getChannelName(request.params) + " :They aren't on that channel\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNotOnChannel(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "You're not on that channel" << std::endl;

	errorMessage = ":" + server.getServerName() + " 442 " + client.getNickName() + " " + getChannelName(request.params) + " :You're not on that channel\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errUserOnChannel(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "User is already on channel" << std::endl;

	errorMessage = ":" + server.getServerName() + " 443 " + client.getNickName() + " " + getChannelName(request.params) + " :User is already on channel\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNotRegistered(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "You have not registered" << std::endl;

	errorMessage = ":" + server.getServerName() + " 451 " + client.getNickName() + " :You have not registered\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNeedMoreParams(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Not enough parameters" << std::endl;

	errorMessage = ":" + server.getServerName() + " 461 " + client.getNickName() + " " + request.args + " :Not enough parameters\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errAlreadyRegistered(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "You may not reregister" << std::endl;

	errorMessage = ":" + server.getServerName() + " 462 " + client.getNickName() + " :You may not reregister\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errPasswdMismatch(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Password incorrect" << std::endl;

	errorMessage = ":" + server.getServerName() + " 464 " + client.getNickName() + " :Password incorrect\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errYoureBannedCreep(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "You are banned from this server" << std::endl;

	errorMessage = ":" + server.getServerName() + " 465 " + client.getNickName() + " :You are banned from this server\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errChannelIsFull(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Cannot join channel (+l)" << std::endl;

	errorMessage = ":" + server.getServerName() + " 471 " + client.getNickName() + " " + getChannelName(request.params) + " :Cannot join channel (+l)\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errUnknownMode(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Unknown mode character" << std::endl;

	errorMessage = ":" + server.getServerName() + " 472 " + client.getNickName() + " " + request.args + " :is unknown mode char to me\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errInviteOnlyChan(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Cannot join channel (+i)" << std::endl;

	errorMessage = ":" + server.getServerName() + " 473 " + client.getNickName() + " " + getChannelName(request.params) + " :Cannot join channel (+i)\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errBannedFromChan(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Cannot join channel (+b)" << std::endl;

	errorMessage = ":" + server.getServerName() + " 474 " + client.getNickName() + " " + getChannelName(request.params) + " :Cannot join channel (+b)\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errBadChannelKey(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Cannot join channel (+k)" << std::endl;

	errorMessage = ":" + server.getServerName() + " 475 " + client.getNickName() + " " + getChannelName(request.params) + " :Cannot join channel (+k)\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errBadChanMask(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "Bad Channel Mask" << std::endl;

	errorMessage = ":" + server.getServerName() + " 476 " + client.getNickName() + " " + getChannelName(request.params) + " :Bad Channel Mask\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoPrivileges(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Permission Denied - You're not an IRC operator" << std::endl;

	errorMessage = ":" + server.getServerName() + " 481 " + client.getNickName() + " :Permission Denied - You're not an IRC operator\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errChanOpPrivsNeeded(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "You're not channel operator" << std::endl;

	errorMessage = ":" + server.getServerName() + " 482 " + client.getNickName() + " " + request.args + " :You're not channel operator\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errCantKillServer(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "You can't kill a server!" << std::endl;

	errorMessage = ":" + server.getServerName() + " 483 " + client.getNickName() + " :You can't kill a server!\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoOperHost(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "No O-lines for your host" << std::endl;

	errorMessage = ":" + server.getServerName() + " 491 " + client.getNickName() + " :No O-lines for your host\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errUModeUnknownFlag(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Unknown MODE flag" << std::endl;

	errorMessage = ":" + server.getServerName() + " 501 " + client.getNickName() + " :Unknown MODE flag\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errUsersDontMatch(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Can't change mode for other users" << std::endl;

	errorMessage = ":" + server.getServerName() + " 502 " + client.getNickName() + " :Can't change mode for other users\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errHelpNotFound(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	std::cout << RED << "Error: " << RESET << "No help available on this topic" << std::endl;

	errorMessage = ":" + server.getServerName() + " 524 " + client.getNickName() + " " + request.args + " :No help available on this topic\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errInvalidKey(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Key is not well-formed" << std::endl;

	errorMessage = ":" + server.getServerName() + " 525 " + client.getNickName() + " " + request.args + " :Key is not well-formed\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errInvalidModeParam(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Invalid mode parameter" << std::endl;

	errorMessage = ":" + server.getServerName() + " 696 " + client.getNickName() + " " + request.args + " :Invalid mode parameter\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNoPrivs(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "Insufficient oper privileges" << std::endl;

	errorMessage = ":" + server.getServerName() + " 723 " + client.getNickName() + " " + request.args + " :Insufficient oper privileges\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}

void Error::_errNickLocked(Client &client, Server &server, t_request &request)
{
	std::string errorMessage;
	int ret;

	(void)request;
	std::cout << RED << "Error: " << RESET << "You must use a nick assigned to you" << std::endl;

	errorMessage = ":" + server.getServerName() + " 902 " + client.getNickName() + " :You must use a nick assigned to you\r\n";

	ret = send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	if (ret < 0)
		errorFunction("send: ");

	std::cout << CYAN << "Server: `" << errorMessage << "`" << RESET << std::endl;
}
