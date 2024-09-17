#include "../include/Server.hpp"

void FindPR(std::string cmd, std::string tofind, std::string &str)
{
	size_t i = 0;
	for (; i < cmd.size(); i++){
		if (cmd[i] != ' '){
			std::string tmp;
			for (; i < cmd.size() && cmd[i] != ' '; i++)
				tmp += cmd[i];
			if (tmp == tofind) break;
			else tmp.clear();
		}
	}
	if (i < cmd.size()) str = cmd.substr(i);
	i = 0;
	for (; i < str.size() && str[i] == ' '; i++);
	str = str.substr(i);
}

std::string SplitCmdPR(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, reason;
	int count = 2;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 2) return std::string("");
	FindPR(cmd, tmp[1], reason);
	return reason;
}

int Server::SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd)
{
	reason = SplitCmdPR(cmd, tmp);
	if(tmp.size() < 2) {tmp.clear(); return 0;}
	tmp.erase(tmp.begin());
	std::string str = tmp[0]; std::string str1; tmp.clear();
	for (size_t i = 0; i < str.size(); i++){//split the first string by ',' to get the Salons names
		if (str[i] == ',')
			{tmp.push_back(str1); str1.clear();}
		else str1 += str[i];
	}
	tmp.push_back(str1);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
	if (reason[0] == ':') reason.erase(reason.begin());
	else //shrink to the first space
		{for (size_t i = 0; i < reason.size(); i++){if (reason[i] == ' '){reason = reason.substr(0, i);break;}}}
	for (size_t i = 0; i < tmp.size(); i++){// erase the '#' from the Salon name and check if the Salon valid
			if (*(tmp[i].begin()) == '#')
				tmp[i].erase(tmp[i].begin());
			else
				{senderror(403, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such Salon\r\n"); tmp.erase(tmp.begin() + i--);}
		}
	return 1;
}

void Server::leaveSalon(Server& server, std::string& cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string reason;
	if (!SplitCmdPart(cmd, tmp, reason, fd))// ERR_NEEDMOREPARAMS (461) // if the Salon name is empty
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	for (size_t i = 0; i < tmp.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->_salon.size(); j++){ // search for the Salon
			if (this->_salon[j].GetName() == tmp[i]){ // check if the Salon exist
				flag = true;
				if (!_salon[j].get_client(fd) && !_salon[j].get_admin(fd)) // ERR_NOTONSalon (442) // if the client is not in the Salon
					{senderror(442, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not on that Salon\r\n"); continue;}
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " PART #" << tmp[i];
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					_salon[j].sendTo_all(ss.str());
					if (_salon[j].get_admin(_salon[j].GetClientInSalon(GetClient(fd)->GetNickName())->GetFd()))
						_salon[j].remove_admin(_salon[j].GetClientInSalon(GetClient(fd)->GetNickName())->GetFd());
					else
						_salon[j].remove_client(_salon[j].GetClientInSalon(GetClient(fd)->GetNickName())->GetFd());
					if (_salon[j].GetClientsNumber() == 0)
						_salon.erase(_salon.begin() + j);
			}
		}
		if (!flag) // ERR_NOSUCHSalon (403) // if the Salon doesn't exist
			senderror(403, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :No such Salon\r\n");
	}
	(void)server;
}
