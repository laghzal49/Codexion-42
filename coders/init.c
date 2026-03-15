/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 23:00:45 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/03/15 23:34:50 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_sim_core(t_sim *sim)
{
	sim->stop_flag = 0;
	sim->start_time_ms = get_time_in_ms();
	pthread_mutex_init(&sim->stop_mutex, NULL);
	pthread_mutex_init(&sim->log_mutex, NULL);
	return (0);
}

int init_alloc(t_sim *sim)
{
    sim->coders = malloc(sizeof(t_coder) * sim->params.num_coders);
    sim->dongles = malloc(sizeof(t_dongle) * sim->params.num_coders);
    if (!sim->coders || !sim->dongles)
    {
        free(sim->coders);
        free(sim->dongles);
        error_exit("malloc failed");
    }
    return (0);
}

int	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.num_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].available_flag = 1;
		sim->dongles[i].cooldown_timestamp = 0;
		i++;
	}
	return (0);
}

int	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->params.num_coders)
	{
		sim->coders[i].id = i;
		sim->coders[i].state = 0;
		sim->coders[i].compile_count = 0;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1)
			% sim->params.num_coders];
		sim->coders[i].sim = sim;
		i++;
	}
	return (0);
}

int init_sim(t_sim *sim)
{
    int i = 0;
    if (init_sim_core(sim) || init_alloc(sim) || init_dongles(sim) || init_coders(sim))
        return (1);
    while (i < sim->params.num_coders)
    {
        if (pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]) != 0)
        {
            pthread_mutex_lock(&sim->stop_mutex);
            sim->stop_flag = 1;
            pthread_mutex_unlock(&sim->stop_mutex);
            while (--i >= 0) pthread_join(sim->coders[i].thread, NULL);
            return (1);
        }
        i++;
    }
    return (0);
}
