#include "libraries.hpp"
#include "Server.hpp"

int	main(int ac, char **av)
{
	if (parsing(ac, av) == EXIT_FAILURE)
		return (EXIT_FAILURE);

	unsigned short	port = atoi(av[1]);
	std::string		password = av[2];
	Server			*ircServer = new Server(port, password);
	t_sigaction		sa;

	bzero(&sa, sizeof(t_sigaction));
	saveOrDestroyServer(ircServer, SAVE);
	signal(SIGTERM, SIG_IGN);
	signal(SIGSTOP, SIG_IGN);
	signal(SIGINT, sa.sa_handler = killServer);
	signal(SIGQUIT, sa.sa_handler = killServer);
	signal(SIGABRT, sa.sa_handler = killServer);
	std::cout << BLUE << "Waiting for connections..." << RESET << std::endl;
	while (true)
	{
		try
		{
			ircServer->start();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	return (EXIT_SUCCESS);
}
