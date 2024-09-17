NAME = ircserv

PORT = 8080

PASS = 1234

IP = 127.0.0.1

# Colors
RED = \033[31m
BLUE = \033[34m
GREEN = \033[32m
RESET = \033[0m

# Path

INC_PATH = ./incs/
SRC_PATH = ./srcs/
OBJ_PATH = ./objs/
CLASSES_PATH = classes/

# Files

TESTS = test.cpp

CLASSES = Server.cpp Client.cpp Response.cpp Error.cpp Channel.cpp
CLASSES := $(addprefix $(CLASSES_PATH), $(CLASSES))

SRCS = main.cpp parsing.cpp signal.cpp $(CLASSES)
SRCS := $(addprefix $(SRC_PATH), $(SRCS))

OBJS := $(subst $(SRC_PATH), $(OBJ_PATH), $(SRCS:.cpp=.o))
DEPS := $(OBJS:.o=.d)

# Flags

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CXXFLAGS += -MMD -I $(INC_PATH)
CXXFLAGS += -g3 -fsanitize=address -fsanitize=leak -fsanitize=undefined
CXXFLAGS += -D IP=\"$(IP)\"

# Rules

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) created$(RESET) $(BLUE)✓$(RESET)"

run: $(NAME)
	./$(NAME) $(PORT) $(PASS)

clean:
	@rm -rf $(OBJ_PATH) $(DEPS) $(OBJS)
	@echo "$(RED)$(NAME) objects deleted$(RESET) $(BLUE)✓$(RESET)"

fclean: clean
	@rm -rf $(NAME)
	@echo "$(RED)$(NAME) deleted$(RESET) $(BLUE)✓$(RESET)"

re: fclean all

.PHONY: all clean fclean re run test

# Pattern rules

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)
