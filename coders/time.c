/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 08:56:47 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/11 18:41:20 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

long long   get_time_in_ms()
{
  long long         time;
  struct timeval    tv;
  
  if (gettimeofday(&tv, NULL) == -1)
    return (1);
  time = (tv.tv_sec * 1000)  + (tv.tv_usec / 1000);
  return (time);
}

void    ft_usleep(long long time_to_sleep)
{
  long long start_time;

  start_time = get_time_in_ms();
  while ((get_time_in_ms() - start_time) < time_to_sleep)
  {
    usleep(50);
  }
}
