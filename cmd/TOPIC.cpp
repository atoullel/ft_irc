#include "../include/Server.hpp"
#include <ctime>
/*
ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHSalon (403)
ERR_NOTONSalon (442)
ERR_CHANOPRIVSNEEDED (482)
:localhost 442 #jj :You're not on that Salon
:localhost 442 jj :You're Not a Salon operator
*/
std::string Server::tTopic()
{
	std::time_t current = std::time(NULL);
	std::stringstream res;

	res << current;
	return res.str();
}
std::string Server::gettopic(std::string &input)
{
	size_t pos = input.find(":");
	if (pos == std::string::npos)
	{
		return "";
	}
	return input.substr(pos);
}

int Server::getpos(std::string &cmd)
{
	for (int i = 0; i < (int)cmd.size(); i++)
		if (cmd[i] == ':' && (cmd[i - 1] == 32))
			return i;
	return -1;
}

void Server::changeTopic(std::string cmd, int fd)
{
	if (cmd == "TOPIC :")
		{senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");return;} // ERR_NEEDMOREPARAMS (461) if there are not enough parameters
	std::vector<std::string> scmd = split_cmd(cmd);
	if (scmd.size() == 1)
		{senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");return;} // ERR_NEEDMOREPARAMS (461) if there are not enough parameters
	std::string nmch = scmd[1].substr(1);
	if (!GetSalon(nmch)) // ERR_NOSUCHSalon (403) if the given Salon does not exist
		{senderror(403, "#" + nmch, fd, " :No such Salon\r\n"); return;}
	if (!(GetSalon(nmch)->get_client(fd)) && !(GetSalon(nmch)->get_admin(fd)))
		{senderror(442, "#" + nmch, fd, " :You're not on that Salon\r\n");return;} // ERR_NOTONSalon (442) if the client is not on the Salon
	if (scmd.size() == 2)
	{
		if (GetSalon(nmch)->GetTopicName() == "")
		{_sendResponse(": 331 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " :No topic is set\r\n", fd);return;} // RPL_NOTOPIC (331) if no topic is set
		size_t pos = GetSalon(nmch)->GetTopicName().find(":");
		if (GetSalon(nmch)->GetTopicName() != "" && pos == std::string::npos)
		{
			_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + GetSalon(nmch)->GetTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + GetClient(fd)->GetNickName() + " " + GetSalon(nmch)->GetTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
		else
		{
			size_t pos = GetSalon(nmch)->GetTopicName().find(" ");
			if (pos == 0)
				GetSalon(nmch)->GetTopicName().erase(0, 1);
			_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + GetSalon(nmch)->GetTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + GetClient(fd)->GetNickName() + " " + GetSalon(nmch)->GetTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
	}

	if (scmd.size() >= 3)
	{
		std::vector<std::string> tmp;
		int pos = getpos(cmd);
		if (pos == -1 || scmd[2][0] != ':')
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(scmd[2]);
		}
		else
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(cmd.substr(getpos(cmd)));
		}

		if (tmp[2][0] == ':' && tmp[2][1] == '\0')
		{senderror(331, "#" + nmch, fd, " :No topic is set\r\n");return;} // RPL_NOTOPIC (331) if no topic is set

		if (GetSalon(nmch)->Gettopic_restriction() && GetSalon(nmch)->get_client(fd))
		{senderror(482, "#" + nmch, fd, " :You're Not a Salon operator\r\n");return;} // ERR_CHANOPRIVSNEEDED (482) if the client is not a Salon operator
		else if (GetSalon(nmch)->Gettopic_restriction() && GetSalon(nmch)->get_admin(fd))
		{
			GetSalon(nmch)->SetTime(tTopic());
			GetSalon(nmch)->SetTopicName(tmp[2]);
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + GetSalon(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			else
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " " + GetSalon(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			GetSalon(nmch)->sendTo_all(rpl);
		}
		else
		{
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
			{
				GetSalon(nmch)->SetTime(tTopic());
				GetSalon(nmch)->SetTopicName(tmp[2]);
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " " + GetSalon(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			else
			{
				size_t poss = tmp[2].find(" ");
				GetSalon(nmch)->SetTopicName(tmp[2]);
				if (poss == std::string::npos && tmp[2][0] == ':' && tmp[2][1] != ':')
					tmp[2] = tmp[2].substr(1);
				GetSalon(nmch)->SetTopicName(tmp[2]);
				GetSalon(nmch)->SetTime(tTopic());
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " " + GetSalon(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			GetSalon(nmch)->sendTo_all(rpl);
		}
	}
}

