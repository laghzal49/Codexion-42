/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:32:05 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 13:57:27 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	insert_in_heaps(t_dev *coder, t_tool *first, t_tool *second)
{
	lock_dongles(first, second, 1);
	if (heap_insert(coder->left_dongle->heap, coder) != SUCCESS)
	{
		unlock_dongles(first, second, 1);
		return (FAIL);
	}
	if (heap_insert(coder->right_dongle->heap, coder) != SUCCESS)
	{
		heap_remove_at(coder->left_dongle->heap,
			heap_find_index(coder->left_dongle->heap, coder));
		unlock_dongles(first, second, 1);
		return (FAIL);
	}
	unlock_dongles(first, second, 1);
	return (SUCCESS);
}

static int	try_take(t_dev *coder, t_tool *first, t_tool *second)
{
	long long	now;

	lock_dongles(first, second, 1);
	now = get_time_in_ms();
	if (!is_top_of_heap(coder->left_dongle, coder)
		|| !is_top_of_heap(coder->right_dongle, coder)
		|| first->in_use || second->in_use
		|| now < first->cooldown || now < second->cooldown)
	{
		unlock_dongles(first, second, 1);
		usleep(50);
		return (FAIL);
	}
	first->in_use = 1;
	second->in_use = 1;
	cleanup_heaps_locked(coder);
	unlock_dongles(first, second, 1);
	log_print(coder, "has taken a dongle");
	log_print(coder, "has taken a dongle");
	return (SUCCESS);
}

int	request_dongles(t_dev *coder)
{
	t_tool	*first;
	t_tool	*second;

	lock_order(coder, &first, &second);
	coder->request_time = get_time_in_ms();
	if (insert_in_heaps(coder, first, second) == FAIL)
		return (FAIL);
	while (!should_stop(coder->all))
	{
		if (try_take(coder, first, second) == SUCCESS)
			return (SUCCESS);
	}
	lock_dongles(first, second, 1);
	cleanup_heaps_locked(coder);
	unlock_dongles(first, second, 1);
	return (FAIL);
}

void	put_dongle(t_dev *coder)
{
	t_tool		*first;
	t_tool		*second;
	long long	now;
	long long	cd;

	cd = coder->all->parms.dongle_cooldown;
	lock_order(coder, &first, &second);
	lock_dongles(first, second, 0);
	now = get_time_in_ms();
	first->in_use = 0;
	first->cooldown = now + cd;
	second->in_use = 0;
	second->cooldown = now + cd;
	unlock_dongles(first, second, 0);
}
