/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:09:20 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/13 17:33:58 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>
#include <unistd.h>

int coder(t_all *all)
{
  int   i;

  i = 0;
  while (i < all->parms.num_coders)
  {
    if (pthread_create(all->coder[i].thread, NULL,  coding, NULL))
      return (1);
    if (pthread_join(all->coder[i].thread, NULL)
  }
}

