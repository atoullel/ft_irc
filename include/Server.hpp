#ifndef FT_IRC_SERVER_H
# define FT_IRC_SERVER_H

	#include <iostream>
	#include <vector>
	#include <string>
	#include <sys/socket.h>

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

	class Client;

	class Server {
		private:
			static bool			_Signal;
			int					_port;
			std::string			_password;
			std::vector<Client>	_clients;
			int 				_serv_socket;
			struct sockaddr_in	_serv_addr;
			struct epoll_event	_events[100];
			int 				_num_socket;
			int 				_num_events;
			int 				_epfd;


		public:
			Server();
			Server(int port, std::string password);
			~Server();
			Server	&operator=(Server const &src);

			void newClient();
			void newData();
	};
#endif //FT_IRC_SERVER_H
