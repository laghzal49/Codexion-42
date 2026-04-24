CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
NAME = codexion
SRCS = clean_all.c coder.c codexion.c dongle.c heap.c heap2.c init.c manger.c parsing.c simlition.c starter.c state.c time.c utils.c
OBJS = $(SRCS:.c=.o)
HEADERS = codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) a.out

clean_all: fclean

re: fclean all

.PHONY: all clean fclean clean_all re
