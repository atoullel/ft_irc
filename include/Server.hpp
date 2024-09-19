#ifndef SERVER_HPP
# define SERVER_HPP

	#include <iostream>
	#include <cstdlib>
	#include <vector>
	#include <string>
	#include <sys/socket.h>
    #include <netinet/in.h> //Internet domain sockets
	#include <sys/epoll.h>
	#include <unistd.h>
	#include <string.h>
	#include <csignal>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <sstream>
	#include <errno.h>


	#include "Client.hpp"
	#include "Define.hpp"
	#include "Salon.hpp"

	#define WHITE "\e[0;37m"
	#define GREEN "\e[1;32m"
	#define YELLOW "\e[1;33m"
	#define RED "\e[1;31m"
	#define	INTRO "██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗    ████████╗ ██████╗ \n"\
	"██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝    ╚══██╔══╝██╔═══██╗\n"\
	"██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗         ██║   ██║   ██║\n"\
	"██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝         ██║   ██║   ██║\n"\
	"╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗       ██║   ╚██████╔╝\n"\
	" ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝       ╚═╝    ╚═════╝ \n"\
	"\n"\
	"███████╗████████╗        ██╗██████╗  ██████╗\n"\
	"██╔════╝╚══██╔══╝        ██║██╔══██╗██╔════╝\n"\
	"█████╗     ██║           ██║██████╔╝██║     \n"\
	"██╔══╝     ██║           ██║██╔══██╗██║     \n"\
	"██║        ██║   ███████╗██║██║  ██║╚██████╗\n"\
	"╚═╝        ╚═╝   ╚══════╝╚═╝╚═╝  ╚═╝ ╚═════╝\n"\

	#define MAX_EVENTS 1024

	class Client;
	// class Salon;


	class Server {
		private:
			static bool			Signal;
			int					_port;
			std::string			_password;
			std::vector<Salon>  _salon;
			std::vector<Client>	_clients;
			int 				_serv_socket;
			struct sockaddr_in	_serv_addr;
			struct epoll_event	_evlist[MAX_EVENTS]; // list used by epoll_wait
			int 				_epfd; // epoll instance file descriptor


		public:
			//static bool			Signal;

			Server();
		Server(int port, std::string _password);
	~Server();
	Server(Server const &src);
	Server &operator=(Server const &src);
	//----------------- Getters -----------------//

int GetPort(){return this->_port;}
int GetFd(){return this->_serv_socket;}
Client *GetClient(int fd){
	for (size_t i = 0; i < this->_clients.size(); i++){
		if (this->_clients[i].GetFd() == fd)
			return &this->_clients[i];
	}
	return NULL;
}
Client *GetClientNick(std::string nickname){
	for (size_t i = 0; i < this->_clients.size(); i++){
		if (this->_clients[i].GetNickName() == nickname)
			return &this->_clients[i];
	}
	return NULL;
}

Salon *GetSalon(std::string name)
{
	for (size_t i = 0; i < this->_salon.size(); i++){
		if (this->_salon[i].GetName() == name)
			return &_salon[i];
	}
	return NULL;
}

void Launch();
void Init_server();
void newClient();
void ft_error(std::string reason);
void newData(int fd);
//SETTERS
void SetFd(int fd){this->_serv_socket = fd;}
void SetPort(int _port){this->_port = _port;}
void SetPassword(std::string _password){this->_password = _password;}
std::string GetPassword(){return this->_password;}
void AddClient(Client newClient){this->_clients.push_back(newClient);}
void AddChannel(Salon newChannel){this->_salon.push_back(newChannel);}
	void set_username(std::string username, int fd);
	void set_nickname(std::string cmd, int fd);
	//---------------//Remove Methods
	void RemoveClient(int fd);
	void RemoveSalon(std::string name);
	void RmSalons(int fd);
	//---------------//Send Methods
	void senderror(int code, std::string clientname, int fd, std::string msg);
	void senderror(int code, std::string clientname, std::string salonname, int fd, std::string msg);
	void 		_sendResponse(std::string response, int fd);
	//---------------//Close and Signal Methods
	static void SignalHandler(int signum);
	//---------------//Parsing Methods
	std::vector<std::string> split_recivedBuffer(std::string str);
	std::vector<std::string> split_cmd(std::string &str);
	void parsing_switch(std::string& cmd, int clientFd);
	//---------------//Authentification Methods
	bool notregistered(int fd);
	bool nickNameInUse(std::string& nickname);
	bool is_validNickname(std::string& nickname);
	void client_authen(std::string cmd, int fd);
	//---------------------------//JOIN CMD
	void	joinSalon(std::string cmd, int fd);
	int		SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
	void	ExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd);
	void	NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
	int		SearchForClients(std::string nickname);
	//---------------------------//PART CMD
	void	leaveSalon(std::string cmd, int fd);
	int		SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd);
	//---------------------------//CKIK CMD
	void	kickClient(std::string cmd, int fd);
	std::string SplitCmdKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd);
	//---------------------------//PRIVMSG CMD
	void	privateMessage(std::string cmd, int fd);
	void	CheckForSalons_Clients(std::vector<std::string> &tmp, int fd);
	//---------------------------//QUITE CMD
	void	quitClient(std::string cmd, int fd);
	//---------------------------//MODE CMD
	void 		changeMode(std::string cmd, int fd);
	std::string invite_only(Salon *Salon, char opera, std::string chain);
	std::string topic_restriction(Salon *Salon ,char opera, std::string chain);
	std::string password_mode(std::vector<std::string> splited, Salon *Salon, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	std::string operator_privilege(std::vector<std::string> splited, Salon *Salon, size_t& pos, int fd, char opera, std::string chain, std::string& arguments);
	std::string Salon_limit(std::vector<std::string> splited, Salon *Salon, size_t &pos, char opera, int fd, std::string chain, std::string& arguments);
	bool		isvalid_limit(std::string& limit);
	std::string mode_toAppend(std::string chain, char opera, char mode);
	std::vector<std::string> splitParams(std::string params);
	void getCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params);
	//---------------------------//TOPIC CMD
	std::string tTopic();
	void changeTopic(std::string cmd, int fd);
	void inviteClient(std::string cmd, int fd);
	std::string gettopic(std::string& input);
	int getpos(std::string &cmd);
	std::vector<std::string> splitMessage(std::string input);
	std::vector<std::string> splitCommand(std::string& cmd);
	};

#endif //FT_IRC_SERVER_H
