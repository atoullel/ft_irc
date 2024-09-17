#ifndef ERROR_HPP
# define ERROR_HPP

# include <cerrno>
# include <cstring>
# include <exception>
# include <map>
# include "colors.hpp"
# include "struct.hpp"

class Client;
class Server;

class Error : public std::exception
{
	private:
		std::map<int, void (Error::*)(Client &, Server &, t_request &)>	_error;
		std::string		_errorMessage;

		void _errUnknownError(Client &, Server &, t_request &);
		void _errNoSuchNick(Client &, Server &, t_request &);
		void _errNoSuchServer(Client &, Server &, t_request &);
		void _errNoSuchChannel(Client &, Server &, t_request &);
		void _errCannotSendToChan(Client &, Server &, t_request &);
		void _errTooManyChannels(Client &, Server &, t_request &);
		void _errWasNoSuchNick(Client &, Server &, t_request &);
		void _errNoOrigin(Client &, Server &, t_request &);
		void _errNoRecipient(Client &, Server &, t_request &);
		void _errNoTextToSend(Client &, Server &, t_request &);
		void _errInputTooLong(Client &, Server &, t_request &);
		void _errUnknownCommand(Client &, Server &, t_request &);
		void _errNoMotd(Client &, Server &, t_request &);
		void _errNoNicknameGiven(Client &, Server &, t_request &);
		void _errErroneusNickname(Client &, Server &, t_request &);
		void _errNicknameInUse(Client &, Server &, t_request &);
		void _errNickCollision(Client &, Server &, t_request &);
		void _errUserNotInChannel(Client &, Server &, t_request &);
		void _errNotOnChannel(Client &, Server &, t_request &);
		void _errUserOnChannel(Client &, Server &, t_request &);
		void _errNotRegistered(Client &, Server &, t_request &);
		void _errNeedMoreParams(Client &, Server &, t_request &);
		void _errAlreadyRegistered(Client &, Server &, t_request &);
		void _errPasswdMismatch(Client &, Server &, t_request &);
		void _errYoureBannedCreep(Client &, Server &, t_request &);
		void _errChannelIsFull(Client &, Server &, t_request &);
		void _errUnknownMode(Client &, Server &, t_request &);
		void _errInviteOnlyChan(Client &, Server &, t_request &);
		void _errBannedFromChan(Client &, Server &, t_request &);
		void _errBadChannelKey(Client &, Server &, t_request &);
		void _errBadChanMask(Client &, Server &, t_request &);
		void _errNoPrivileges(Client &, Server &, t_request &);
		void _errChanOpPrivsNeeded(Client &, Server &, t_request &);
		void _errCantKillServer(Client &, Server &, t_request &);
		void _errNoOperHost(Client &, Server &, t_request &);
		void _errUModeUnknownFlag(Client &, Server &, t_request &);
		void _errUsersDontMatch(Client &, Server &, t_request &);
		void _errHelpNotFound(Client &, Server &, t_request &);
		void _errInvalidKey(Client &, Server &, t_request &);
		void _errStartTLS(Client &, Server &, t_request &);
		void _errInvalidModeParam(Client &, Server &, t_request &);
		void _errNoPrivs(Client &, Server &, t_request &);
		void _errNickLocked(Client &, Server &, t_request &);
		void _errSASLFail(Client &, Server &, t_request &);
		void _errSASLTooLong(Client &, Server &, t_request &);
		void _errSASLAborted(Client &, Server &, t_request &);
		void _errSASLAlready(Client &, Server &, t_request &);
	public:
		Error(const char *str = "") throw();
        virtual ~Error(void) throw();
		void	errorFunction(const char *str);
		void	sendErrorMessages(Client &client, const char *message);
		void	errorRequest(int error, Client &client, Server &server, t_request &request);
		virtual const char* what() const throw();
};

enum e_error
{
	ERR_UNKNOWNERROR = 400,
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHSERVER = 402,
	ERR_NOSUCHCHANNEL = 403,
	ERR_CANNOTSENDTOCHAN = 404,
	ERR_TOOMANYCHANNELS = 405,
	ERR_WASNOSUCHNICK = 406,
	ERR_NOORIGIN = 409,
	ERR_NORECIPIENT = 411,
	ERR_NOTEXTTOSEND = 412,
	ERR_INPUTTOOLONG = 417,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NOMOTD = 422,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NICKCOLLISION = 436,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_YOUREBANNEDCREEP = 465,
	ERR_CHANNELISFULL = 471,
	ERR_UNKNOWNMODE = 472,
	ERR_INVITEONLYCHAN = 473,
	ERR_BANNEDFROMCHAN = 474,
	ERR_BADCHANNELKEY = 475,
	ERR_BADCHANMASK = 476,
	ERR_NOPRIVILEGES = 481,
	ERR_CHANOPRIVSNEEDED = 482,
	ERR_CANTKILLSERVER = 483,
	ERR_NOOPERHOST = 491,
	ERR_UMODEUNKNOWNFLAG = 501,
	ERR_USERSDONTMATCH = 502,
	ERR_HELPNOTFOUND = 524,
	ERR_INVALIDKEY = 525,
	ERR_INVALIDMODEPARAM = 696,
	ERR_NOPRIVS = 723,
	ERR_NICKLOCKED = 902,
};

#endif
