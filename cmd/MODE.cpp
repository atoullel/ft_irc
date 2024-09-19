#include "../include/Server.hpp"

std::string Server::mode_toAppend(std::string chain, char opera, char mode)
{
	std::stringstream ss;

	ss.clear();
	char last = '\0';
	for(size_t i = 0; i < chain.size(); i++)
	{
		if(chain[i] == '+' || chain[i] == '-')
			last = chain[i];
	}
	if(last != opera)
		ss << opera << mode;
	else
		ss << mode;
	return ss.str();
}

void Server::getCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params)
{
	std::istringstream stm(cmd);
	stm >> name;
	stm >> modeset;
	size_t found = cmd.find_first_not_of(name + modeset + " \t\v");
	if(found != std::string::npos)
		params = cmd.substr(found);
}


std::vector<std::string> Server::splitParams(std::string params)
{
	if(!params.empty() && params[0] == ':')
		params.erase(params.begin());
	std::vector<std::string> tokens;
	std::string param;
	std::istringstream stm(params);
	while (std::getline(stm, param, ','))
	{
		tokens.push_back(param);
		param.clear();
	}
	return tokens;
}

void Server::changeMode(std::string cmd, int clientFd)
{
	std::string SalonName;
	std::string params;
	std::string modeset;
	std::stringstream mode_chain;
	std::string arguments = ":";
	Salon *Salon;
	char opera = '\0';

	arguments.clear();
	mode_chain.clear();
	Client *cli = GetClient(clientFd);
	size_t found = cmd.find_first_not_of("MODEmode \t\v");
	if(found != std::string::npos)
		cmd = cmd.substr(found);
	else
	{
		_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), clientFd);
		return ;
	}
	getCmdArgs(cmd ,SalonName, modeset ,params);
	std::vector<std::string> tokens = splitParams(params);
	if(SalonName[0] != '#' || !(Salon = GetSalon(SalonName.substr(1))))
	{
		_sendResponse(ERR_SalonNOTFOUND(cli->GetUserName(),SalonName), clientFd);
		return ;
	}
	else if (!Salon->get_client(clientFd) && !Salon->get_admin(clientFd))
	{
		senderror(442, GetClient(clientFd)->GetNickName(), SalonName, GetClient(clientFd)->GetFd(), " :You're not on that Salon\r\n"); return;
	}
	else if (modeset.empty()) // response with the Salon modes (MODE #Salon)
	{
		_sendResponse(RPL_SALONMODES(cli->GetNickName(), Salon->GetName(), Salon->getModes()) + \
		RPL_CREATIONTIME(cli->GetNickName(), Salon->GetName(),Salon->get_creationtime()),clientFd);
		return ;
	}
	else if (!Salon->get_admin(clientFd)) // client is not Salon operator
	{
		_sendResponse(ERR_NOTOPERATOR(Salon->GetName()), clientFd);
		return ;
	}
	else if(Salon)
	{
		size_t pos = 0;
		for(size_t i = 0; i < modeset.size(); i++)
		{
			if(modeset[i] == '+' || modeset[i] == '-')
				opera = modeset[i];
			else
			{
				if(modeset[i] == 'i')//invite mode
					mode_chain << invite_only(Salon , opera, mode_chain.str());
				else if (modeset[i] == 't') //topic restriction mode
					mode_chain << topic_restriction(Salon, opera, mode_chain.str());
				else if (modeset[i] == 'k') //password set/remove
					mode_chain <<  password_mode(tokens, Salon, pos, opera, clientFd, mode_chain.str(), arguments);
				else if (modeset[i] == 'o') //set/remove user operator privilege
						mode_chain << operator_privilege(tokens, Salon, pos, clientFd, opera, mode_chain.str(), arguments);
				else if (modeset[i] == 'l') //set/remove Salon limits
					mode_chain << Salon_limit(tokens, Salon, pos, opera, clientFd, mode_chain.str(), arguments);
				else
					_sendResponse(ERR_UNKNOWNMODE(cli->GetNickName(), Salon->GetName(),modeset[i]),clientFd);
			}
		}
	}
	std::string chain = mode_chain.str();
	if(chain.empty())
		return ;
 	Salon->sendTo_all(RPL_CHANGEMODE(cli->getHostname(), Salon->GetName(), mode_chain.str(), arguments));
}

std::string Server::invite_only(Salon *Salon, char opera, std::string chain)
{
	std::string param;
	param.clear();
	if(opera == '+' && !Salon->getModeAtindex(0))
	{
		Salon->setModeAtindex(0, true);
		Salon->SetInvitOnly(1);
		param =  mode_toAppend(chain, opera, 'i');
	}
	else if (opera == '-' && Salon->getModeAtindex(0))
	{
		Salon->setModeAtindex(0, false);
		Salon->SetInvitOnly(0);
		param =  mode_toAppend(chain, opera, 'i');
	}
	return param;
}

std::string Server::topic_restriction(Salon *Salon ,char opera, std::string chain)
{
	std::string param;
	param.clear();
	if(opera == '+' && !Salon->getModeAtindex(1))
	{
		Salon->setModeAtindex(1, true);
		Salon->set_topicRestriction(true);
		param =  mode_toAppend(chain, opera, 't');
	}
	else if (opera == '-' && Salon->getModeAtindex(1))
	{
		Salon->setModeAtindex(1, false);
		Salon->set_topicRestriction(false);
		param =  mode_toAppend(chain, opera, 't');
	}
	return param;
}

bool validPassword(std::string password)
{
	if(password.empty())
		return false;
	for(size_t i = 0; i < password.size(); i++)
	{
		if(!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}
std::string Server::password_mode(std::vector<std::string> tokens, Salon *Salon, size_t &pos, char opera, int fd, std::string chain, std::string &arguments)
{
	std::string param;
	std::string pass;

	param.clear();
	pass.clear();
	if(tokens.size() > pos)
		pass = tokens[pos++];
	else
	{
		_sendResponse(ERR_NEEDMODEPARM(Salon->GetName(),std::string("(k)")),fd);
		return param;
	}
	if(!validPassword(pass))
	{
		_sendResponse(ERR_INVALIDMODEPARM(Salon->GetName(),std::string("(k)")),fd);
		return param;
	}
	if(opera == '+')
	{
		Salon->setModeAtindex(2, true);
		Salon->SetPassword(pass);
		if(!arguments.empty())
			arguments += " ";
		arguments += pass;
		param = mode_toAppend(chain,opera, 'k');
	}
	else if (opera == '-' && Salon->getModeAtindex(2))
	{
		if(pass == Salon->GetPassword())
		{
			Salon->setModeAtindex(2, false);
			Salon->SetPassword("");
			param = mode_toAppend(chain,opera, 'k');
		}
		else
			_sendResponse(ERR_KEYSET(Salon->GetName()),fd);
	}
	return param;
}

std::string Server::operator_privilege(std::vector<std::string> tokens, Salon *Salon, size_t& pos, int fd, char opera, std::string chain, std::string& arguments)
{
	std::string user;
	std::string param;

	param.clear();
	user.clear();
	if(tokens.size() > pos)
		user = tokens[pos++];
	else
	{
		_sendResponse(ERR_NEEDMODEPARM(Salon->GetName(),"(o)"),fd);
		return param;
	}
	if(!Salon->clientInSalon(user))
	{
		_sendResponse(ERR_NOSUCHNICK(Salon->GetName(), user),fd);
		return param;
	}
	if(opera == '+')
	{
		Salon->setModeAtindex(3,true);
		if(Salon->change_clientToAdmin(user))
		{
			param = mode_toAppend(chain, opera, 'o');
			if(!arguments.empty())
				arguments += " ";
			arguments += user;
		}
	}
	else if (opera == '-')
	{
		Salon->setModeAtindex(3,false);
		if(Salon->change_adminToClient(user))
		{
			param = mode_toAppend(chain, opera, 'o');
				if(!arguments.empty())
					arguments += " ";
			arguments += user;

		}
	}
	return param;
}

bool Server::isvalid_limit(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

std::string Server::Salon_limit(std::vector<std::string> tokens,  Salon *Salon, size_t &pos, char opera, int fd, std::string chain, std::string& arguments)
{
	std::string limit;
	std::string param;

	param.clear();
	limit.clear();
	if(opera == '+')
	{
		if(tokens.size() > pos )
		{
			limit = tokens[pos++];
			if(!isvalid_limit(limit))
			{
				_sendResponse(ERR_INVALIDMODEPARM(Salon->GetName(),"(l)"), fd);
			}
			else
			{
				Salon->setModeAtindex(4, true);
				Salon->SetLimit(std::atoi(limit.c_str()));
				if(!arguments.empty())
					arguments += " ";
				arguments += limit;
				param =  mode_toAppend(chain, opera, 'l');
			}
		}
		else
			_sendResponse(ERR_NEEDMODEPARM(Salon->GetName(),"(l)"),fd);
	}
	else if (opera == '-' && Salon->getModeAtindex(4))
	{
		Salon->setModeAtindex(4, false);
		Salon->SetLimit(0);
		param  = mode_toAppend(chain, opera, 'l');
	}
	return param;
}
