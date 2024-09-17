#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <vector>
# include "Error.hpp"
# include "struct.hpp"

class Server;
class Channel;

class Client : public Error
{
private:
	t_socket					_socketServer;

	int							_clientFd;

	size_t						_idMessage;
	std::string					_messageRecv;
	std::string					_message;

	std::string 				_nickname;
	std::string					_username;
	std::string					_realname;
	std::string					_hostname;
	std::string					_servername;

	bool						_isOperator;
	bool						_isAuthentified;
	std::vector<std::string>	_invitedChannels;
	std::vector<Channel *>		_joinedChannels;

public:
	Client(void);
	Client(t_socket &socketServer);
	virtual ~Client(void) throw();

// Getters
	int			getClientFd(void) const;

	size_t		getIdMessage(void) const;
	std::string	getMessage(void) const;

	std::string getNickName(void) const;
	std::string	getUserName(void) const;
	std::string	getRealName(void) const;
	std::string	getHostName(void) const;
	std::string	getServerName(void) const;
	std::vector<std::string> getInvitedChannels(void) const;
	std::vector<Channel *> getJoinedChannels(void) const;

	bool		isOperator(void) const;
	bool		isAuthentified(void) const;

// Setters
	void		setSocketServer(t_socket &socketServer);

	void		incrementIdMessage(void);

	void		setNickName(std::string &nick);
	void		setUserName(std::string &username);
	void		setRealName(std::string &realname);
	void		setHostName(std::string &hostname);
	void		setServerName(std::string &servername);
	void		setInvitedChannel(std::string &channel);

	void		setOperator(bool &op);
	void		setAuthentified(void);

// Methods
	void		acceptClient(void);
	void		sendMessage(void);
	void		addJoinedChannel(Channel *channel);
	void		removeJoinedChannel(Channel *channel);
	bool		isInvitedChannel(std::string const &channel);
};

#endif
