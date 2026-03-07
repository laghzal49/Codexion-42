CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
NAME = codexion
SRCS = coders/clean_all.c coders/coder.c coders/monitor.c coders/codexion.c coders/dongle.c \
	coders/dongle_helper.c coders/heap.c coders/heap_utils.c coders/init.c coders/parsing.c \
	coders/starter.c coders/state.c coders/time.c coders/utils.c \
	coders/ft_malloc.c
	
OBJS = $(SRCS:.c=.o)
HEADERS = coders/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

clean_all: fclean

re: fclean all

.PHONY: all clean fclean clean_all re
