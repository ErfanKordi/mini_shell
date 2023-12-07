/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarabin <amarabin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 07:43:24 by amarabin          #+#    #+#             */
/*   Updated: 2023/12/04 04:21:56 by amarabin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ioctl.h>
# include <sys/types.h>
# include <unistd.h>

typedef struct s_token
{
	bool			is_cmd;
	bool			is_option;
	bool			is_param;
	bool			is_input_redirection;
	bool			is_output_redirection;
	bool			is_pipe;
	bool			is_append_mode;
	bool			is_background;
	bool			is_cmd_separator;
	bool			is_escaped;
	bool			is_quoted;
	char			*value;
}					t_token;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}					t_env;

t_token				**split_cmd_line(char *s, t_env *env_var_list);
void				free_token_matrix(t_token **tokens);

char				*ft_strdup(const char *s);
void				ft_putchar_fd(char c, int fd);
void				ft_putstr_fd(const char *s, int fd, bool newline);
size_t				ft_strlen(const char *s);
int					ft_strncmp(char *s1, char *s2, size_t n);
char				*ft_strtrim(char const *s1, char const *set);
char				*ft_strjoin(char const *s1, char const *s2);
char				*ft_strchr(const char *s, int c);
char				*ft_strgetbetween(const char *start, const char *end);

char				**prepare_enelope_for_execve(t_env *head);
int					unset_env(t_env **head, char *var);
int					set_env(t_env **head, char *key_val_str);
char				*get_env(t_env *head, const char *key);
void				free_env_var_list(t_env *head);
t_env				*init_env_var(char *envp[]);
void				print_env_var_list(t_env *head);
#endif
