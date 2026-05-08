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

static void	insert_in_heaps(t_dev *coder, t_tool *first, t_tool *second)
{
	pthread_mutex_lock(&first->heap_mutex);
	if (first == second)
	{
		heap_insert(first->heap, coder);
		pthread_mutex_unlock(&first->heap_mutex);
		return ;
	}
	pthread_mutex_lock(&second->heap_mutex);
	heap_insert(coder->left_dongle->heap, coder);
	heap_insert(coder->right_dongle->heap, coder);
	pthread_mutex_unlock(&second->heap_mutex);
	pthread_mutex_unlock(&first->heap_mutex);
}

static void	unlock_all(t_tool *first, t_tool *second)
{
	if (first != second)
		pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	if (first != second)
		pthread_mutex_unlock(&second->heap_mutex);
	pthread_mutex_unlock(&first->heap_mutex);
}

static int	try_take_once(t_dev *coder, t_tool *first, t_tool *second)
{
	long long	now;

	pthread_mutex_lock(&first->heap_mutex);
	if (first != second)
		pthread_mutex_lock(&second->heap_mutex);
	pthread_mutex_lock(&first->mutex);
	if (first != second)
		pthread_mutex_lock(&second->mutex);
	now = get_time_in_ms();
	if (first == second
		|| !is_top_of_heap(coder->left_dongle, coder)
		|| !is_top_of_heap(coder->right_dongle, coder)
		|| first->in_use || second->in_use
		|| now < first->cooldown || now < second->cooldown)
		return (unlock_all(first, second), usleep(100), 0);
	first->in_use = 1;
	second->in_use = 1;
	cleanup_heaps_locked(coder);
	unlock_all(first, second);
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
		if (try_take_once(coder, first, second))
			return (1);
	}
	pthread_mutex_lock(&first->heap_mutex);
	if (first != second)
		pthread_mutex_lock(&second->heap_mutex);
	cleanup_heaps_locked(coder);
	if (first != second)
		pthread_mutex_unlock(&second->heap_mutex);
	pthread_mutex_unlock(&first->heap_mutex);
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
}
