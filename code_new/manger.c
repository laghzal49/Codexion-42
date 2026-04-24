/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 14:48:06 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_better_request(t_heap *heap, t_coder *first, t_coder *second)
{
	if (first->has_first_dongle != second->has_first_dongle)
		return (first->has_first_dongle > second->has_first_dongle);
	if (!heap->is_edf)
		return (first->request_seq < second->request_seq);
	if (first->time_to_die == second->time_to_die)
	{
		if (first->compile_count == second->compile_count)
			return (first->request_seq < second->request_seq);
		return (first->compile_count < second->compile_count);
	}
	return (first->time_to_die < second->time_to_die);
}

static int	update_best_index(t_all *all, int index, int best_index)
{
	long long	current_time;
	t_dongle	*dongle;
	t_coder		*best;

	current_time = get_time_in_ms();
	dongle = all->heap->items[index]->target_dongle;
	if (!dongle || dongle->in_use != 0 || current_time < dongle->cooldown)
		return (best_index);
	if (best_index == -1)
		return (index);
	best = all->heap->items[best_index];
	if (is_better_request(all->heap, all->heap->items[index], best))
		return (index);
	return (best_index);
}

static int	find_best_request(t_all *all)
{
	int	index;
	int	best_index;

	best_index = -1;
	index = 0;
	while (index < all->heap->size)
	{
		best_index = update_best_index(all, index, best_index);
		index++;
	}
	return (best_index);
}

static int	try_grant_requests(t_all *all)
{
	int	best_index;

	best_index = find_best_request(all);
	if (best_index == -1)
		return (0);
	all->heap->items[best_index]->target_dongle->in_use = 1;
	all->heap->items[best_index]->granted = 1;
	pthread_cond_signal(&all->heap->items[best_index]->cv);
	heap_remove_at(all->heap, best_index);
	return (1);
}

void	*manager_routine(void *arg)
{
	t_all	*all;

	all = (t_all *)arg;
	pthread_mutex_lock(&all->req_mu);
	while (!all->stop_flag)
	{
		while (heap_is_empty(all->heap) && !all->stop_flag)
			pthread_cond_wait(&all->req_cv, &all->req_mu);
		if (all->stop_flag)
			break ;
		if (try_grant_requests(all) == 0)
		{
			pthread_mutex_unlock(&all->req_mu);
			usleep(100);
			pthread_mutex_lock(&all->req_mu);
		}
	}
	pthread_mutex_unlock(&all->req_mu);
	return (NULL);
}
