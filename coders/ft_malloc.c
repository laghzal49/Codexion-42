/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 05:59:46 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/03/28 06:13:12 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_list_gc	**get_manager(void)
{
	static t_list_gc	*gc = NULL;

	return (&gc);
}

void	*ft_malloc(size_t size)
{
	t_list_gc	*node;

	node = malloc(sizeof(t_list_gc) + size);
	if (!node)
		return (NULL);
	node->next = *get_manager();
	node->prev = NULL;
	if (*get_manager() != NULL)
		(*get_manager())->prev = node;
	*get_manager() = node;
	return (node + 1);
}

void	free_all(void)
{
	t_list_gc	*curr;
	t_list_gc	*next;

	curr = *get_manager();
	while (curr)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
	*get_manager() = NULL;
}
