/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarabin <amarabin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 10:11:03 by amarabin          #+#    #+#             */
/*   Updated: 2023/12/08 18:48:50 by amarabin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_bzero(void *s, int n)
{
	int				c;
	unsigned char	*p;

	p = s;
	c = 0;
	while (n--)
	{
		p[c] = 0;
		c++;
	}
}


void	*ft_calloc(size_t count, size_t size)
{
	void	*ret;

	ret = malloc(size * count);
	if (!ret)
		return (0);
	ft_bzero(ret, size * count);
	return (ret);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*r;
	int		i;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	i = 0;
	r = (char *)malloc((len + 1) * sizeof(char));
	if (!r)
		return (NULL);
	while (s[start] && len--)
	{
		r[i] = s[start];
		i++;
		start++;
	}
	r[i] = '\0';
	return (r);
}

void	free_arrayofstrings(char **a)
{
	int	i;

	i = 0;
	while (a[i])
	{
		free(a[i]);
		i++;
	}
	free(a);
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char		*d;
	const char	*s;
	size_t		i;

	if (!dest || !src)
		return (dest);
	i = 0;
	d = (char *)dest;
	s = (const char *)src;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
	return (dest);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	l;
	size_t	src_l;

	src_l = 0;
	if (src)
		src_l = ft_strlen(src);
	else
		return (0);
	if (!dst)
		return (src_l);
	if (src_l >= size - 1)
		l = size - 1;
	else
		l = src_l;
	ft_memcpy(dst, src, l);
	dst[l] = '\0';
	return (src_l);
}

char	*ft_strchr(const char *s, int c)
{
	while (s && *s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == '\0')
		return ((char *)s);
	return (NULL);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*trimmed;
	size_t	start;
	size_t	end;
	size_t	len;

	if (!s1)
		return (NULL);
	start = 0;
	while (s1[start] && ft_strchr(set, s1[start]))
		start++;
	end = ft_strlen(s1);
	while (end > start && ft_strchr(set, s1[end - 1]))
		end--;
	len = end - start;
	trimmed = (char *)malloc((len + 1) * sizeof(char));
	if (!trimmed)
		return (NULL);
	ft_strlcpy(trimmed, s1 + start, len + 1);
	return (trimmed);
}

char	*ft_strnstr( char *s1,  char *s2, size_t n)
{
	size_t	s2_len;
	size_t	i;
	char	*p;

	p = NULL;
	s2_len = ft_strlen((char *)s2);
	i = 0;
	if (s1 == NULL)
	{
		*p = 0;
	}
	if (s2_len == 0)
		return ((char *)s1);
	if (n < s2_len)
		return (NULL);
	while (s1[i] != '\0')
	{
		if (ft_strncmp(&s1[i], s2, s2_len) == 0)
			return ((char *)&s1[i]);
		if (i + s2_len >= n)
			break ;
		i++;
	}
	return (NULL);
}


char	*ft_strdup(const char *s)
{
	char	*dup;
	size_t	len;
	size_t	i;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	dup = (char *)malloc((len + 1) * sizeof(char));
	if (!dup)
		return (NULL);
	i = 0;
	while (s[i])
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
void	ft_putstr_fd(const char *s, int fd, bool newline)
{
	while (*s)
	{
		ft_putchar_fd(*s, fd);
		s++;
	}
	if (newline)
		ft_putchar_fd('\n', fd);
}
size_t	ft_strlen(const char *s)
{
	size_t	len;

	if (!s)
		return (0);
	len = 0;
	while (s[len])
		len++;
	return (len);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*joined;
	size_t	len1;
	size_t	len2;
	size_t	i;

	len1 = 0;
	len2 = 0;
	if (s1)
		len1 = ft_strlen(s1);
	if (s2)
		len2 = ft_strlen(s2);
	joined = (char *)malloc((len1 + len2 + 1) * sizeof(char));
	if (!joined)
		return (NULL);
	i = 0;
	while (i < len1 + len2)
	{
		if (i < len1)
			joined[i] = s1[i];
		else
			joined[i] = s2[i - len1];
		i++;
	}
	joined[len1 + len2] = '\0';
	return (joined);
}
int	ft_strncmp(char *s1, char *s2, size_t n)
{
	if (s1 == NULL)
	{
		if (s2 == NULL)
			return (0);
		else
			return (-1);
	}
	else if (s2 == NULL)
		return (1);
	while (n > 0)
	{
		if (*s1 != *s2)
			return ((unsigned char)*s1 - (unsigned char)*s2);
		if (*s1 == '\0')
			return (0);
		s1++;
		s2++;
		n--;
	}
	return (0);
}

char	*ft_strgetbetween(const char *start, const char *end)
{
	char	*word;
	size_t	i;

	word = (char *)malloc((end - start + 2) * sizeof(char));
	if (!word)
		return (NULL);
	i = 0;
	while (*start && start <= end)
	{
		word[i] = *start;
		start++;
		i++;
	}
	word[i] = '\0';
	return (word);
}

int ft_isalnum(int c)
{
	return ((c >= 'A' && c <= 'Z') || \
			(c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

char	*ft_strrplbtwn(char *orig, char *new, char *start, char *end)
{
	size_t	head_len;
	size_t	ncont_len;
	size_t	tail_len;
	char	*new_str;

	if (!new)
		new = "";
	head_len = start - orig;
	ncont_len = ft_strlen(new);
	tail_len = ft_strlen(end + 1);
	if (!orig || !start || !end || start > end)
		return (NULL);
	new_str = malloc(head_len + ncont_len + tail_len + 1);
	if (!new_str)
		return (NULL);
	ft_strlcpy(new_str, orig, head_len + 1);
	ft_strlcpy(new_str + head_len, new, ncont_len + 1);
	ft_strlcpy(new_str + head_len + ncont_len, end + 1, tail_len + 1);
	return (new_str);
}