#ifndef SALON_HPP
#define SALON_HPP

// #include "Server.hpp"
#include <sstream>
#include <sys/socket.h>
#include "Client.hpp"
#include <string>
#include <iostream>
#include <vector>

// class Client;
class Salon {
	private:

		int invit_only;
		int topic;
		int key;
		int limit;
		bool topic_restriction;
		std::string name;
		std::string time_creation;
		std::string password;
		std::string created_at;
		std::string topic_name;
		std::vector<Client> clients;
		std::vector<Client> admins;
		std::vector<std::pair<char, bool> > modes;
	public:
		Salon();
		~Salon();
		Salon(Salon const &src);
		Salon &operator=(Salon const &src);
		//---------------//Setters
		void SetInvitOnly(int invit_only);
		void SetTopic(int topic);
		void SetKey(int key);
		void SetLimit(int limit);
		void SetTopicName(std::string topic_name);
		void SetPassword(std::string password);
		void SetName(std::string name);
		void SetTime(std::string time);
		void set_topicRestriction(bool value);
		void setModeAtindex(size_t index, bool mode);
		void set_createiontime();
		//---------------//Getters
		int GetInvitOnly();
		int GetTopic();
		int GetKey();
		int GetLimit();
		int GetClientsNumber();
		bool Gettopic_restriction() const;
		bool getModeAtindex(size_t index);
		bool clientInSalon(std::string &nick);
		std::string GetTopicName();
		std::string GetPassword();
		std::string GetName();
		std::string GetTime();
		std::string get_creationtime();
		std::string getModes();
		std::string clientSalon_list();
		Client *get_client(int fd);
		Client *get_admin(int fd);
		Client *GetClientInSalon(std::string name);
		//---------------//Methods
		void add_client(Client newClient);
		void add_admin(Client newClient);
		void remove_client(int fd);
		void remove_admin(int fd);
		bool change_clientToAdmin(std::string& nick);
		bool change_adminToClient(std::string& nick);
		//---------------//SendToAll
		void sendTo_all(std::string rpl1);
		void sendTo_all(std::string rpl1, int fd);
};

#endif
