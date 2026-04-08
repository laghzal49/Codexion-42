/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:00:45 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/03/15 23:34:50 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ****************************************************************************/

#include "codexion.h"


int	init_sim_core(t_sim *sim)
{
	sim->stop_flag = 0;
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (1);
	sim->start_time_ms = get_time_in_ms();
	return (0);
}

int	init_alloc(t_sim *sim)
{
	sim->coders = ft_malloc(sim->params.num_coders * sizeof(t_coder));
	if (sim->coders == NULL)
		return (1);
	sim->dongles = ft_malloc(sim->params.num_coders * sizeof(t_dongle));
	if (sim->dongles == NULL)
		return (1);
	return (0);
}

int	init_sim(t_sim *sim)
{
	int	i;

	i = 0;
	if (init_sim_core(sim) || init_alloc(sim) || init_dongles(sim)
		|| init_coders(sim))
		return (1);
	while (i < sim->params.num_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop_flag = 1;
			pthread_mutex_unlock(&sim->stop_mutex);
			while (--i >= 0)
				pthread_join(sim->coders[i].thread, NULL);
			return (1);
		}
		i++;
	}
	return (0);
}
