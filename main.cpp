#include "include/Server.hpp"

bool checkPort(std::string port){
	return (port.find_first_not_of("0123456789") == std::string::npos && \
	std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) <= 65535); // Ports 0 to 1023 are reserved
}

int main(int ac, char **av)
{
	if (ac != 3) {
		std::cout << "Incorrect number of arguments, specify PORT and PASSWORD" << std::endl;
		return (-1);
	}
	if (!checkPort(av[1]) || !*av[2]) {
		std::cout << "Invalid PORT or PASSWORD" << std::endl;
		return (-1);
	}
	Server serv(std::atoi(av[1]), av[2]);
	//std::cout << INTRO;
	try
	{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		signal(SIGPIPE, SIG_IGN);
		serv.Launch();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}