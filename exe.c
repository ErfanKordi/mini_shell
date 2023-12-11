/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekordi <ekordi@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 14:55:02 by ekordi            #+#    #+#             */
/*   Updated: 2023/12/11 11:56:52 by ekordi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

void	execute(char *cmd, char **args, t_env *env, bool last_cmd, int *original_stdout)
{
	char	*path;
	int pid1;
	int		fd[2];

	path = cmd_path(cmd, env);
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
		dup2(fd[1], STDOUT_FILENO);
		if (last_cmd)
			dup2(original_stdout[0], STDOUT_FILENO);
		if (execve(path, args, get_env_for_exe(env)) == -1)
			ft_putstr_fd("Exec Error", 2, true);
	}
	else
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		if (last_cmd)
			dup2(original_stdout[1], STDIN_FILENO);
		waitpid(pid1, NULL, 0);
	}
}
