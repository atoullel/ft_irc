NAME =          irc

CC =            c++

C++FLAGS =		-Wall -Wextra -Werror -std=c++98

INC =	include

SRC =	main.cpp\
		src/Server.cpp\
		src/Auth.cpp\
		src/Client.cpp\
		src/Salon.cpp\
		src/Parse.cpp\
		cmd/INVITE.cpp\
		cmd/JOIN.cpp\
		cmd/KICK.cpp\
		cmd/MODE.cpp\
		cmd/PART.cpp\
		cmd/PRIVMSG.cpp\
		cmd/QUIT.cpp\
		cmd/TOPIC.cpp\

OBJ = $(SRC:.cpp=.o)

all: $(NAME)
		@echo "Compiled !"

$(NAME): $(OBJ)
		@$(CC) $(C++FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	@$(CC) $(C++FLAGS) -I $(INC) -o $@ -c $<

clean:
				${RM} ${OBJ}

fclean:
				${RM} ${OBJ}
				rm -f ${NAME}

re:         	fclean all

.PHONY:        all clean fclean re
