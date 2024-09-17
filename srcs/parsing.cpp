#include "libraries.hpp"
# include "Server.hpp"
# include "Client.hpp"


static
int checkPort(char *port)
{
	std::string	port_str = port;
	std::string valid_chars = "0123456789";
	int			port_int = atoi(port);
	long		port_long = atol(port);

	for (size_t i = 0; i < port_str.length(); i++)
	{
		if (valid_chars.find(port_str[i]) == std::string::npos)
		{
			std::cerr << RED << "Error : the port need to be all digits" << RESET << std::endl;
			return (EXIT_FAILURE);
		}
	}
	if (port_int != port_long)
	{
		std::cerr << RED << "Error : overflow int" << RESET << std::endl;
		return (EXIT_FAILURE);
	}
	if (port_int < 1024 || port_int > 65535)
	{
		std::cerr << RED << "Error : please select a port between 1024 and 65535" << RESET << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int	parsing(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << RED << "Usage: " << av[0] << " <port> <password>" << RESET << std::endl;
		return (EXIT_FAILURE);
	}
	if (checkPort(av[1]) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

std::string getChannelName(std::vector<std::string> &params)
{
	std::string channel_name;

	for (size_t i = 0; i < params.size(); i++)
	{
		if (params[i][0] == '#')
		{
			channel_name = params[i];
			break;
		}
	}
	return (channel_name);
}

int parseUserCmd(t_user_info *user_info, t_request &req, Client &client, Server &server)
{
	if (req.params.size() < 4)
		return ERR_NEEDMOREPARAMS;

	user_info->username = req.params[0];
	user_info->hostname = req.params[1];
	user_info->servername = req.params[2];
	user_info->realname = req.params[3];
	if (req.params.size() > 4)
	{
		for (size_t i = 4; i < req.params.size(); i++)
			user_info->realname += " " + req.params[i];
	}

	if (server.isRegistered(user_info->username))
		return ERR_ALREADYREGISTERED;

	if (user_info->username.length() > USERLEN)
		user_info->username = user_info->username.substr(0, USERLEN);

	if (user_info->username == "0" || user_info->realname == "*" || user_info->username.empty() || user_info->realname.empty())
		user_info->username = client.getNickName();

	return (EXIT_SUCCESS);
}
