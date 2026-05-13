/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 10:10:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 12:45:23 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_top_of_heap(t_tool *dongle, t_dev *coder)
{
	if (!dongle || !dongle->heap)
		return (FAIL);
	if (heap_is_empty(dongle->heap))
		return (FAIL);
	return (dongle->heap->items[0] == coder);
}

void	lock_order(t_dev *coder, t_tool **first, t_tool **second)
{
	if (coder->left_dongle < coder->right_dongle)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
	else
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
}

void	cleanup_heaps_locked(t_dev *coder)
{
	if (coder->left_dongle == coder->right_dongle)
	{
		heap_remove_at(coder->left_dongle->heap,
			heap_find_index(coder->left_dongle->heap, coder));
		return ;
	}
	heap_remove_at(coder->left_dongle->heap,
		heap_find_index(coder->left_dongle->heap, coder));
	heap_remove_at(coder->right_dongle->heap,
		heap_find_index(coder->right_dongle->heap, coder));
}


void	lock_dongles(t_tool *first, t_tool *second, int heap_lock)
{
	if (heap_lock)
	{
		pthread_mutex_lock(&first->heap_mutex);
		pthread_mutex_lock(&second->heap_mutex);
	}
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
}

void	unlock_dongles(t_tool *first, t_tool *second, int heap_lock)
{
	if (heap_lock)
	{
		pthread_mutex_unlock(&second->heap_mutex);
		pthread_mutex_unlock(&first->heap_mutex);
	}
		pthread_mutex_unlock(&second->mutex);
		pthread_mutex_unlock(&first->mutex);
}
