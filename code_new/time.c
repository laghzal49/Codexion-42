/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:42:22 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/13 17:47:17 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>

long long   get_time_in_ms(void)
{
  struct timeval time;
  long long t;

  if (gettimeofday(&time, NULL) == -1)
    return (1);
  t = (time.tv_sec * 1000) + (time.tv_usec / 1000);
  return (t);
}
