/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 15:27:51 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/11 16:09:18 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

int start_sim(t_sim *sim)
{
  int   i;

  i = 0;
  sim->start_time_ms = get_time_in_ms();
  while (i < sim->params.num_coders)
  {
    sim->coders[i].last_compile = sim->start_time_ms;
    if (pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]) != 0)
      return (1);
    i++;
  }
  return (0);
}

void    join_sim(t_sim *sim)
{
  int   i;

  i = 0;
  while (i < sim->params.num_coders)
  {
    pthread_join(sim->coders[i].thread, NULL);
    i++;
  }
}

