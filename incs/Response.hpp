#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <sys/socket.h>
# include <map>
# include "struct.hpp"
# include "Error.hpp"

class Client;
class Server;

class Response : public Error
{
private:
	std::map<std::string, int (Response::*)(Client &, Server &, t_request &)>	_response;

	t_request	_parseMessage(std::string &message);

	int			_cap(Client &client, Server &server, t_request &req);
	int			_pass(Client &client, Server &server, t_request &req);
	int			_isValidJoinRequest(t_request &, Client &, Server &, std::vector<std::string> &);
	int			_processChannel(Client &, Server &, std::string, std::string);
	int			_join(Client &client, Server &server, t_request &req);
	int			_nick(Client &client, Server &server, t_request &req);
	int			_user(Client &client, Server &server, t_request &req);
	int			_ping(Client &client, Server &server, t_request &req);
	int			_pong(Client &client, Server &server, t_request &req);
	int			_mode(Client &client, Server &server, t_request &req);
	int			_privmsg(Client &client, Server &server, t_request &req);
	int			_quit(Client &client, Server &server, t_request &req);
	int			_kick(Client &client, Server &server, t_request &req);
	int			_invite(Client &client, Server &server, t_request &req);
	int			_topic(Client &client, Server &server, t_request &req);

public:
	Response(void);
	virtual ~Response(void) throw();

	void	sendResponse(Client &client, Server &server, std::string &message);
	void	sendAnyMsg(Client &client, std::string &message);
	void	sendPrivMessage(Client &client, Server &server, std::string &message, std::string &recipientNickname);
};

int parseUserCmd(t_user_info *user_info, t_request &req, Client &client, Server &server);

#endif
