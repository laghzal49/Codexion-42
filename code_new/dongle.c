/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:32:05 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/03 12:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	grant_dongles(t_dev *coder, t_tool *first, t_tool *second)
{
	long long	now;

	now = get_time_in_ms();
	if (first->in_use || second->in_use
		|| now < first->cooldown || now < second->cooldown)
		return (0);
	first->in_use = 1;
	second->in_use = 1;
	cleanup_heaps_locked(coder);
	pthread_mutex_unlock(&first->mutex);
	if (first != second)
		pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&coder->all->heap_mutex);
	pthread_mutex_lock(&coder->cv_mu);
	coder->time_to_die = get_time_in_ms() + coder->all->parms.time_to_burnout;
	pthread_mutex_unlock(&coder->cv_mu);
	log_print(coder, "has taken a dongle");
	log_print(coder, "has taken a dongle");
	return (1);
}

static int	try_acquire(t_dev *coder, t_tool *first, t_tool *second)
{
	pthread_mutex_lock(&coder->all->heap_mutex);
	if (!is_top_of_heap(coder->left_dongle, coder)
		|| !is_top_of_heap(coder->right_dongle, coder))
	{
		pthread_mutex_unlock(&coder->all->heap_mutex);
		usleep(100);
		return (0);
	}
	pthread_mutex_lock(&first->mutex);
	if (first != second)
		pthread_mutex_lock(&second->mutex);
	if (grant_dongles(coder, first, second))
		return (1);
	if (first != second)
		pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_unlock(&coder->all->heap_mutex);
	usleep(100);
	return (0);
}

int	request_dongles(t_dev *coder)
{
	t_tool	*first;
	t_tool	*second;

	coder->request_seq = next_seq(coder);
	lock_order(coder, &first, &second);
	pthread_mutex_lock(&coder->all->heap_mutex);
	heap_insert(coder->left_dongle->heap, coder);
	heap_insert(coder->right_dongle->heap, coder);
	pthread_mutex_unlock(&coder->all->heap_mutex);
	while (!should_stop(coder->all))
	{
		if (try_acquire(coder, first, second))
			return (1);
	}
	cleanup_heaps(coder);
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
	pthread_mutex_lock(&coder->all->heap_mutex);
	pthread_mutex_lock(&first->mutex);
	if (first != second)
		pthread_mutex_lock(&second->mutex);
	now = get_time_in_ms();
	first->in_use = 0;
	first->cooldown = now + cd;
	second->in_use = 0;
	second->cooldown = now + cd;
	if (first != second)
		pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	pthread_mutex_unlock(&coder->all->heap_mutex);
	pthread_mutex_lock(&coder->all->req_mu);
	pthread_cond_broadcast(&coder->all->req_cv);
	pthread_mutex_unlock(&coder->all->req_mu);
}
