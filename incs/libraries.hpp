#ifndef LIBRARIES_HPP
# define LIBRARIES_HPP

# include <iostream>
# include <cstdlib>
# include <cstring>
# include <cerrno>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/stat.h>
# include <sys/select.h>
# include <algorithm>
# include <sstream>
# include <vector>
# include <iterator>
# include "colors.hpp"
# include "struct.hpp"

class Server;
class Client;

/**
 * @brief Parses the arguments passed to the main function and checks if the port is in the correct range.
 *
 * @param ac The number of arguments passed to the main function.
 * @param av The array of arguments passed to the main function.
 * @return int Returns EXIT_SUCCESS if the parsing is successful, otherwise returns EXIT_FAILURE.
 */
int		parsing(int ac, char **av);

void 	saveOrDestroyServer(Server *server, int mode);

void	killServer(int sig_num);

std::string getChannelName(std::vector<std::string> &params);

#endif
