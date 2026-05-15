/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   starter.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:55:22 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 12:31:34 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_start_state(t_app *all)
{
	int			i;
	long long	start_ms;

	start_ms = get_time_in_ms();
	i = 0;
	while (i < all->parms.num_coders)
	{
		pthread_mutex_lock(&all->coder[i].cv_mu);
		all->coder[i].time_to_die = start_ms + \
			all->parms.time_to_burnout;
		pthread_mutex_unlock(&all->coder[i].cv_mu);
		i++;
	}
}

static void	brodcast_start(t_app *all)
{
	pthread_mutex_lock(&all->req_mu);
	all->start_time_ms = get_time_in_ms();
	pthread_cond_broadcast(&all->req_cv);
	pthread_mutex_unlock(&all->req_mu);
}

static int	create_coder_threads(t_app *all)
{
	int	i;

	i = 0;
	while (i < all->parms.num_coders)
	{
		if (pthread_create(&all->coder[i].thread, NULL,
				coder_routine, &all->coder[i]) != 0)
		{
			set_stop(all);
			join_coder_threads(all, i);
			return (FAIL);
		}
		i++;
	}
	return (SUCCESS);
}

int	join_coder_threads(t_app *all, int end)
{
	int	i;

	i = 0;
	if (!end)
		end = all->parms.num_coders;
	while (i < end)
	{
		if (pthread_join(all->coder[i].thread, NULL) != 0)
		{
			set_stop(all);
			while (++i < end)
				pthread_join(all->coder[i].thread, NULL);
			return (FAIL);
		}
		i++;
	}
	return (SUCCESS);
}

int	start_and_join_coders(t_app *all)
{
	if (create_coder_threads(all) != SUCCESS)
	{
		set_stop(all);
		return (FAIL);
	}
	set_start_state(all);
	if (pthread_create(&all->monitor_thread, NULL, monitor_routine, all) != 0)
	{
		set_stop(all);
		join_coder_threads(all, 0);
		return (FAIL);
	}
	brodcast_start(all);
	if (join_coder_threads(all, 0) != SUCCESS)
	{
		pthread_join(all->monitor_thread, NULL);
		return (FAIL);
	}
	set_stop(all);
	if (pthread_join(all->monitor_thread, NULL) != 0)
		return (FAIL);
	return (SUCCESS);
}
