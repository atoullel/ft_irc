#ifndef STRUCT_HPP
# define STRUCT_HPP

# include "libraries.hpp"

# define MAX_CLIENTS 10
# define BUFFER_SIZE 1024
# define USERLEN 18
# define CHANNELLEN 20

# ifndef IP
#  define IP "127.0.0.1"
# endif

typedef struct sockaddr_in	s_sockaddr_in;
typedef struct sockaddr 	s_sockaddr;
typedef struct sigaction	t_sigaction;

typedef struct s_socket
{
	int							fd;
	s_sockaddr_in				addresse;
	socklen_t					addresse_len;
}								t_socket;

typedef struct s_fds
{
	fd_set						read;
	int							max;
}								t_fds;

typedef struct s_request
{
	std::string					raw;
	std::string					prefix;
	std::string					cmd;
	std::string					args;
	std::vector<std::string>	params;
}								t_request;

typedef struct s_user_info
{
	std::string					username;
	std::string					realname;
	std::string					hostname;
	std::string					servername;
}								t_user_info;

enum e_mode
{
	NORMAL,
	CLIENT_DISCONNECT
};

enum e_server_status
{
	SAVE,
	DESTROY
};

#endif
