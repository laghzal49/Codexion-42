/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:32:05 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:18:02 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

int	request_dongles(t_dev *coder)
{
	pthread_mutex_lock(&coder->all->heap_mutex);
	coder->request_seq = ++coder->all->request_seq;
	heap_insert(coder->all->heap, coder);
	pthread_mutex_unlock(&coder->all->heap_mutex);
	if (!wait_for_dongle(coder))
		return (0);
	if (should_stop(coder->all))
		return (0);
	log_print(coder, "has taken a dongle");
	log_print(coder, "has taken a dongle");
	return (1);
}

int	wait_for_dongle(t_coder *coder)
{
	pthread_mutex_lock(&coder->cv_mu);
	while (!coder->granted && !should_stop(coder->all))
		pthread_cond_wait(&coder->cv, &coder->cv_mu);
	pthread_mutex_unlock(&coder->cv_mu);
	return (coder->granted);
}

int	check_avalible(t_tool *first, t_tool *second)
{
	t_tool		*lo;
	t_tool		*hi;
	long long	now;

	if (first == second)
		return (0);
	if (first < second)
	{
		lo = first;
		hi = second;
	}
	else
	{
		lo = second;
		hi = first;
	}
	pthread_mutex_lock(&lo->mutex);
	pthread_mutex_lock(&hi->mutex);
	now = get_time_in_ms();
	if (first->in_use || second->in_use
		|| now < first->cooldown || now < second->cooldown)
	{
		pthread_mutex_unlock(&hi->mutex);
		pthread_mutex_unlock(&lo->mutex);
		return (0);
	}
	first->in_use = 1;
	second->in_use = 1;
	pthread_mutex_unlock(&hi->mutex);
	pthread_mutex_unlock(&lo->mutex);
	return (1);
}

void	take_dongles(t_dev *coder)
{
	t_tool	*first;
	t_tool	*second;

	first = coder->left_dongle;
	second = coder->right_dongle;
	if (!check_avalible(first, second))
		return ;
	log_print(coder, "has taken a dongle");
	log_print(coder, "has taken a dongle");
	coder->granted = 1;
}

void	put_dongle(t_dev *coder)
{
	t_tool	*first;
	t_tool	*second;
	t_tool	*lo;
	t_tool	*hi;

	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first == second)
	{
		pthread_mutex_lock(&first->mutex);
		first->in_use = 0;
		first->cooldown = get_time_in_ms() + coder->all->parms.dongle_cooldown;
		pthread_mutex_unlock(&first->mutex);
		coder->granted = 0;
		return ;
	}
	if (first < second)
	{
		lo = first;
		hi = second;
	}
	else
	{
		lo = second;
		hi = first;
	}
	pthread_mutex_lock(&lo->mutex);
	pthread_mutex_lock(&hi->mutex);
	first->in_use = 0;
	second->in_use = 0;
	first->cooldown = get_time_in_ms() + coder->all->parms.dongle_cooldown;
	second->cooldown = get_time_in_ms() + coder->all->parms.dongle_cooldown;
	pthread_mutex_unlock(&hi->mutex);
	pthread_mutex_unlock(&lo->mutex);
	coder->granted = 0;
}
