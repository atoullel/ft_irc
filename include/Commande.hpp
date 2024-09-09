#ifndef COMMANDE_HPP
#define COMMANDE_HPP

#include <string>
#include "Server.hpp"

class Commande
{
public:
    void authenticateClient(Server* server, std::string cmd, int clientFd);
    void setNick(Server* server, std::string cmd, int clientFd);
    void setUser(Server* server, std::string cmd, int clientFd);
    void quitClient(Server* server, std::string cmd, int clientFd);
    void kickClient(Server* server, std::string cmd, int clientFd);
    void joinChannel(Server* server, std::string cmd, int clientFd);
    void changeTopic(Server* server, std::string cmd, int clientFd);  // BONUS
    void changeMode(Server* server, std::string cmd, int clientFd);
    void leaveChannel(Server* server, std::string cmd, int clientFd);
    void privateMessage(Server* server, std::string cmd, int clientFd);
    void inviteClient(Server* server, std::string cmd, int clientFd);
};
#endif