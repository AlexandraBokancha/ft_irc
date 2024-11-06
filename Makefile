NAME = ircserv

CC = c++

CFLAGS = -std=c++98 -Wall -Wextra -Werror -Wconversion -g3

SRCS =  main.cpp \
		Server.cpp \
		Client.cpp

OBJS = $(SRCS:.cpp=.o)


%.o: %.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(NAME)

clean: 
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re