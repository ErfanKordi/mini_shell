/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekordi <ekordi@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 07:46:42 by amarabin          #+#    #+#             */
/*   Updated: 2023/12/11 12:18:26 by ekordi           ###   ########.fr       */
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

char **argv_generator(t_token **cmd, int start_index, t_env *env_var_list)
{
	int	arg_count;
	int	i;
	char **argv;

	i = start_index + 1;
	arg_count = 0;
	while (cmd[i] != NULL && cmd[i]->is_param) {
		if (expand_token(cmd[i], env_var_list))
			arg_count++;
		i++;
	}
	argv = malloc((arg_count + 2) * sizeof(char *));
	if (!argv)
		return (0);
	argv[0] = cmd[start_index]->value;
	i = 0;
	while (i++ < arg_count)
		argv[i] = cmd[start_index + i]->value;
	argv[arg_count + 1] = NULL;
	return (argv);
}
int prepare_and_execute(t_token **cmd, int start_index, t_env *env_var_list)
{
	int i;
	i = 0;
	char **argv = argv_generator(cmd, start_index, env_var_list);
	//count the cmd numbers
	int	nb_cmd = 0;
	while (cmd[i])
	{
		if (cmd[i]->is_cmd)
			nb_cmd++;
		i++;
	}
	if (nb_cmd == 1)
	{
		char	*path;
		int pid1;
		path = cmd_path(cmd[start_index]->value, env_var_list);
		if (path == NULL)
			ft_putstr_fd("Path Error", 2, true);
		//execute(cmd[start_index]->value, argv,  env_var_list);
		pid1 = fork();
		if (pid1 < 0)
		{
			perror("fork");
			exit(1);
		}
		if (pid1 == 0)
		{
			if (execve(path, argv, get_env_for_exe(env_var_list)) == -1)
				ft_putstr_fd("Exec Error", 2, true);
		}
		waitpid(pid1, 0, 0);
	}
	else
	{
		int original_stdout[] = {dup(STDOUT_FILENO), dup(STDIN_FILENO)};
		i = 0;
		while (i < nb_cmd - 1)
		{
			execute(cmd[start_index]->value, argv, env_var_list, false, original_stdout);
			while (cmd[++start_index])
			{
				if (cmd[start_index]->is_cmd)
					break;
			}
			free(argv);
			argv = argv_generator(cmd, start_index, env_var_list);
			i++;
		}
		execute(cmd[start_index]->value, argv, env_var_list, true, original_stdout);
	}
	free(argv);
	//return (start_index + arg_count + 1);
	return 1;
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
				if (cmd[i + 1] && cmd[i + 1]->is_param && \
						!set_env(&env_var_list, cmd[i + 1]->value))
					return (-1);
				i += 2;
			}
			else if (!ft_strncmp(cmd[i]->value, "unset", 5)
				&& ft_strlen(cmd[i]->value) == 5)
			{
				if (cmd[i + 1] && cmd[i + 1]->is_param)
					unset_env(&env_var_list, cmd[i + 1]->value);
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
			{
				prepare_and_execute(cmd, i, env_var_list);
				return 1;
				// if(!i)
				// 	return (-1);
			}
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
