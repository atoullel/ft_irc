#pragma once

#define CRLF "\r\n"

#define RPL_CONNECTED(nickname) (": 001 " + nickname + " : Welcome to the IRC server!" + CRLF)
#define RPL_UMODEIS(hostname, Salonname, mode, user)  ":" + hostname + " MODE " + Salonname + " " + mode + " " + user + CRLF
#define RPL_CREATIONTIME(nickname, Salonname, creationtime) ": 329 " + nickname + " #" + Salonname + " " + creationtime + CRLF
#define RPL_SALONMODES(nickname, Salonname, modes) ": 324 " + nickname + " #" + Salonname + " " + modes + CRLF
#define RPL_CHANGEMODE(hostname, Salonname, mode, arguments) (":" + hostname + " MODE #" + Salonname + " " + mode + " " + arguments + CRLF)
#define RPL_NICKCHANGE(oldnickname, nickname) (":" + oldnickname + " NICK " + nickname + CRLF)
#define RPL_JOINMSG(hostname, ipaddress, Salonname) (":" + hostname + "@" + ipaddress + " JOIN #" + Salonname + CRLF)
#define RPL_NAMREPLY(nickname, Salonname, clientslist) (": 353 " + nickname + " @ #" + Salonname + " :" + clientslist + CRLF)
#define RPL_ENDOFNAMES(nickname, Salonname) (": 366 " + nickname + " #" + Salonname + " :END of /NAMES list" + CRLF)
#define RPL_TOPICIS(nickname, Salonname, topic) (": 332 " + nickname + " #" +Salonname + " :" + topic + "\r\n")

///////// ERRORS ////////////////
#define ERR_NEEDMODEPARM(Salonname, mode) (": 696 #" + Salonname + " * You must specify a parameter for the key mode. " + mode + CRLF)
#define ERR_INVALIDMODEPARM(Salonname, mode) ": 696 #" + Salonname + " Invalid mode parameter. " + mode + CRLF
#define ERR_KEYSET(Salonname) ": 467 #" + Salonname + " Salon key already set. " + CRLF
#define ERR_UNKNOWNMODE(nickname, Salonname, mode) ": 472 " + nickname + " #" + Salonname + " " + mode + " :is not a recognised Salon mode" + CRLF
#define ERR_NOTENOUGHPARAM(nickname) (": 461 " + nickname + " :Not enough parameters." + CRLF)
#define ERR_SalonNOTFOUND(nickname, Salonname) (": 403 " + nickname + " " + Salonname + " :No such Salon" + CRLF)
#define ERR_NOTOPERATOR(Salonname) (": 482 #" + Salonname + " :You're not a Salon operator" + CRLF)
#define ERR_NOSUCHNICK(Salonname, name) (": 401 #" + Salonname + " " + name + " :No such nick/Salon" + CRLF )
#define ERR_INCORPASS(nickname) (": 464 " + nickname + " :Password incorrect !" + CRLF )
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You may not reregister !" + CRLF )
#define ERR_NONICKNAME(nickname) (": 431 " + nickname + " :No nickname given" + CRLF )
#define ERR_NICKINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + CRLF)
#define ERR_ERRONEUSNICK(nickname) (": 432 " + nickname + " :Erroneus nickname" + CRLF)
#define ERR_NOTREGISTERED(nickname) (": 451 " + nickname + " :You have not registered!" + CRLF)
#define ERR_CMDNOTFOUND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command" + CRLF)
