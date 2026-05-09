/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 10:10:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/07 12:45:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_top_of_heap(t_tool *dongle, t_dev *coder)
{
	if (!dongle || !dongle->heap)
		return (0);
	if (heap_is_empty(dongle->heap))
		return (0);
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

long long	next_seq(t_dev *coder)
{
	long long	seq;

	pthread_mutex_lock(&coder->all->req_mu);
	coder->all->request_seq++;
	seq = coder->all->request_seq;
	pthread_mutex_unlock(&coder->all->req_mu);
	return (seq);
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

void	lock_mutex(t_tool *first, t_tool *second, int lock, int heap)
{
	if (lock == 0)
	{
		if (heap == 1)
		{
			pthread_mutex_lock(&first->heap_mutex);
			pthread_mutex_lock(&second->heap_mutex);
		}
		pthread_mutex_lock(&first->mutex);
		pthread_mutex_lock(&second->mutex);
	}
	else
	{
		pthread_mutex_unlock(&second->mutex);
		pthread_mutex_unlock(&first->mutex);
		if (heap == 1)
		{
			pthread_mutex_unlock(&second->heap_mutex);
			pthread_mutex_unlock(&first->heap_mutex);
		}
	}
}