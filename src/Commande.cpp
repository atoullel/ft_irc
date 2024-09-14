#include "../include/Commande.hpp"
#include "../include/Server.hpp"

// (PASS)
void Commande::authenticateClient(Server* server, std::string cmd, int clientFd) {
}

// (NICK)
void Commande::setNick(Server* server, std::string cmd, int clientFd) {

}

// (USER)
void Commande::setUser(Server* server, std::string cmd, int clientFd) {

}

// (QUIT)
void Commande::quitClient(Server* server, std::string cmd, int clientFd) {

}

// (KICK)
void Commande::kickClient(Server* server, std::string cmd, int clientFd) {

}

// (JOIN)
void Commande::joinChannel(Server* server, std::string cmd, int clientFd) {

}

// (TOPIC)
void Commande::changeTopic(Server* server, std::string cmd, int clientFd) {

}

//  (MODE)
void Commande::changeMode(Server* server, std::string cmd, int clientFd) {

}

// (PART)
void Commande::leaveChannel(Server* server, std::string cmd, int clientFd) {

}

//  (PRIVMSG)
void Commande::privateMessage(Server* server, std::string cmd, int clientFd) {

}

// (INVITE)
void Commande::inviteClient(Server* server, std::string cmd, int clientFd) {

}