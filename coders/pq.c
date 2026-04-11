/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pq.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 20:21:33 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/09 11:06:21 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

int pq_init(t_pq *pq)
{
  pq->size = 0;
  return (0);
}

int pq_push(t_pq *pq, t_request req)
{
  if (pq->size >= 2)
    return (0);
  pq->arr[pq->size] = req;
  pq->size++;
  if (pq->size == 2)
  {
    if (pq->arr[1].key < pq->arr[0].key)
    {
      swap(&pq->arr[1], &pq->arr[0]);     
   }
  }
  return (1);
}

t_request	pq_pop(t_pq *pq)
{
	t_request	top;

	top = pq->arr[0];
  if (pq->size == 2)
    pq->arr[0] = pq->arr[1];
  pq->size--;
	return (top);
}

t_request	*pq_peek(t_pq *pq)
{
	if (pq->size == 0)
		return (NULL);
	return (&pq->arr[0]);
}
