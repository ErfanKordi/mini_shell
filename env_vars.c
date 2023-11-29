/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarabin <amarabin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 05:25:22 by amarabin          #+#    #+#             */
/*   Updated: 2023/11/29 08:55:40 by amarabin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env	*create_env_var_node(const char *str)
{
	char	*eq_sign_pos;
	t_env	*node;

	eq_sign_pos = ft_strchr(str, '=');
	if (!eq_sign_pos)
		return (NULL);
	node = (t_env *)malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->next = NULL;
	node->key = ft_strgetbetween(str, eq_sign_pos - 1);
	if (!node->key)
		return (free(node), NULL);
	node->value = ft_strdup(eq_sign_pos + 1);
	if (!node->value)
		return (free(node->key), free(node), NULL);
	return (node);
}

void	free_env_var_list(t_env *head)
{
	t_env	*temp;

	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp->key);
		free(temp->value);
		free(temp);
	}
}

t_env	*init_env_var(char *envp[])
{
	t_env	*head;
	t_env	*tail;
	t_env	*new_node;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (envp[i++] != NULL)
	{
		new_node = create_env_var_node(envp[i - 1]);
		if (new_node == NULL)
			return (free_env_var_list(head), NULL);
		if (tail == NULL)
		{
			head = new_node;
			tail = new_node;
		}
		else
		{
			tail->next = new_node;
			tail = new_node;
		}
	}
	return (head);
}

/**
 * 0 = memory failure
 * 1 = success
 */
int	set_env(t_env *head, char *var, char *value)
{
	t_env	*current;
	t_env	*new_node;

	current = head;
	while (current != NULL)
	{
		if (!ft_strncmp(current->key, var, ft_strlen(var)))
		{
			free(current->value);
			current->value = ft_strdup(value);
			return (1);
		}
		current = current->next;
	}
	new_node = malloc(sizeof(t_env));
	if (new_node == NULL)
		return (0);
	new_node->key = strdup(var);
	new_node->value = strdup(value);
	new_node->next = NULL;
	while (head->next != NULL)
		head = head->next;
	head->next = new_node;
	return (1);
}

int	unset_env(t_env **head, char *var)
{
	t_env	*current;
	t_env	*prev;

	current = *head;
	prev = NULL;
	while (current != NULL)
	{
		if (!ft_strncmp(current->key, var, ft_strlen(var)))
		{
			if (prev == NULL)
				*head = current->next;
			else
				prev->next = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return (1);
		}
		prev = current;
		current = current->next;
	}
	return (0);
}

int	get_env_list_size(t_env *head)
{
	int	count;

	count = 0;
	while (head != NULL)
	{
		count++;
		head = head->next;
	}
	return (count);
}

void	free_envp(char **envp, int i)
{
	while (i-- > 0)
		free(envp[i]);
	free(envp);
}

char	**prepare_enelope_for_execve(t_env *head)
{
	int		size;
	char	**envp;
	char	*tmp;
	int		i;

	i = 0;
	size = get_env_list_size(head);
	envp = (char **)malloc((size + 1) * sizeof(char *));
	while (head != NULL)
	{
		tmp = ft_strjoin(head->key, "=");
		if (!tmp)
			return (free_envp(envp, i), NULL);
		envp[i] = ft_strjoin(tmp, head->value);
		if (!envp[i])
			return (free(tmp), free_envp(envp, i), NULL);
		free(tmp);
		i++;
		head = head->next;
	}
	envp[size] = NULL;
	return (envp);
}
