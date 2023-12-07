/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarabin <amarabin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 05:25:22 by amarabin          #+#    #+#             */
/*   Updated: 2023/12/04 04:10:37 by amarabin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env	*create_env_var_node(char *str)
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
int	set_env(t_env **head, char *key_val_str)
{
	t_env	*current;
	t_env	*new_node;
	char	*eq_sign_pos;

	eq_sign_pos = ft_strchr(key_val_str, '=');
	if (!eq_sign_pos)
	{
		return (1);
	}
	current = *head;
	while (current != NULL)
	{
		if (!ft_strncmp(current->key, key_val_str, eq_sign_pos - key_val_str))
		{
			free(current->value);
			current->value = ft_strdup(eq_sign_pos + 1);
			return (current->value != NULL);
		}
		current = current->next;
	}
	new_node = create_env_var_node(key_val_str);
	if (!new_node)
		return (0);
	if (*head == NULL)
		*head = new_node;
	else
	{
		current = *head;
		while (current->next != NULL)
			current = current->next;
		current->next = new_node;
	}
	return (1);
}

char	*get_env(t_env *head, const char *key)
{
	t_env	*current;

	current = head;
	while (current != NULL)
	{
		if (!ft_strncmp(current->key, key, ft_strlen(key)))
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

/**
 * 0 = not updated
 * 1 = updated
 */
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

char	**prepare_envp_for_execve(t_env *head)
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

void	print_env_var_list(t_env *head)
{
	t_env	*current;

	current = head;
	while (current != NULL)
	{
		ft_putstr_fd(current->key, STDOUT_FILENO, false);
		ft_putstr_fd("=", STDOUT_FILENO, false);
		ft_putstr_fd(current->value, STDOUT_FILENO, true);
		current = current->next; // Move to the next node
	}
}
