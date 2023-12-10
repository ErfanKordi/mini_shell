/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekordi <ekordi@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 14:55:02 by ekordi            #+#    #+#             */
/*   Updated: 2023/12/07 15:18:27 by ekordi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


char	*cmd_path(char *cmd, char **envp)
{
	char	*temp;
	char	**paths;
	int		i;
	char	*path;

	i = 0;
	while (ft_strnstr(envp[i], "PATH", 4) == 0)
		i++;
	paths = ft_split(envp[i] + 5, ':');
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
	return NULL;
}
void	execute(char *cmd, char **envp)
{
	char	*path;

	path = cmd_path(cmd, envp);
	if(!path)
		return ;
	char *args[] = {cmd, NULL};
	int pid1 = fork();
	if (pid1 < 0)
	{
		perror("fork");
		exit(1);
	}
	if (pid1 == 0)
	{
		if (execve(path, args, envp) == -1)
		ft_putstr_fd("Exec Error", 2, true);
	}
	waitpid(pid1, 0, 0);
}
