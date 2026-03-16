/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 12:39:57 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_higher_priority(t_scheduler *heap, t_dev *a, t_dev *b)
{
	if (heap->is_edf == FIFO)
	{
		if (a->request_time == b->request_time)
			return (a->coder_id < b->coder_id);
		return (a->request_time < b->request_time);
	}
	else
	{
		if (a->time_to_die == b->time_to_die)
		{
			if (a->compile_count == b->compile_count)
				return (a->request_time < b->request_time);
			return (a->compile_count < b->compile_count);
		}
		return (a->time_to_die < b->time_to_die);
	}
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
	bubble_down(heap, index, heap->size);
}

t_scheduler	*heap_init(int max_size, int is_edf)
{
	t_scheduler	*heap;

	if (max_size <= 0)
		return (NULL);
	heap = ft_malloc(sizeof(t_scheduler));
	if (!heap)
		return (NULL);
	heap->items = ft_malloc(sizeof(t_dev *) * max_size);
	if (!heap->items)
		return (NULL);
	heap->max_size = max_size;
	heap->size = 0;
	heap->is_edf = is_edf;
	return (heap);
}
