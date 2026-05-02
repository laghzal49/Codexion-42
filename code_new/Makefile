CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -fno-pie
NAME = codexion
SRCS = clean_all.c coder.c monitor.c codexion.c dongle.c heap.c heap2.c init.c parsing.c manger.c starter.c state.c time.c utils.c
OBJS = $(SRCS:.c=.o)
HEADERS = codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -no-pie -o $(NAME) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) a.out

clean_all: fclean

re: fclean all

.PHONY: all clean fclean clean_all re
