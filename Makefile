CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
NAME = codexion
SRC_DIR = code_new
SRCS = clean_all.c coder.c codexion.c dongle.c heap.c heap2.c init.c manger.c monitor.c parsing.c starter.c state.c time.c utils.c
OBJS = $(addprefix $(SRC_DIR)/,$(SRCS:.c=.o))
HEADERS = $(SRC_DIR)/codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(SRC_DIR)/codexion $(SRC_DIR)/a.out

re: fclean all

.PHONY: all clean fclean re
