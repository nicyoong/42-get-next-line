/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 16:45:09 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/11 16:29:41 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "get_next_line_bonus.h"

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

static t_fd_buffer	*get_fd_buffer(t_fd_buffer **head, int fd)
{
	t_fd_buffer	*current;
	t_fd_buffer	*new_node;

	current = *head;
	while (current)
	{
		if (current->fd == fd)
			return (current);
		current = current->next;
	}
	new_node = (t_fd_buffer *)malloc(sizeof(t_fd_buffer));
	if (!new_node)
		return (NULL);
	new_node->fd = fd;
	new_node->saved = NULL;
	new_node->next = *head;
	*head = new_node;
	return (new_node);
}

static void	free_fd_buffer(t_fd_buffer **head, int fd)
{
	t_fd_buffer	*current;
	t_fd_buffer	*prev;

	current = *head;
	prev = NULL;
	while (current)
	{
		if (current->fd == fd)
		{
			if (prev)
				prev->next = current->next;
			else
				*head = current->next;
			free(current->saved);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

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

static int	read_and_save(int fd, char **saved)
{
	char	buffer[BUFFER_SIZE + 1];
	ssize_t	bytes_read;
	char	*temp;

	bytes_read = read(fd, buffer, BUFFER_SIZE);
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
			return (1);
		bytes_read = read(fd, buffer, BUFFER_SIZE);
	}
	if (bytes_read == 0 && (!*saved || **saved == '\0'))
		return (0);
	return (bytes_read);
}

char	*get_next_line(int fd)
{
	static t_fd_buffer	*head;
	t_fd_buffer			*fd_buffer;
	char				*line;

	fd_buffer = get_fd_buffer(&head, fd);
	if (!fd_buffer || read_and_save(fd, &fd_buffer->saved) < 0)
	{
		free_fd_buffer(&head, fd);
		return (NULL);
	}
	if (!fd_buffer->saved || fd_buffer->saved[0] == '\0')
	{
		free_fd_buffer(&head, fd);
		return (NULL);
	}
	line = extract_and_update_buffer(&fd_buffer->saved);
	return (line);
}
