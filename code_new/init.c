/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 20:37:08 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/13 17:48:26 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *init_coders(t_all *all)
{
  int   i;
  int   next_id;

  i = 0;
  all->coder = malloc(sizeof(t_coder) * all->parms.num_coders);
  if (!all->coder)
    return (NULL);
  while (i < all->parms.num_coders)
  {
    all->coder[i].coder_id = i;
    all->coder[i].complie_count = 0;
    all->coder[i].last_compile = get_time_in_ms();
    all->coder[i].right_dongle = &all->dongles[i];
    next_id = (i + 1) % all->parms.num_coders;
    all->coder[i].left_dongle = &all->dongles[next_id];
    i++;
  }
  return NULL;
}

void    *init_dongle(t_all *all)
{
  int   i;

  i = 0;
  all->dongles = malloc(sizeof(t_dongle) * all->parms.num_coders);
  if (!all->dongles)
    return (NULL);
  while (i < all->parms.num_coders)
  {
    all->dongles->in_use = 0;
    if (pthread_mutex_init(&all->dongles[i].mutex, NULL))
      return (NULL);
    all->dongles->cooldown = all->parms.dongle_cooldown;
    i++;
  }
}

int init_all(t_all *all)
{
  if (init_coders(all))
    return (1);
  if (init_dongle(all))
    return (1);
}
