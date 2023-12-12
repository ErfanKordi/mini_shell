/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekordi <ekordi@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 07:46:42 by amarabin          #+#    #+#             */
/*   Updated: 2023/12/12 14:31:37 by ekordi           ###   ########.fr       */
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
	}	argv = malloc((arg_count + 2) * sizeof(char *));
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
	int		i;
	char	**argv;
	char	*path;
	int		pid1;
	int 	original_std_fd[2];

	original_std_fd[0] = dup(STDOUT_FILENO);
	original_std_fd[1] = dup(STDIN_FILENO);
	i = 0;
	argv = argv_generator(cmd, start_index, env_var_list);
	int	nb_cmd = 0;

	while (cmd[i])
	{
		if (cmd[i]->is_cmd)
			nb_cmd++;
		i++;
	}

	if (nb_cmd == 1)
	{
		int r = built_in_cmds(argv, env_var_list, cmd[start_index + 1]);
		if (r == 1)
			return 1; //execution was succsesful
		else if (r == 0)
		{
			path = cmd_path(argv[0], env_var_list);
			if (path == NULL)
				return 0;
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
	}
	else
	{
		i = 0;
		while (i < nb_cmd - 1)
		{
			execute(argv, env_var_list, false, original_std_fd, cmd[start_index + 1]);
			while (cmd[++start_index])
			{
				if (cmd[start_index]->is_cmd)
					break;
			}
			free(argv);
			argv = argv_generator(cmd, start_index, env_var_list);
			i++;
		}
		execute(argv, env_var_list, true, original_std_fd, cmd[start_index + 1]);
	}
	free(argv);
	return 1;
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

int	execute_cmd_line(char *line, t_env *env_var_list)
{
	t_token	**cmd;

	cmd = split_cmd_line(line);
	if (cmd == NULL)
		return (0);
	// if (cmd)
	// 	{
	// 		for (int j = 0; cmd[j] != NULL; ++j)
	// 		{
	// 			printf("\tToken %d: \"%s\" ", j + 1, cmd[j]->value);
	// 			// Check and print only true flags
	// 			if (cmd[j]->is_cmd)
	// 				printf("Cmd ");
	// 			if (cmd[j]->is_option)
	// 				printf("Opt ");
	// 			if (cmd[j]->is_param)
	// 				printf("Param ");
	// 			if (cmd[j]->is_input_redirection)
	// 				printf("InRedir ");
	// 			if (cmd[j]->is_output_redirection)
	// 				printf("OutRedir ");
	// 			if (cmd[j]->is_pipe)
	// 				printf("Pipe ");
	// 			if (cmd[j]->is_append_mode)
	// 				printf("Append ");
	// 			if (cmd[j]->is_background)
	// 				printf("Background ");
	// 			if (cmd[j]->is_cmd_separator)
	// 				printf("CmdSep ");
	// 			if (cmd[j]->is_escaped)
	// 				printf("Escaped ");
	// 			if (cmd[j]->is_quoted)
	// 				printf("Quoted ");
	// 			printf("\n"); // New line after listing all true flags
	// 		}
	// 		//free_token_matrix(cmd);
	// 	}
	// 	else
	// 	{
	// 		printf("\tError in tokenizing or in-quotes string found\n");
	// 	}
	// 	printf("\n");
	prepare_and_execute(cmd, 0, env_var_list);
	free_token_matrix(cmd);
	return 1;
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
		if (!ft_strncmp(input, "exit", 4))
			break;
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
