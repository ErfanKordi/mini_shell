/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekordi <ekordi@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 14:55:02 by ekordi            #+#    #+#             */
/*   Updated: 2023/12/11 16:44:09 by ekordi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
int built_in_cmds(char **args, t_env *env, t_token *cmd);

char	*cmd_path(char *cmd, t_env *env)
{
	char	*temp;
	char	**paths;
	int		i;
	char	*path;

	i = 0;
	paths = ft_split(get_env(env, "PATH"), ':');
	i = 0;
	while (paths[i])
	{
		path = ft_strjoin(paths[i++], "/");
		temp = ft_strjoin(path, cmd);
		if (access(temp, X_OK) == 0)
		{
			free_arrayofstrings(paths);
			free(path);
			return (temp);
		}
		free(temp);
		free(path);
	}
	ft_putstr_fd("zsh: command not found: ", 2, false);
	ft_putstr_fd(cmd, 2, true);
	free_arrayofstrings(paths);
	return (NULL);
}

void	execute(char **args, t_env *env, bool last_cmd, int *original_std_fd, t_token *cmd)
{
	char	*path;
	int pid1;
	int		fd[2];

	if (access(args[0], X_OK) == 0)
		path = args[0];
	else
		path = cmd_path(args[0], env);
	if (path == NULL)
		ft_putstr_fd("Path Error", 2, true);
	if (pipe(fd) == -1)
		ft_putstr_fd("Pipe Error", 2, true);
	pid1 = fork();
	if (pid1 < 0)
	{
		perror("fork");
		exit(1);
	}
	if (pid1 == 0)
	{
		close(fd[0]);
		dup2( fd[1], STDOUT_FILENO);
		if (last_cmd)
			dup2(original_std_fd[0], STDOUT_FILENO);
		int r = built_in_cmds(args, env, cmd);
		if (r == 1)
			exit(0);//execution was succsesful
		else if(r == 0)
		{
			if (execve(path, args, get_env_for_exe(env)) == -1)
				ft_putstr_fd("Exec Error", 2, true);
		}
	}
	else
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		if (last_cmd)
			dup2(original_std_fd[1], STDIN_FILENO);
		waitpid(pid1, NULL, 0);
	}
}

int execute_cd(char **args, t_token *cmd) {
    char *tmp;
    char *tmp2;

    if (args[1] != NULL && cmd->is_param) {
        if (args[1][0] == '~') {
            if (chdir(args[1]) != 0) {
                perror("chdir");
                return 0;
            }
            return 1;
        } else {
            tmp = getcwd(NULL, 0);
            if (tmp == NULL) {
                perror("getcwd");
                return 0;
            }
			tmp2 = ft_strjoin(tmp, "/");
			free(tmp);
			if (tmp2 == NULL)
				return (0);
			tmp = ft_strjoin(tmp2, cmd->value);
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


void execute_echo(char **args) {
    bool newline = true;
	int i = 0;

    while (args[i] != NULL) {
        if (args[i][0] == '-' && args[i][1] == 'n' && args[i][2] == '\0') {
            newline = false;
        } else {
            write(STDOUT_FILENO, args[i], strlen(args[i]));
            if (args[i + 1] != NULL) {
                write(STDOUT_FILENO, " ", 1);
            } else {
                i++;
                break;
            }
        }
        i++;
    }

    if (newline) {
        write(STDOUT_FILENO, "\n", 1);
    }
}


int built_in_cmds(char **args, t_env *env, t_token *cmd)
{
	if (!ft_strncmp(args[0], "echo", 4) && ft_strlen(args[0]) == 4)
		{
			execute_echo(args + 1);
			return 1;
		}
	else if (!ft_strncmp(args[0], "pwd", 3) && ft_strlen(args[0]) == 3)
	{
		if (!execute_pwd())
			return (-2);
		else
			return 1;
	}
	else if (!ft_strncmp(args[0], "cd", 2) && ft_strlen(args[0]) == 2)
	{
		if (!execute_cd(args, cmd))
			return -2;
		else
			return 1;
	}
	else if (!ft_strncmp(args[0], "export", 6) && ft_strlen(args[0]) == 6)
	{
		printf("export\n");
		if (args[1] && cmd->is_param && !set_env(&env, cmd->value))
			return (-2);
		else
			return 1;
	}
	else if (!ft_strncmp(args[0], "unset", 5) && ft_strlen(args[0]) == 5)
	{
		if (args[1] && cmd->is_param)
			unset_env(&env, cmd->value);
		return 1;
	}
	else if (!ft_strncmp(args[0], "env", 3) && ft_strlen(args[0]) == 3)
		{
			print_env_var_list(env);
			return 1;
		}
	return 0;
}
