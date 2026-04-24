/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   starter.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:55:22 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 14:48:08 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_start_state(t_all *all)
{
	int			index;
	long long	start;

	start = get_time_in_ms();
	all->start_time_ms = start;
	index = 0;
	while (index < all->parms.num_coders)
	{
		pthread_mutex_lock(&all->coder[index].cv_mu);
		all->coder[index].last_compile = start;
		all->coder[index].time_to_die = start + all->parms.time_to_burnout;
		pthread_mutex_unlock(&all->coder[index].cv_mu);
		index++;
	}
}

static int	create_coder_threads(t_all *all)
{
	int	index;

	index = 0;
	while (index < all->parms.num_coders)
	{
		if (pthread_create(&all->coder[index].thread, NULL,
				coder_routine, &all->coder[index]) != 0)
		{
			set_stop(all);
			while (index-- > 0)
				pthread_join(all->coder[index].thread, NULL);
			return (1);
		}
		index++;
	}
	return (0);
}

static int	join_coder_threads(t_all *all)
{
	int	index;

	index = 0;
	while (index < all->parms.num_coders)
	{
		if (pthread_join(all->coder[index].thread, NULL) != 0)
		{
			set_stop(all);
			while (++index < all->parms.num_coders)
				pthread_join(all->coder[index].thread, NULL);
			return (1);
		}
		index++;
	}
	return (0);
}

static int	start_controller_threads(t_all *all)
{
	if (pthread_create(&all->manager_thread, NULL, manager_routine, all) != 0)
		return (1);
	if (pthread_create(&all->monitor_thread, NULL, monitor_routine, all) != 0)
	{
		set_stop(all);
		pthread_join(all->manager_thread, NULL);
		return (1);
	}
	return (0);
}

int	start_and_join_coders(t_all *all)
{
	if (create_coder_threads(all) != 0)
		return (1);
	set_start_state(all);
	if (start_controller_threads(all) != 0)
	{
		join_coder_threads(all);
		return (1);
	}
	if (join_coder_threads(all) != 0)
	{
		pthread_join(all->manager_thread, NULL);
		pthread_join(all->monitor_thread, NULL);
		return (1);
	}
	set_stop(all);
	if (pthread_join(all->manager_thread, NULL) != 0)
		return (1);
	if (pthread_join(all->monitor_thread, NULL) != 0)
		return (1);
	return (0);
}
