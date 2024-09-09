#ifndef FT_IRC_PARSE_H
# define FT_IRC_PARSE_H

#include "Server.hpp"
#include "Commande.hpp"
#include <vector>
#include <string>

struct CommandHandler {
    std::string command;
    void (Commande::*handler)(Server*, std::string&, int);
};

//split pour recuperer la commande
std::vector<std::string> splitMessage(std::string input);
std::vector<std::string> splitCommand(std::string& cmd);
//le gros switch qui lance les commandes
void parsing_switch(std::string &cmd, int fd);


#endif