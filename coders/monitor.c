/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 15:42:57 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/11 18:44:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <unistd.h>

int	check_death(t_sim *sim, int i)
{
	long long	time_since_compl;

	time_since_compl = get_time_in_ms() - sim->coders[i].last_compile;
	if (time_since_compl >= sim->params.time_to_burnout)
	{
		pthread_mutex_lock(&sim->log_mutex);
		printf("%lld %d burned out\n", 
			get_time_in_ms() - sim->start_time_ms, sim->coders[i].id);
		sim->stop_flag = 1;
		pthread_mutex_unlock(&sim->log_mutex);
		return (1);
	}
	return (0);
}

int	check_all_finished(t_sim *sim, int finished)
{
	if (sim->params.compiles_required > 0 
		&& finished == sim->params.num_coders)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		sim->stop_flag = 1;
		pthread_mutex_unlock(&sim->stop_mutex);
		return (1);
	}
	return (0);
}

int	check_coders(t_sim *sim)
{
	int	i;
	int	finished;

	i = 0;
	finished = 0;
	while (i < sim->params.num_coders)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		if (check_death(sim, i))
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			return (1);
		}
		if (sim->params.compiles_required > 0 
			&& sim->coders[i].compile_count >= sim->params.compiles_required)
			finished++;
		pthread_mutex_unlock(&sim->stop_mutex);
		i++;
	}
	return (check_all_finished(sim, finished));
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (1)
	{
		if (check_coders(sim) == 1)
			break ;
		usleep(1000);
	}
	return (NULL);
}
