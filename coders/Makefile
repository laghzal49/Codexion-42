# ==============================================================================
#                                 VARIABLES
# ==============================================================================

# Program executable name
NAME        = codexion

# Compiler and flags (-I. looks in current dir, -Igarbge_colle looks in your custom dir)
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
              garbge_colle/ft_malloc.c

# Object files
OBJS        = $(SRCS:.c=.o)

# Header files (Used as dependencies so changes trigger recompilation)
HEADERS     = codexion.h garbge_colle/ft_malloc.h

# ==============================================================================
#                                   RULES
# ==============================================================================

# Default rule to build the executable
all: $(NAME)

# Link object files to create the executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

# Compile .c files into .o files
# $< is the source file, $@ is the target object file
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Remove object files
clean:
	rm -f $(OBJS)

# Remove object files and the executable
fclean: clean
	rm -f $(NAME)

# Completely recompile from scratch
re: fclean all

# Phony targets to prevent Make from confusing these with actual files
.PHONY: all clean fclean re
