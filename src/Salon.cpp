#include "../include/Salon.hpp"
#include <ctime>

Salon::Salon(){
	this->invit_only = 0;
	this->topic = 0;
	this->key = 0;
	this->limit = 0;
	this->topic_restriction = false;
	this->name = "";
	this->topic_name = "";
	char charaters[5] = {'i', 't', 'k', 'o', 'l'};
	for(int i = 0; i < 5; i++)
		modes.push_back(std::make_pair(charaters[i],false));
	this->created_at = "";
}
Salon::~Salon(){}
Salon::Salon(Salon const &src){*this = src;}
Salon &Salon::operator=(Salon const &src){
	if (this != &src){
		this->invit_only = src.invit_only;
		this->topic = src.topic;
		this->key = src.key;
		this->limit = src.limit;
		this->topic_restriction = src.topic_restriction;
		this->name = src.name;
		this->password = src.password;
		this->created_at = src.created_at;
		this->topic_name = src.topic_name;
		this->clients = src.clients;
		this->admins = src.admins;
		this->modes = src.modes;
	}
	return *this;
}
//---------------//Setters
void Salon::SetInvitOnly(int invit_only){this->invit_only = invit_only;}
void Salon::SetTopic(int topic){this->topic = topic;}
void Salon::SetTime(std::string time){this->time_creation = time;}
void Salon::SetKey(int key){this->key = key;}
void Salon::SetLimit(int limit){this->limit = limit;}
void Salon::SetTopicName(std::string topic_name){this->topic_name = topic_name;}
void Salon::SetPassword(std::string password){this->password = password;}
void Salon::SetName(std::string name){this->name = name;}
void Salon::set_topicRestriction(bool value){this->topic_restriction = value;}
void Salon::setModeAtindex(size_t index, bool mode){modes[index].second = mode;}
void Salon::set_createiontime(){
	std::time_t _time = std::time(NULL);
	std::ostringstream oss;
	oss << _time;
	this->created_at = std::string(oss.str());
}
//---------------//Setters
//---------------//Getters
int Salon::GetInvitOnly(){return this->invit_only;}
int Salon::GetTopic(){return this->topic;}
int Salon::GetKey(){return this->key;}
int Salon::GetLimit(){return this->limit;}
int Salon::GetClientsNumber(){return this->clients.size() + this->admins.size();}
bool Salon::Gettopic_restriction() const{return this->topic_restriction;}
bool Salon::getModeAtindex(size_t index){return modes[index].second;}
bool Salon::clientInSalon(std::string &nick){
	for(size_t i = 0; i < clients.size(); i++){
		if(clients[i].GetNickName() == nick)
			return true;
	}
	for(size_t i = 0; i < admins.size(); i++){
		if(admins[i].GetNickName() == nick)
			return true;
	}
	return false;
}
std::string Salon::GetTopicName(){return this->topic_name;}
std::string Salon::GetPassword(){return this->password;}
std::string Salon::GetName(){return this->name;}
std::string Salon::GetTime(){return this->time_creation;}
std::string Salon::get_creationtime(){return created_at;}
std::string Salon::getModes(){
	std::string mode;
	for(size_t i = 0; i < modes.size(); i++){
		if(modes[i].first != 'o' && modes[i].second)
			mode.push_back(modes[i].first);
	}
	if(!mode.empty())
		mode.insert(mode.begin(),'+');
	return mode;
}
std::string Salon::clientSalon_list(){
	std::string list;
	for(size_t i = 0; i < admins.size(); i++){
		list += "@" + admins[i].GetNickName();
		if((i + 1) < admins.size())
			list += " ";
	}
	if(clients.size())
		list += " ";
	for(size_t i = 0; i < clients.size(); i++){
		list += clients[i].GetNickName();
		if((i + 1) < clients.size())
			list += " ";
	}
	return list;
}
Client *Salon::get_client(int fd){
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->GetFd() == fd)
			return &(*it);
	}
	return NULL;
}
Client *Salon::get_admin(int fd){
	for (std::vector<Client>::iterator it = admins.begin(); it != admins.end(); ++it){
		if (it->GetFd() == fd)
			return &(*it);
	}
	return NULL;
}
Client* Salon::GetClientInSalon(std::string name)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->GetNickName() == name)
			return &(*it);
	}
	for (std::vector<Client>::iterator it = admins.begin(); it != admins.end(); ++it){
		if (it->GetNickName() == name)
			return &(*it);
	}
	return NULL;
}
//---------------//Getters
//---------------//Methods
void Salon::add_client(Client newClient){clients.push_back(newClient);}
void Salon::add_admin(Client newClient){admins.push_back(newClient);}
void Salon::remove_client(int fd){
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->GetFd() == fd)
			{clients.erase(it); break;}
	}
}
void Salon::remove_admin(int fd){
	for (std::vector<Client>::iterator it = admins.begin(); it != admins.end(); ++it){
		if (it->GetFd() == fd)
			{admins.erase(it); break;}
	}
}
bool Salon::change_clientToAdmin(std::string& nick){
	size_t i = 0;
	for(; i < clients.size(); i++){
		if(clients[i].GetNickName() == nick)
			break;
	}
	if(i < clients.size()){
		admins.push_back(clients[i]);
		clients.erase(i + clients.begin());
		return true;
	}
	return false;
}

bool Salon::change_adminToClient(std::string& nick){
	size_t i = 0;
	for(; i < admins.size(); i++){
		if(admins[i].GetNickName() == nick)
			break;
	}
	if(i < admins.size()){
		clients.push_back(admins[i]);
		admins.erase(i + admins.begin());
		return true;
	}
	return false;

}
//---------------//Methods
//---------------//SendToAll
void Salon::sendTo_all(std::string rpl1)
{
	for(size_t i = 0; i < admins.size(); i++)
		if(send(admins[i].GetFd(), rpl1.c_str(), rpl1.size(),0) == -1)
			std::cerr << "send() faild" << std::endl;
	for(size_t i = 0; i < clients.size(); i++)
		if(send(clients[i].GetFd(), rpl1.c_str(), rpl1.size(),0) == -1)
			std::cerr << "send() faild" << std::endl;
}
void Salon::sendTo_all(std::string rpl1, int fd)
{
	for(size_t i = 0; i < admins.size(); i++){
		if(admins[i].GetFd() != fd)
			if(send(admins[i].GetFd(), rpl1.c_str(), rpl1.size(),0) == -1)
				std::cerr << "send() faild" << std::endl;
	}
	for(size_t i = 0; i < clients.size(); i++){
		if(clients[i].GetFd() != fd)
			if(send(clients[i].GetFd(), rpl1.c_str(), rpl1.size(),0) == -1)
				std::cerr << "send() faild" << std::endl;
	}
}
//---------------//SendToAll