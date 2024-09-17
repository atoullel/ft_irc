#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <memory>
# include <string>
# include "Error.hpp"
# include "struct.hpp"

// class Channel;
class Client;
class Channel;

class Server : public Error
{
private:
// attributes
	std::string					_serverName;
	std::string					_hostName;
	t_fds						_fds;
	t_socket					_socket;
	std::string					_password;
	std::vector<Client *>		_clients;
	std::vector<Channel *>		_channels;
	int							_maxChannelPerUser;

// methods
	void					_createSocket(void);
	void					_addClient(void);
	void					_recvMessage(Client &client);
	void					_sendMessage(Client &client);
	void					_broadcastMessage(Client &client);
public:
// Constructor / Destructor
	Server(unsigned short &port, std::string &password);
	virtual ~Server(void) throw();

// Getters
	std::string				getServerName(void) const;
	std::string				getHostName(void) const;
	std::string				getPassword(void) const;
	std::vector<Client *>	getClients(void);
	Client					*getClientByNickName(std::string &nickname) const;
	Channel					*getChannelByName(std::string &name) const;
	std::vector<Channel *>	getAllChannels(void) const;
	int						getMaxChannelsPerUser(void) const;

// Methods
	void					start(void);
	void					deleteClient(Client &client);
	bool 					isRegistered(std::string &username) const;
	void					addChannel(Channel *channel);
	void					sendJoinMessage(Client &client, Server &server, Channel &channel);
	void					sendUsersList(Client &client, Server &server, Channel &channel);
	void					sendTopicMessage(Client &client, Server &server, Channel &channel);
	void					setMaxChannelsPerUser(int max);
};

#endif
