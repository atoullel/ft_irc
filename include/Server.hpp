#ifndef FT_IRC_SERVER_H
# define FT_IRC_SERVER_H

	#include <iostream>
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

	#include "../include/Client.hpp"

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

	class Server {
		private:
			static bool			Signal;
			int					_port;
			std::string			_password;
			std::vector<Client>	_clients;
			int 				_serv_socket;
			struct sockaddr_in	_serv_addr;
			struct epoll_event	_evlist[MAX_EVENTS]; // list used by epoll_wait
			int 				_epfd; // epoll instance file descriptor


		public:
			//static bool			Signal;

			Server();
			Server(int port, std::string password);
			~Server();
			Server	&operator=(Server const &src);

			void newClient();
			void newData();

			void		Launch();
			void		Init_server();
			void		newClient();
			void		newData(int fd);
			void		ft_error(std::string reason);
			static void	signalHandler(int signum);

		//PARSING
			void parsing_switch(std::string &cmd, int fd);
	};
#endif //FT_IRC_SERVER_H
