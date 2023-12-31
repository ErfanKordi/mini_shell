/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_cmd_line.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekordi <ekordi@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 04:16:25 by amarabin          #+#    #+#             */
/*   Updated: 2023/12/10 18:39:04 by ekordi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_token_matrix(t_token **tokens)
{
	size_t	i;

	if (!tokens)
		return ;
	i = 0;
	while (tokens[i])
	{
		free(tokens[i]->value);
		free(tokens[i]);
		i++;
	}
	free(tokens);
}


static size_t	count_tokens(const char *s, bool *in_quotes)
{
	size_t	word_count;
	char	current_quote;
	bool	escape_next;

	word_count = 0;
	escape_next = false;
	*in_quotes = false;
	while (*s)
	{
		if (*s == '\\')
		{
			word_count++;
			s++;
		}
		else if (*s == '\'' || *s == '\"')
		{
			word_count++;
			current_quote = *s;
			*in_quotes = true;
			s++;
			while (*s && *s != current_quote)
				s++;
			if (*s == current_quote)
				*in_quotes = false;
		}
		else if (strchr(";|<>&", *s))
		{
			word_count++;
			if (*(s + 1) == *s)
				s++;
		}
		else if (*s != ' ' && *s != '\t')
		{
			word_count++;
			while (*s && (*s != ' ' && *s != '\t'))
				s++;
			continue ;
		}
		s++;
	}
	return (word_count);
}

static t_token	**generate_matrix(const char *s)
{
	size_t	word_count;
	bool	in_quotes;
	t_token	**tokens;

	in_quotes = false;
	word_count = count_tokens(s, &in_quotes);
	if (in_quotes)
		return (NULL);
	tokens = (t_token **)malloc((word_count + 1) * sizeof(t_token *));
	if (!tokens)
		return (NULL);
	tokens[word_count] = NULL;
	// printf("word_count: %zu\n", word_count);
	return (tokens);
}

static t_token	**fill_matrix(const char *s, t_token **tokens)
{
	const char	*p;
	size_t		i;
	char		current_quote;
	bool		new_cmd;
	bool		single_quoted;

	i = 0;
	new_cmd = true;
	while (*s)
	{
		if (*s == ' ' || *s == '\t')
		{
			while (*s == ' ' || *s == '\t')
				s++;
			continue ;
		}
		if (!(*s))
			break ;
		tokens[i++] = (t_token *)malloc(sizeof(t_token));
		if (!tokens[i - 1])
		{
			free_token_matrix(tokens);
			return (NULL);
		}
		*(tokens[i - 1]) = (t_token){false, false, false, false, false, false,
			false, false, false, false, false, false, NULL};
		if (*s == '\\')
		{
			p = s + 1;
			tokens[i - 1]->is_escaped = true;
		}
		else if ((*s == '\'' || *s == '\"'))
		{
			tokens[i - 1]->is_quoted = true;
			tokens[i - 1]->is_param = true;
			if(*s == '\'')
				single_quoted = true;
			else
				single_quoted = false;
			current_quote = *s;
			p = s + 1;
			while (*p && *p != current_quote)
				p++;
			if (!(*p))
			{
				free_token_matrix(tokens);
				return (NULL);
			}
		}
		else if (strchr(";|<>&", *s))
		{
			if (*s == '|')
				tokens[i - 1]->is_pipe = true;
			else if (*s == '<')
				tokens[i - 1]->is_input_redirection = true;
			else if (*s == '>')
			{
				if (*(s + 1) == '>')
					tokens[i - 1]->is_append_mode = true;
				else
					tokens[i - 1]->is_output_redirection = true;
			}
			else if (*s == '&')
				tokens[i - 1]->is_background = true;
			else if (*s == ';')
				tokens[i - 1]->is_cmd_separator = true;
			if (*(s + 1) == *s)
				p = s + 1;
			else
				p = s;
			new_cmd = true;
		}
		else if (*s == '-')
		{
			p = s;
			while (*p && (*p != ' ' && *p != '\t'))
				p++;
			if (*p == ' ' || *p == '\t')
				p--;
			tokens[i - 1]->is_param = true;
		}
		else if (*s != ' ' && *s != '\t')
		{
			p = s;
			while (*p && (*p != ' ' && *p != '\t'))
				p++;
			if (*p == ' ' || *p == '\t')
				p--;
			if (new_cmd)
				tokens[i - 1]->is_cmd = true;
			else
				tokens[i - 1]->is_param = true;
			new_cmd = false;
		}
		tokens[i - 1]->value = ft_strgetbetween(s, p);
		if (tokens[i - 1]->value == NULL)
			return (free_token_matrix(tokens), NULL);
		s = p;
		if (*s)
			s++;
	}
	tokens[i] = NULL;
	return (tokens);
}

t_token	**split_cmd_line(char *s)
{
	t_token	**tokens;

	if (!s)
		return (NULL);
	tokens = generate_matrix(s);
	if (!tokens)
		return (NULL);
	return (fill_matrix(s, tokens));
}

// int	main(void)
// {
// 	t_token	**tokens;

// 	char *test_cases[] = {
// 		"echo Hello World",                  // Simple command
// 		"ls -l | grep 'txt'",                // Pipe and single quotes
// 		"cat \"file name with spaces.txt\"", // Double quotes
// 		"grep \\| filename",                 // Escaped character
// 		"find . -name \\*.c",                // Escaped wildcard
// 		"echo 'Single quote with a \\ backslash'",
// 		// Escaped char in single quotes
// 		"echo \"Double quote with a \\ backslash\"",
// 		// Escaped char in double quotes
// 		"command > output.txt",  // Redirection
// 		"command < input.txt",   // Input redirection
// 		"command >> append.txt", // Append redirection
// 		"command1 && command2",  // Logical AND
// 		"command1 || command2",  // Logical OR
// 		"ls -l \"My Documents\" 'Backup Folder'",
// 		"grep -E \"^a\\w*\" sample.txt | sort",
// 		"echo \"This is a test\" && echo 'Another test'",
// 		"mkdir -p /tmp/test\\ folder",
// 		"find . -name \"*.txt\" -exec cat {} \\;",
// 		"echo \"Nested \\\"quotes\\\" here\"",
// 		"curl -X POST http://example.com/api -d '{\"key\": \"value\"}'",
// 		"tar -czf backup.tar.gz --exclude='*.tmp' /home/user/Documents",
// 		"awk 'BEGIN {FS=\",\"; OFS=\"\\t\"} {print $1, $2}' data.csv",
// 		"python -c 'import os; print(os.listdir(\".\"))'",
// 		NULL // Sentinel value
// 	};
// 	for (int i = 0; test_cases[i] != NULL; ++i)
// 	{
// 		printf("Test Case %d: \"%s\"\n", i + 1, test_cases[i]);
// 		tokens = split_cmd_line(test_cases[i]);
// 		if (tokens)
// 		{
// 			for (int j = 0; tokens[j] != NULL; ++j)
// 			{
// 				printf("\tToken %d: \"%s\" ", j + 1, tokens[j]->value);
// 				// Check and print only true flags
// 				if (tokens[j]->is_cmd)
// 					printf("Cmd ");
// 				if (tokens[j]->is_option)
// 					printf("Opt ");
// 				if (tokens[j]->is_param)
// 					printf("Param ");
// 				if (tokens[j]->is_input_redirection)
// 					printf("InRedir ");
// 				if (tokens[j]->is_output_redirection)
// 					printf("OutRedir ");
// 				if (tokens[j]->is_pipe)
// 					printf("Pipe ");
// 				if (tokens[j]->is_append_mode)
// 					printf("Append ");
// 				if (tokens[j]->is_background)
// 					printf("Background ");
// 				if (tokens[j]->is_cmd_separator)
// 					printf("CmdSep ");
// 				if (tokens[j]->is_escaped)
// 					printf("Escaped ");
// 				if (tokens[j]->is_quoted)
// 					printf("Quoted ");
// 				printf("\n"); // New line after listing all true flags
// 			}
// 			free_token_matrix(tokens);
// 		}
// 		else
// 		{
// 			printf("\tError in tokenizing or in-quotes string found\n");
// 		}
// 		printf("\n");
// 	}
// 	return (0);
// }
