#ifndef FT_IRC_CLIENT_H
# define FT_IRC_CLIENT_H

	#include "Server.hpp"
	#include <string>

	class Client {
		private:
			int			_fd;
			std::string _ipaddr;
			std::string	_username;
			std::string	_nickname;
			std::string _buffer;
			bool 		_operator;
			bool 		_registered;


		public:
			Client();
			Client(Client const &src);
			~Client();
			Client &operator=(Client const &src);

			void	setFd(int fd);
			void	setIp(std::string ip);
			std::string	setBuff(std::string src);
			std::string	getBuff();
			void		clearBuff();
			int			getFd();
};

#endif //FT_IRC_CLIENT_H
