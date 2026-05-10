/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:17:58 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_higher_priority(t_scheduler *heap, t_dev *a, t_dev *b)
{
	long long	first;
	long long	second;

	if (heap->is_edf == FIFO)
	{
		first = a->request_seq;
		second = b->request_seq;
	}
	else
	{
		first = a->time_to_die;
		second = b->time_to_die;
		if (first == second)
		{
			first = a->coder_id;
			second = b->coder_id;
		}
	}
	return (first < second);
}

void	bubble_up(t_scheduler *heap, int idx)
{
	int		parent;
	t_dev	*tmp;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (!is_higher_priority(heap, heap->items[idx], heap->items[parent]))
			break ;
		tmp = heap->items[idx];
		heap->items[idx] = heap->items[parent];
		heap->items[parent] = tmp;
		idx = parent;
	}
}

void	bubble_down(t_scheduler *heap, int idx, int size)
{
	int		left;
	int		right;
	int		best;
	t_dev	*tmp;

	while (1)
	{
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		best = idx;
		if (left < size && is_higher_priority(heap,
				heap->items[left], heap->items[best]))
			best = left;
		if (right < size && is_higher_priority(heap,
				heap->items[right], heap->items[best]))
			best = right;
		if (best == idx)
			break ;
		tmp = heap->items[idx];
		heap->items[idx] = heap->items[best];
		heap->items[best] = tmp;
		idx = best;
	}
}

void	heap_remove_at(t_scheduler *heap, int index)
{
	if (!heap || index < 0 || index >= heap->size)
		return ;
	heap->size--;
	if (index == heap->size)
		return ;
	heap->items[index] = heap->items[heap->size];
	bubble_up(heap, index);
	bubble_down(heap, index, heap->size);
}

t_scheduler	*heap_init(int max_size, int is_edf)
{
	t_scheduler	*heap;

	if (max_size <= 0)
		return (NULL);
	heap = malloc(sizeof(t_scheduler));
	if (!heap)
		return (NULL);
	heap->items = malloc(sizeof(t_dev *) * max_size);
	if (!heap->items)
	{
		free(heap);
		return (NULL);
	}
	heap->max_size = max_size;
	heap->size = 0;
	heap->is_edf = is_edf;
	return (heap);
}
