#include "../include/Parse.hpp"
#include "../include/Server.hpp"
#include "../include/Commande.hpp"

static const CommandHandler commandTable[] = {
	{ "PASS", &Commande::authenticateClient },
	{ "NICK", &Commande::setNick },
	{ "USER", &Commande::setUser },
	{ "QUIT", &Commande::quitClient },
	{ "KICK", &Commande::kickClient },
	{ "JOIN", &Commande::joinChannel },
	{ "TOPIC", &Commande::changeTopic }, //BONUS?
	{ "MODE", &Commande::changeMode },
	{ "PART", &Commande::leaveChannel },
	{ "PRIVMSG", &Commande::privateMessage },
	{ "INVITE", &Commande::inviteClient }
};

static const size_t commandCount = sizeof(commandTable) / sizeof(CommandHandler);

void parsing_switch(Server* server, std::string& cmd, int clientFd){
    if (cmd.empty())
        return;

    std::vector<std::string> parsedCommand = splitCommand(cmd);
    if (parsedCommand.empty())
        return;

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
            (commandTable[i].handler)(server, cmd, clientFd);
            return;
        }
    }

    // Gestion de la commande non trouvée ou client non enregistré(plein de fontions encore a coder)
    if (server->isClientRegistered(clientFd)) {
        server->sendErrorResponse(server->getErrorUnknownCommand(server->getClientNick(clientFd), parsedCommand[0]), clientFd);
    } else {
        server->sendErrorResponse(server->getErrorNotRegistered("*"), clientFd);
    }
}

std::vector<std::string> splitMessage(std::string input){
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

std::vector<std::string> splitCommand(std::string& cmd){
    std::vector<std::string> tokens;
    std::istringstream stream(cmd);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
        token.clear();
    }
    return tokens;
}
