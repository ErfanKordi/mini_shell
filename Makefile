# Define the compiler
CC = gcc

# Define any compile-time flags
CFLAGS = -Wall -Wextra -Werror

# Define the libraries to link against
LIBS = -lreadline

# Define the source files
SRC = main.c split_cmd_line.c ft_utils.c env_vars.c

# Define the name of the executable
OUT = minishell

# Default target
all:
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(OUT)

# Clean target to remove generated files
clean:
	rm -f $(OUT)

# Phony targets
.PHONY: all clean
