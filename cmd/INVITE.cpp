#include "../include/Server.hpp"
/*
RPL_INVITING (341)
ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHSalon (403)
ERR_NOTONSalon (442)
ERR_CHANOPRIVSNEEDED (482)
ERR_USERONSalon (443)*/
void Server::inviteClient(std::string cmd, int clientFd)
{
	std::vector<std::string> scmd = split_cmd(cmd);
	if(scmd.size() < 3)// ERR_NEEDMOREPARAMS (461) if there are not enough parameters
		{senderror(461, GetClient(clientFd)->GetNickName(), clientFd, " :Not enough parameters\r\n"); return;}
	std::string Salonname = scmd[2].substr(1);
	if(scmd[2][0] != '#' || !GetSalon(Salonname))// ERR_NOSUCHSalon (403) if the given Salon does not exist
	    {senderror(403, Salonname, clientFd, " :No such Salon\r\n"); return;}
	if (!(GetSalon(Salonname)->get_client(clientFd)) && !(GetSalon(Salonname)->get_admin(clientFd)))// ERR_NOTONSalon (442) if the client is not on the Salon
	    {senderror(442, Salonname, clientFd, " :You're not on that Salon\r\n"); return;}
	if (GetSalon(Salonname)->GetClientInSalon(scmd[1]))// ERR_USERONSalon (443) if the given nickname is already on the Salon
	    {senderror(443, GetClient(clientFd)->GetNickName(), Salonname, clientFd, " :is already on Salon\r\n"); return;}
	Client *clt = GetClientNick(scmd[1]);
	if (!clt)// ERR_NOSUCHNICK (401) if the given nickname is not found
		{senderror(401, scmd[1], clientFd, " :No such nick\r\n");return;}
	if (GetSalon(Salonname)->GetInvitOnly() && !GetSalon(Salonname)->get_admin(clientFd))// ERR_INVITEONLYCHAN (473) if the Salon is invite-only
		{senderror(482,GetSalon(Salonname)->get_client(clientFd)->GetNickName(),scmd[1],clientFd," :You're not Salon operator\r\n"); return;}
	if (GetSalon(Salonname)->GetLimit() && GetSalon(Salonname)->GetClientsNumber() >= GetSalon(Salonname)->GetLimit()) // ERR_SalonISFULL (471) if the Salon is full
		{senderror(473,GetSalon(Salonname)->get_client(clientFd)->GetNickName(),Salonname,clientFd," :Cannot invit to Salon (+i)\r\n"); return;}
	// RPL_INVITING (341) if the invite was successfully sent
	clt->AddSalonlInvite(Salonname);
	std::string rep1 = ": 341 "+ GetClient(clientFd)->GetNickName()+" "+ clt->GetNickName()+" "+ scmd[2]+"\r\n";
	_sendResponse(rep1, clientFd);
	std::string rep2 = ":"+ clt->getHostname() + " INVITE " + clt->GetNickName() + " " + scmd[2]+"\r\n";
	_sendResponse(rep2, clt->GetFd());
}
