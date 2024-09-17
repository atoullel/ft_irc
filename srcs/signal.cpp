#include "libraries.hpp"
#include "Server.hpp"

void saveOrDestroyServer(Server *server, int mode)
{
	static Server *save = NULL;

	if (mode == SAVE)
		save = server;
	else if (mode == DESTROY)
		delete save;
}

void	killServer(int sig_num)
{
	(void)sig_num;
	std::cout << std::endl;
	saveOrDestroyServer(NULL, DESTROY);
	exit(EXIT_SUCCESS);
}

