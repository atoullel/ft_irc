#include "Server.hpp"

void Server::parsing_switch(std::string &cmd, int fd)
{
	if(cmd.empty())
		return ;
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	size_t found = cmd.find_first_not_of(" \t\v");
	if(found != std::string::npos)
		cmd = cmd.substr(found);
	if(splited_cmd.size() && (splited_cmd[0] == "BONG" || splited_cmd[0] == "bong"))
		return;
    if(splited_cmd.size() && (splited_cmd[0] == "PASS" || splited_cmd[0] == "pass"))
        client_authen(cmd, fd);
	else if (splited_cmd.size() && (splited_cmd[0] == "NICK" || splited_cmd[0] == "nick"))
		set_nickname(cmd,fd);
	else if(splited_cmd.size() && (splited_cmd[0] == "USER" || splited_cmd[0] == "user"))
		set_username(cmd, fd);
	else if (splited_cmd.size() && (splited_cmd[0] == "QUIT" || splited_cmd[0] == "quit"))
		quitClient(cmd,fd);
	else if(notregistered(fd))
	{
		if (splited_cmd.size() && (splited_cmd[0] == "KICK" || splited_cmd[0] == "kick"))
			kickClient(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "JOIN" || splited_cmd[0] == "join"))
			joinSalon(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "TOPIC" || splited_cmd[0] == "topic"))
			changeTopic(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "MODE" || splited_cmd[0] == "mode"))
			changeMode(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "PART" || splited_cmd[0] == "part"))
			leaveSalon(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "PRIVMSG" || splited_cmd[0] == "privmsg"))
			privateMessage(cmd, fd);
		else if (splited_cmd.size() && (splited_cmd[0] == "INVITE" || splited_cmd[0] == "invite"))
			inviteClient(cmd,fd);
		else if (splited_cmd.size())
			_sendResponse(ERR_CMDNOTFOUND(GetClient(fd)->GetNickName(),splited_cmd[0]),fd);
	}
	else if (!notregistered(fd))
		_sendResponse(ERR_NOTREGISTERED(std::string("*")),fd);
}

std::vector<std::string> Server::splitMessage(std::string input){
    std::vector<std::string> lines;
    std::istringstream stream(input);
    std::string line;
    while (std::getline(stream, line)) {
        size_t pos = line.find_first_of("\r\n");
        if (pos != std::string::npos)
            line = line.substr(0, pos);
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> Server::splitCommand(std::string& cmd){
    std::vector<std::string> tokens;
    std::istringstream stream(cmd);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
        token.clear();
    }
    return tokens;
}
