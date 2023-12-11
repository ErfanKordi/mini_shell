#include "minishell.h"

char	*exp_dbl_quot(char *token, t_env *env_var_list)
{
	char	*p;
	char	*value;
	char	*tmp;
	char	*var_start;
	size_t	var_start_offt;

	if (!token)
		return (NULL);
	p = token;
	while (*p)
	{
		if (*p == '$')
		{
			var_start = ++p;
			var_start_offt = var_start - token;
			while (ft_isalnum(*p) || *p == '_')
				p++;
			tmp = ft_strgetbetween(var_start, p - 1);
			if (!tmp)
				return (NULL);
			value = get_env(env_var_list, tmp);
			if (!value)
				value = "";
			free(tmp);
			tmp = ft_strrplbtwn(token, value, var_start - 1, p - 1);
			if (!tmp)
				return (NULL);
			free(token);
			token = tmp;
			p = token + var_start_offt - 1 + ft_strlen(value);
			continue ;
		}
		p++;
	}
	return (ft_strdup(token));
}

int	expand_token(t_token *token, t_env *env_var_list)
{
	char	*tmp;

	if (!token->is_quoted && token->value[0] == '$')
	{
		tmp = get_env(env_var_list, token->value + 1);
		free(token->value);
		if (!tmp)
			token->value = ft_strdup("");
		else
			token->value = ft_strdup(tmp);
	}
	else if (token->is_quoted)
	{
		if (token->is_single_quoted)
			tmp = ft_strtrim(token->value, "\'");
		else
			tmp = exp_dbl_quot(ft_strtrim(token->value, "\""), env_var_list);
		free(token->value);
		token->value = tmp;
	}
	if (!token->value)
		token->value = NULL;
	return (token->value != NULL);
}
