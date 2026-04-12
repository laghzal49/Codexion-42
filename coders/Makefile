NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread
INCLUDES    = -I. -Igarbge_colle

# Source files
SRCS        = codexion.c \
              init.c \
              simulation.c \
              coder_routine.c \
              monitor.c \
              time.c \
              pq.c \
              utils.c \
              garbge_colle/ft_malloc.c \
							dongle.c

OBJS        = $(SRCS:.c=.o)

HEADERS     = codexion.h garbge_colle/ft_malloc.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
