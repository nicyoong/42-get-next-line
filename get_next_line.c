/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 16:44:47 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/09 22:07:07 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "get_next_line.h"

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

static char	*extract_and_update_buffer(char **saved)
{
	size_t	len;
	char	*line;
	char	*new_saved;

	len = 0;
	while ((*saved)[len] && (*saved)[len] != '\n')
		len++;
	if ((*saved)[len] == '\n')
		len++;
	line = ft_substr(*saved, 0, len);
	if ((*saved)[len])
	{
		new_saved = ft_strdup(*saved + len);
		free(*saved);
		*saved = new_saved;
	}
	else
	{
		free(*saved);
		*saved = NULL;
	}
	return (line);
}

static int read_and_save(int fd, char **saved)
{
	char	*buffer;
	ssize_t	bytes_read;
	char	*temp;
	size_t	buffer_size;
	
	buffer_size = BUFFER_SIZE;
	buffer = (char *)malloc(buffer_size + 1);
	if (!buffer)
		return (-1);
	bytes_read = read(fd, buffer, buffer_size);
	while (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		if (*saved)
		{
			temp = ft_strjoin(*saved, buffer);
			free(*saved);
			*saved = temp;
		}
		else
			*saved = ft_strdup(buffer);
		if (ft_strchr(*saved, '\n'))
			break;
		if ((size_t)bytes_read == buffer_size)
		{
			buffer_size *= 2;
			buffer = (char *)realloc(buffer, buffer_size + 1);
			if (!buffer)
				return (-1);
		}
		bytes_read = read(fd, buffer, buffer_size);
	}
	free(buffer);
	if (bytes_read == 0 && (!*saved || **saved == '\0'))
		return (0);
	return (bytes_read);
}

char	*get_next_line(int fd)
{
	static char	*saved;
	char		*line;

	if (read_and_save(fd, &saved) <= 0 && (!saved || saved[0] == '\0'))
	{
		free(saved);
		saved = NULL;
		return (NULL);
	}
	line = extract_and_update_buffer(&saved);
	return (line);
}
