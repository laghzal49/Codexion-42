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
program must take the following arguments (all mandatory):
number_of_coders time_to_burnout time_to_compile time_to_debug
time_to_refactor number_of_compiles_required dongle_cooldown scheduler
◦ number_of_coders: The number of coders and also the number of dongles.
◦ time_to_burnout (in milliseconds): If a coder did not start compiling within
time_to_burnout milliseconds since the beginning of their last compile or the
beginning of the simulation, they burn out.
◦ time_to_compile (in milliseconds): The time it takes for a coder to compile.
During that time, they must hold two dongles.
◦ time_to_debug (in milliseconds): The time a coder will spend debugging.
◦ time_to_refactor (in milliseconds): The time a coder will spend refactoring.
After completing the refactoring phase, the coder will immediately attempt to
acquire dongles and start compiling again.
◦ number_of_compiles_required: If all coders have compiled at least this
many times, the simulation stops. Otherwise, it stops when a coder burns
out.
◦ dongle_cooldown (in milliseconds): After being released, a dongle is unavailable until its cooldown has passed.
◦ scheduler: The arbitration policy used by dongles to decide who gets them
when multiple coders request them.
The value must be exactly one of: fifo or edf.
fifo means First In, First Out: the dongle is granted to the coder whose
request arrived first.
edf means Earliest Deadline First with deadline = last_compile_start +
time_to_burnout.
• Each coder has a number ranging from 1 to number_of_cod