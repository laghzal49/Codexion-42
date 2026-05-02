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
#include <pthread.h>

void	check_coder(t_all *all)
{
	t_dev	*coder;
	int		i;

	pthread_mutex_lock(&all->heap_mutex);
	i = 0;
	while (i < all->heap->size)
	{
		coder = all->heap->items[i];
		if (check_avalible(coder->left_dongle, coder->right_dongle))
		{
			pthread_mutex_lock(&coder->cv_mu);
			coder->granted = 1;
			pthread_cond_signal(&coder->cv);
			pthread_mutex_unlock(&coder->cv_mu);
			heap_remove_at(all->heap, i);
		}
		else
			i++;
	}
	pthread_mutex_unlock(&all->heap_mutex);
}

void	*manager_routine(void *arg)
{
	t_all	*all;
	int		i;

	all = (t_all *)arg;
	while (!should_stop(all))
	{
		check_coder(all);
		usleep(500);
	}
	i = 0;
	while (i < all->parms.num_coders)
	{
		pthread_mutex_lock(&all->coder[i].cv_mu);
		pthread_cond_broadcast(&all->coder[i].cv);
		pthread_mutex_unlock(&all->coder[i].cv_mu);
		i++;
	}
	return (NULL);
}
