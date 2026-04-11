/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 10:41:53 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/10 10:58:05 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

void print_state(t_coder *coder, const char *state)
{
  pthread_mutex_lock(&coder->sim->log_mutex);
  pthread_mutex_lock(&coder->sim->stop_mutex);
  if (coder->sim->stop_flag == 0)
    printf("%lld %d %s\n", get_time_in_ms() - coder->sim->start_time_ms, coder->id, state);
  pthread_mutex_unlock(&coder->sim->stop_mutex);
  pthread_mutex_unlock(&coder->sim->log_mutex);
}

void    coder_routine(void *arg)
{
  t_coder   *coder;

  coder = (t_coder *)arg;
  if (coder->id % 2 == 0)
    ft_usleep(10);
  while (1)
  {

  }
}
