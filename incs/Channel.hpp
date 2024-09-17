#pragma once

#include <iostream>
#include <vector>
#include "Error.hpp"
#include "struct.hpp"

class Client;

class Channel : public Error
{
	private:
		std::string				_name;
		std::string				_topic;
		std::string				_key;
		int						_max_clients;
		std::vector<Client *>	_clients;
		std::vector<Client *>	_operators;
		bool					_protected_topic_mode;
		bool					_invite_only_mode;
		bool					_key_protected_mode;

	public:
		Channel(std::string name);
		~Channel(void) throw();

		// Getters
		std::string				getName(void) const;
		std::vector<Client *>	getClients(void) const;
		Client					*getClientByNickName(std::string const &nickname) const;
		std::vector<Client *>	const&getOperators(void) const;
		std::string				getTopic(void) const;
		std::string				getKey(void) const;
		Client					*getOperatorByName(std::string name) const;
		int						getOperatorIdByName(std::string name) const;
		int						getMaxClients(void);


		// Setters
		void			setOperator(Client *client);
		void			setTopic(std::string topic);
		void			setKey(std::string key);
		void			setInviteOnlyMode(bool state);
		void			setProtectedTopicMode(bool state);
		void			setKeyProtectedMode(bool state, std::string key);
		void			setMaxClients(int max);

		// Methods
		void			addClient(Client *client);
		void			removeOperator(Client *client);
		void 			removeClient(Client *client);
		bool			isProtectedTopic();
		bool			isInviteOnly();
		bool			isOperator(Client *client);
		bool			isKeyProtected();

};

