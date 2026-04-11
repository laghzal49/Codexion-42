/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 08:45:52 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/10 08:56:34 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int init_sim_core(t_sim *sim)
{
  sim->stop_flag = 0;
  if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
    return (1);
  if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
    return (1);
  sim->start_time_ms = 0;
  return (0);
}

int init_alloc(t_sim *sim)
{
  sim->coders = ft_malloc(sim->params.num_coders * sizeof(t_coder));
  if (!sim->coders)
    return (1);
  sim->dongles = ft_malloc(sim->params.num_coders * sizeof(t_dongle));
  if (!sim->dongles)
    return (1);
  return (0);
}

int init_dongles(t_sim *sim)
{
  int   i;

  i = 0;
  while (i < sim->params.num_coders)
  {
    if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
      return (1);
    if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
      return (1);
    pq_init(&sim->dongles[i].queue);
    sim->dongles[i].available_flag = 1;
    sim->dongles[i].cooldown_timestamp = 0;
    i++;
  }
  return (0);
}

int init_coders(t_sim *sim)
{
  int   i;

  i = 0;
  while (i < sim->coders.params.num_coders)
  {
    sim->coders[i].id = i + 1;
    sim->coders[i].compile_count = 0;
    sim->coders[i].last_compile = start_time_ms();
    sim->coders[i].sim = sim;
    sim->coders[i].left_dongle = &sim->dongles[i];
    sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->params.num_coders];
    i++;
  }
  return (0);
}

int init_sim(t_sim *sim)
{
	if (init_sim_core(sim) != 0)
		return (1);
	if (init_alloc(sim) != 0)
		return (1);
	if (init_dongles(sim) != 0)
		return (1);
	if (init_coders(sim) != 0)
		return (1);
	return (0);
}
