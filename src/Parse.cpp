#include "Server.hpp"


const CommandHandler commandTable[] = {
    { "PASS", &Server::client_authen },
    { "NICK", &Server::set_nickname },
    { "USER", &Server::set_username },
    { "QUIT", &Server::quitClient },
    { "KICK", &Server::kickClient },
    { "JOIN", &Server::joinSalon },
    { "TOPIC", &Server::changeTopic }, // BONUS?
    { "MODE", &Server::changeMode },
    { "PART", &Server::leaveSalon },
    { "PRIVMSG", &Server::privateMessage },
    { "INVITE", &Server::inviteClient }

};

static const size_t commandCount = sizeof(commandTable) / sizeof(commandTable[0]);

void Server::parsing_switch(std::string& cmd, int clientFd){
    if (cmd.empty())
        return;
    std::cout << cmd << std::endl;

    std::vector<std::string> parsedCommand = splitCommand(cmd);
    if (parsedCommand.empty())
        return;
    std::cout << parsedCommand.front() << std::endl;
    std::cout << parsedCommand.back() << std::endl;

    // Supprime les espaces en début de chaîne
    size_t found = cmd.find_first_not_of(" \t\v");
    if (found != std::string::npos)
        cmd = cmd.substr(found);

    // Ignore la commande "PING"
    if (parsedCommand[0] == "PING" || parsedCommand[0] == "ping")
        return;

    // Parcours la table des commandes et appel la fonction associée si la commande est trouvée
    for (size_t i = 0; i < commandCount; i++) {
        if (parsedCommand[0] == commandTable[i].command) {
            (this->*(commandTable[i].handler))(*this, cmd, clientFd);
            return;
        }
    }

    // Gestion de la commande non trouvée ou client non enregistré(plein de fontions encore a coder)
    if (notregistered(clientFd)) {
        _sendResponse(ERR_CMDNOTFOUND(this->_clients[clientFd].GetNickName(), parsedCommand[0]), clientFd);
    } else {
        _sendResponse(ERR_NOTREGISTERED(this->_clients[clientFd].GetNickName()), clientFd);
    }
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
