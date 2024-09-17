#ifndef CLIENT_HPP
# define CLIENT_HPP

// #include "Server.hpp"
#include <string>
#include <vector>

class Salon;
class Server;
	class Client {
		private:
			int			_fd;
			std::string _ipaddr;
			std::string	_username;
			std::string	_nickname;
			bool 		_operator;
			bool 		_registered;
			std::string _buffer;
			std::string _ipadd;
			bool		_logedin;
			std::vector<std::string> ChannelsInvite;


		public:
			Client();
			Client(std::string _nickname, std::string _username, int _fd);
			Client(Client const &src);
			~Client();
			Client &operator=(Client const &src);

			void	SetFd(int fd);
			void	SetIp(std::string ip);
			void SetNickname(std::string& _nickname);
			bool GetInviteSalon(std::string &ChName);
			int		GetFd();
			bool	getRegistered();
			std::string GetNickName();
			bool 		GetLogedIn();
			std::string GetUserName();
			std::string getIpAdd();
			std::string getBuffer();
			std::string getHostname();
			//---------------//Setters
			void setLogedin(bool value);
			void SetUsername(std::string& username);
			void setBuffer(std::string recived);
			void setRegistered(bool value);
			void setIpAdd(std::string ipadd);
			//---------------//Methods
			void clearBuffer();
			void AddSalonlInvite(std::string &chname);
			void RmSalonInvite(std::string &chname);
};

#endif //FT_IRC_CLIENT_H
