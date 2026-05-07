CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
NAME = codexion
SRCS = code_new/clean_all.c code_new/coder.c code_new/monitor.c code_new/codexion.c code_new/dongle.c code_new/dongle_helper.c code_new/heap.c code_new/heap2.c code_new/heap_utils.c code_new/init.c code_new/parsing.c code_new/starter.c code_new/state.c code_new/time.c code_new/utils.c
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
