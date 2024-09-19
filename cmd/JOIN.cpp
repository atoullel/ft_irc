#include "../include/Server.hpp"

int Server::SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string ChStr, PassStr, buff;
	std::istringstream iss(cmd);
	while(iss >> cmd)
		tmp.push_back(cmd);
	if (tmp.size() < 2) {token.clear(); return 0;}
	tmp.erase(tmp.begin());
	ChStr = tmp[0]; tmp.erase(tmp.begin());
	if (!tmp.empty()) {PassStr = tmp[0]; tmp.clear();}
	for (size_t i = 0; i < ChStr.size(); i++){
		if (ChStr[i] == ',')
				{token.push_back(std::make_pair(buff, "")); buff.clear();}
		else buff += ChStr[i];
	}
	token.push_back(std::make_pair(buff, ""));
	if (!PassStr.empty()){
		size_t j = 0; buff.clear();
		for (size_t i = 0; i < PassStr.size(); i++){
			if (PassStr[i] == ',')
				{token[j].second = buff; j++; buff.clear();}
			else buff += PassStr[i];
		}
		token[j].second = buff;
	}
	for (size_t i = 0; i < token.size(); i++)//erase the empty Salon names
		{if (token[i].first.empty())token.erase(token.begin() + i--);}
	for (size_t i = 0; i < token.size(); i++){//ERR_NOSUCHSalon (403) // if the Salon doesn't exist
		if (*(token[i].first.begin()) != '#')
			{senderror(403, GetClient(fd)->GetNickName(), token[i].first, GetClient(fd)->GetFd(), " :No such Salon\r\n"); token.erase(token.begin() + i--);}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

int Server::SearchForClients(std::string nickname)
{
	int count = 0;
	for (size_t i = 0; i < this->_salon.size(); i++){
		if (this->_salon[i].GetClientInSalon(nickname))
			count++;
	}
	return count;
}

bool IsInvited(Client *cli, std::string ChName, int flag){
	if(cli->GetInviteSalon(ChName)){
		if (flag == 1)
			cli->RmSalonInvite(ChName);
		return true;
	}
	return false;
}

void Server::ExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int j, int fd)
{
	if (this->_salon[j].GetClientInSalon(GetClient(fd)->GetNickName()))// if the client is already registered
		return;
	if (SearchForClients(GetClient(fd)->GetNickName()) >= 10)//ERR_TOOMANYSalonS (405) // if the client is already in 10 Salons
		{senderror(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many Salons\r\n"); return;}
	if (!this->_salon[j].GetPassword().empty() && this->_salon[j].GetPassword() != token[i].second){// ERR_BADSalonKEY (475) // if the password is incorrect
		if (!IsInvited(GetClient(fd), token[i].first, 0))
			{senderror(475, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join Salon (+k) - bad key\r\n"); return;}
	}
	if (this->_salon[j].GetInvitOnly()){// ERR_INVITEONLYCHAN (473) // if the Salon is invit only
		if (!IsInvited(GetClient(fd), token[i].first, 1))
			{senderror(473, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join Salon (+i)\r\n"); return;}
	}
	if (this->_salon[j].GetLimit() && this->_salon[j].GetClientsNumber() >= this->_salon[j].GetLimit())// ERR_SalonISFULL (471) // if the Salon reached the limit of number of clients
		{senderror(471, GetClient(fd)->GetNickName(), "#" + token[i].first, GetClient(fd)->GetFd(), " :Cannot join Salon (+l)\r\n"); return;}
	// add the client to the Salon
	Client *cli = GetClient(fd);
	this->_salon[j].add_client(*cli);
	if(_salon[j].GetTopicName().empty())
		_sendResponse(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),token[i].first) + \
			RPL_NAMREPLY(GetClient(fd)->GetNickName(),_salon[j].GetName(),_salon[j].clientSalon_list()) + \
			RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),_salon[j].GetName()),fd);
	else
		_sendResponse(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),token[i].first) + \
			RPL_TOPICIS(GetClient(fd)->GetNickName(),_salon[j].GetName(),_salon[j].GetTopicName()) + \
			RPL_NAMREPLY(GetClient(fd)->GetNickName(),_salon[j].GetName(),_salon[j].clientSalon_list()) + \
			RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),_salon[j].GetName()),fd);
    _salon[j].sendTo_all(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),token[i].first), fd);
}


void Server::NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
	if (SearchForClients(GetClient(fd)->GetNickName()) >= 10)//ERR_TOOMANYSalonS (405) // if the client is already in 10 Salons
		{senderror(405, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :You have joined too many Salons\r\n"); return;}
	Salon newSalon;
	newSalon.SetName(token[i].first);
	newSalon.add_admin(*GetClient(fd));
	newSalon.set_createiontime();
	this->_salon.push_back(newSalon);
	// notifiy thet the client joined the Salon
    _sendResponse(RPL_JOINMSG(GetClient(fd)->getHostname(),GetClient(fd)->getIpAdd(),newSalon.GetName()) + \
        RPL_NAMREPLY(GetClient(fd)->GetNickName(),newSalon.GetName(),newSalon.clientSalon_list()) + \
        RPL_ENDOFNAMES(GetClient(fd)->GetNickName(),newSalon.GetName()),fd);
}

void Server::joinSalon(std::string cmd, int fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	// SplitJoin(token, cmd, fd);
	if (!SplitJoin(token, cmd, fd))// ERR_NEEDMOREPARAMS (461) // if the Salon name is empty
		{senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); return;}
	if (token.size() > 10) //ERR_TOOMANYTARGETS (407) // if more than 10 Salons
		{senderror(407, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Too many Salons\r\n"); return;}
	for (size_t i = 0; i < token.size(); i++){
		bool flag = false;
		for (size_t j = 0; j < this->_salon.size(); j++){
			if (this->_salon[j].GetName() == token[i].first){
				ExistCh(token, i, j, fd);
				flag = true; break;
			}
		}
		if (!flag)
			NotExistCh(token, i, fd);
	}
}
