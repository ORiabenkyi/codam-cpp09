NAME    = PmergeMe
CC      = c++
CCFLAG  = -Wall -Wextra -Werror -std=c++98 -I inc

SRCS    = src/PmergeMe.cpp \
          src/main.cpp

HEADS   = inc/PmergeMe.hpp

OBJS    = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CCFLAG) $(OBJS) -o $(NAME)

%.o: %.cpp $(HEADS)
	$(CC) $(CCFLAG) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
