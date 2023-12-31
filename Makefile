CC = gcc

CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g

LIBS = -lreadline

SRC = main.c split_cmd_line.c ft_utils.c env_vars.c exe.c ft_split.c expand.c

OUT = minishell

all:
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(OUT)

clean:
	rm -f $(OUT)

.PHONY: all clean
