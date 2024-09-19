#include "../include/Server.hpp"


void FindQ(std::string cmd, std::string tofind, std::string &str)
{
	size_t i = 0;
	for (; i < cmd.size(); i++){
		if (cmd[i] != ' '){
			std::string tmp;
			for (; i < cmd.size() && cmd[i] != ' '; i++)
				tmp += cmd[i];
			if (tmp == tofind)
				break;
			else tmp.clear();
		}
	}
	if (i < cmd.size())
		str = cmd.substr(i);
	i = 0;
	for (; i < str.size() && str[i] == ' '; i++);
	str = str.substr(i);
}

std::string	SplitQuit(std::string cmd)
{
	std::istringstream stm(cmd);
	std::string reason,str;
	stm >> str;
	FindQ(cmd, str, reason);
	if (reason.empty())
		return std::string("Quit");
	if (reason[0] != ':'){ //if the message does not start with ':'
		for (size_t i = 0; i < reason.size(); i++){
			if (reason[i] == ' ')
				{reason.erase(reason.begin() + i, reason.end());break;}
		}
		reason.insert(reason.begin(), ':');
	}
	return reason;
}

void Server::quitClient(std::string cmd, int fd)
{
	std::string reason;
	reason = SplitQuit(cmd);
	for (size_t i = 0; i < _salon.size(); i++)
	{
		if (_salon[i].get_client(fd)){
			_salon[i].remove_client(fd);
			if (_salon[i].GetClientsNumber() == 0)
				_salon.erase(_salon.begin() + i);
			else{
				std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT " + reason + "\r\n";
				_salon[i].sendTo_all(rpl);
			}
		}
		else if (_salon[i].get_admin(fd)){
			_salon[i].remove_admin(fd);
			if (_salon[i].GetClientsNumber() == 0)
				_salon.erase(_salon.begin() + i);
			else{
				std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT " + reason + "\r\n";
				_salon[i].sendTo_all(rpl);
			}
		}
	}
	std::cout << RED << "Client <" << fd << "> Disconnected" << WHITE << std::endl;
	RmSalons(fd);
	RemoveClient(fd);
	close(fd);
}
