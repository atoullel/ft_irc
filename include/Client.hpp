#ifndef FT_IRC_CLIENT_H
# define FT_IRC_CLIENT_H

	#include "Server.hpp"

	class Client {
		private:
			int			_fd;
			std::string	username;
			std::string	nickname;
			bool 		_operator;
			bool 		registered;


		public:
			Client();
			Client(Client const &src);
			~Client();
			Client &operator=(Client const &src);
};

#endif //FT_IRC_CLIENT_H
