#include "../include/Server.hpp"



void FindK(std::string cmd, std::string tofind, std::string &str)
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

std::string SplitCmdK(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, reason;
	int count = 3;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 3) return std::string("");
	FindK(cmd, tmp[2], reason);
	return reason;
}

std::string Server::SplitCmdKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd)
{
	std::string reason = SplitCmdK(cmd, tmp);
	if (tmp.size() < 3) // check if the client send the Salon name and the client to kick
		return std::string("");
	tmp.erase(tmp.begin());
	std::string str = tmp[0]; std::string str1;
	user = tmp[1]; tmp.clear();
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
	return reason;
}

void	Server::kickClient(Server& server, std::string cmd, int fd)
{
	//ERR_BADCHANMASK (476) // if the Salon mask is invalid
	std::vector<std::string> tmp;
	std::string reason ,user;
	reason = SplitCmdKick(cmd, tmp, user, fd);
	if (user.empty())
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	for (size_t i = 0; i < tmp.size(); i++){ // search for the Salon
		if (GetSalon(tmp[i])){// check if the Salon exist
			Salon *ch = GetSalon(tmp[i]);
			if (!ch->get_client(fd) && !ch->get_admin(fd)) // check if the client is in the Salon
				{senderror(442, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not on that Salon\r\n"); continue;}
			if(ch->get_admin(fd)){ // check if the client is admin
				if (ch->GetClientInSalon(user)){ // check if the client to kick is in the Salon
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " KICK #" << tmp[i] << " " << user;
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					ch->sendTo_all(ss.str());
					if (ch->get_admin(ch->GetClientInSalon(user)->GetFd()))
						ch->remove_admin(ch->GetClientInSalon(user)->GetFd());
					else
						ch->remove_client(ch->GetClientInSalon(user)->GetFd());
					if (ch->GetClientsNumber() == 0)
						_salon.erase(_salon.begin() + i);
				}
				else // if the client to kick is not in the Salon
					{senderror(441, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :They aren't on that Salon\r\n"); continue;}
			}
			else // if the client is not admin
				{senderror(482, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not Salon operator\r\n"); continue;}
		}
		else // if the Salon doesn't exist
			senderror(403, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :No such Salon\r\n");
	}
}