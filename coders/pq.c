/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pq.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 20:21:33 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/12 15:58:00 by tlaghzal         ###   ########.fr       */
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

void	pq_init(t_pq *pq)
{
	pq->size = 0;
}

void	pq_push(t_pq *pq, t_request req)
{
	if (pq->size >= 2)
		return ;
	pq->heap[pq->size] = req;
	pq->size++;
	if (pq->size == 2)
	{
		if (pq->heap[1].key < pq->heap[0].key)
			swap(&pq->heap[1], &pq->heap[0]);
	}
}

t_request	pq_pop(t_pq *pq)
{
	t_request	top;

	top = pq->heap[0];
	if (pq->size == 2)
		pq->heap[0] = pq->heap[1];
	pq->size--;
	return (top);
}

t_request	*pq_peek(t_pq *pq)
{
	if (pq->size == 0)
		return (NULL);
	return (&pq->heap[0]);
}
