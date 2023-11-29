/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarabin <amarabin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 07:46:42 by amarabin          #+#    #+#             */
/*   Updated: 2023/11/29 10:19:11 by amarabin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_signal(int sig)
{
	if (sig == SIGINT)
	{
		ft_putstr_fd("\n", STDOUT_FILENO, true);
		rl_on_new_line();
		rl_replace_line("", 0);
	}
}

/**
 * 0 = memory prot failure
 * 1 = no errors
 */
int	execute_cd(t_token **cmd, int i)
{
	char	*tmp;
	char	*tmp2;

	if (cmd[i] && cmd[i]->is_param)
	{
		if (cmd[i]->value[0] == '~')
		{
			if (chdir(cmd[i]->value))
				ft_putstr_fd("Error\n", STDERR_FILENO, false);
			return (1);
		}
		else
		{
			tmp = getcwd(NULL, 0);
			if (tmp == NULL)
				return (0);
			tmp2 = ft_strjoin(tmp, "/");
			free(tmp);
			if (tmp2 == NULL)
				return (0);
			tmp = ft_strjoin(tmp2, cmd[i]->value);
			free(tmp2);
			if (tmp == NULL)
				return (0);
			if (chdir(tmp) != 0)
				ft_putstr_fd("Error\n", STDERR_FILENO, false);
			return (free(tmp), 1);
		}
	}
	else if (chdir(getenv("HOME")))
		ft_putstr_fd("Error\n", STDERR_FILENO, false);
	return (1);
}

/**
 * 0 = memory prot failure
 * 1 = no errors
 */
int	execute_pwd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (ft_putstr_fd("Error\n", STDERR_FILENO, false), 0);
	ft_putstr_fd(pwd, STDOUT_FILENO, true);
	free(pwd);
	return (1);
}

int	execute_echo(t_token **cmd, int i)
{
	bool	newline;

	newline = true;
	while (cmd[i])
	{
		if (cmd[i]->is_option && !ft_strncmp(cmd[i]->value, "-n", 2))
			newline = false;
		else if (cmd[i]->is_param)
		{
			ft_putstr_fd(cmd[i]->value, STDOUT_FILENO, false);
			if (cmd[i + 1] && cmd[i + 1]->is_param)
				ft_putchar_fd(' ', 1);
			else
			{
				i++;
				break ;
			}
		}
		i++;
	}
	if (newline)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (i);
}

/**
 * 0 = exit
 * -1 = memory failure
 */
int	execute_cmd_line(char *line, t_env *env_var_list)
{
	t_token	**cmd;
	int		i;

	cmd = split_cmd_line(line);
	if (cmd == NULL)
		return (0);
	i = 0;
	while (cmd[i])
	{
		if (cmd[i]->is_cmd)
		{
			if (!ft_strncmp(cmd[i]->value, "echo", 4)
				&& ft_strlen(cmd[i]->value) == 4)
				i = execute_echo(cmd, i + 1);
			else if (!ft_strncmp(cmd[i]->value, "pwd", 3)
				&& ft_strlen(cmd[i]->value) == 3)
			{
				if (!execute_pwd())
					return (-1);
				i++;
			}
			else if (!ft_strncmp(cmd[i]->value, "cd", 2)
				&& ft_strlen(cmd[i]->value) == 2)
			{
				if (!execute_cd(cmd, i + 1))
					return (-1);
				i++;
				if (cmd[i] != NULL)
					i++;
			}
			else if (!ft_strncmp(cmd[i]->value, "export", 6)
				&& ft_strlen(cmd[i]->value) == 6)
			{
				if (cmd[i + 1] && cmd[i + 1]->is_param && !set_env(&env_var_list, cmd[i + 1]->value))
					return (-1);
				i += 2;
			}
			else if (!ft_strncmp(cmd[i]->value, "unset", 5)
				&& ft_strlen(cmd[i]->value) == 5)
			{
				if (cmd[i + 1] && cmd[i + 1]->is_param)
					unset_env(&env_var_list, cmd[i + 1]->value); //return value doesn't count here
				i += 2;
			}
			else if (!ft_strncmp(cmd[i]->value, "env", 3)
				&& ft_strlen(cmd[i]->value) == 3)
			{
				print_env_var_list(env_var_list);
				i++;
			}
			else if (!ft_strncmp(cmd[i]->value, "exit", 4))
				return (0);
			else
				return (1);
		}
	}
	free_token_matrix(cmd);
	return (i);
}

int	main(int argc, char **argv, char *envp[])
{
	char				*input;
	struct sigaction	sa_sigint;
	int					ret;
	t_env				*env_var_list;

	(void)argc;
	(void)argv;
	sa_sigint.sa_handler = handle_signal;
	sa_sigint.sa_flags = 0;
	sigaction(SIGINT, &sa_sigint, NULL);
	signal(SIGQUIT, SIG_IGN);
	env_var_list = init_env_var(envp);
	while (1)
	{
		input = readline("prompt> ");
		if (!input)
			break ;
		if (*input)
		{
			add_history(input);
			ret = execute_cmd_line(input, env_var_list);
			if (!ret)
				break ;
			else if (ret == -1)
			{
				// ///Mem prot Error ft_putstr_fd("Error\n...
				break ;
			}
		}
		free(input);
	}
	ft_putstr_fd("\nExiting shell...", STDOUT_FILENO, true);
	free_env_var_list(env_var_list);
	return (0);
}
