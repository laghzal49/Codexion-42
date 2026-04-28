/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:02:09 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_better_request(t_heap *heap, t_coder *a, t_coder *b)
{
	if (!heap->is_edf)
	{
		if (a->request_seq == b->request_seq)
			return (a->coder_id < b->coder_id);
		return (a->request_seq < b->request_seq);
	}
	if (a->time_to_die == b->time_to_die)
	{
		if (a->request_seq == b->request_seq)
			return (a->coder_id < b->coder_id);
		return (a->request_seq < b->request_seq);
	}
	return (a->time_to_die < b->time_to_die);
}

static int	update_best_index(t_all *all, int i, int best_i)
{
	long long	now;
	t_dongle	*d;

	now = get_time_in_ms();
	d = all->heap->items[i]->target_dongle;
	if (!d || d->in_use != 0 || now < d->cooldown)
		return (best_i);
	if (best_i == -1)
		return (i);
	if (is_better_request(all->heap, all->heap->items[i], all->heap->items[best_i]))
		return (i);
	return (best_i);
}

static int	find_best_request(t_all *all)
{
	int	i;
	int	best_i;

	best_i = -1;
	i = 0;
	while (i < all->heap->size)
	{
		best_i = update_best_index(all, i, best_i);
		i++;
	}
	return (best_i);
}

static int	try_grant_requests(t_all *all)
{
	int	best_i;

	best_i = find_best_request(all);
	if (best_i == -1)
		return (0);
	all->heap->items[best_i]->target_dongle->in_use = 1;
	all->heap->items[best_i]->granted = 1;
	pthread_cond_broadcast(&all->heap->items[best_i]->cv);
	heap_remove_at(all->heap, best_i);
	return (1);
}

static void	broadcast_stop_locked(t_all *all)
{
	int	index;

	all->stop_flag = 1;
	pthread_cond_broadcast(&all->req_cv);
	index = 0;
	while (index < all->parms.num_coders)
	{
		pthread_cond_broadcast(&all->coder[index].cv);
		index++;
	}
}

void	*manager_routine(void *arg)
{
	t_all	*all;

	all = (t_all *)arg;
	pthread_mutex_lock(&all->req_mu);
	while (!all->stop_flag)
	{
		while (heap_is_empty(all->heap) && !all->stop_requested
			&& !all->stop_flag)
			pthread_cond_wait(&all->req_cv, &all->req_mu);
		if (all->stop_requested)
		{
			broadcast_stop_locked(all);
			break ;
		}
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
