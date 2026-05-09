/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:32:05 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/07 12:30:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

static int	insert_in_heaps(t_dev *coder, t_tool *first, t_tool *second)
{
	lock_mutex(first, second, 0, 1);
	heap_insert(coder->left_dongle->heap, coder);
	heap_insert(coder->right_dongle->heap, coder);
	lock_mutex(first, second, 1, 1);
	return (0);
}

static int	try_take(t_dev *coder, t_tool *first, t_tool *second)
{
	long long	now;

	lock_mutex(first, second, 0, 1);
	now = get_time_in_ms();
	if (!is_top_of_heap(coder->left_dongle, coder)
		|| !is_top_of_heap(coder->right_dongle, coder)
		|| first->in_use || second->in_use
		|| now < first->cooldown || now < second->cooldown)
	{
		lock_mutex(first, second, 1, 1);
		usleep(100);
		return (0);
	}
	first->in_use = 1;
	second->in_use = 1;
	cleanup_heaps_locked(coder);
	lock_mutex(first, second, 1, 1);
	log_print(coder, "has taken a dongle");
	log_print(coder, "has taken a dongle");
	return (1);
}

int	request_dongles(t_dev *coder)
{
	t_tool	*first;
	t_tool	*second;

	coder->request_seq = next_seq(coder);
	lock_order(coder, &first, &second);
	insert_in_heaps(coder, first, second);
	while (!should_stop(coder->all))
	{
		if (try_take(coder, first, second))
			return (1);
	}
	lock_mutex(first, second, 0, 1);
	cleanup_heaps_locked(coder);
	lock_mutex(first, second, 1, 1);
	return (0);
}

void	put_dongle(t_dev *coder)
{
	t_tool		*first;
	t_tool		*second;
	long long	now;
	long long	cd;

	cd = coder->all->parms.dongle_cooldown;
	lock_order(coder, &first, &second);
	lock_mutex(first, second, 0, 0);
	now = get_time_in_ms();
	first->in_use = 0;
	first->cooldown = now + cd;
	second->in_use = 0;
	second->cooldown = now + cd;
	lock_mutex(first, second, 1, 0);
}
