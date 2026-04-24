/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 14:48:06 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_higher_priority(t_heap *heap, t_coder *first, t_coder *second)
{
	if (first->has_first_dongle != second->has_first_dongle)
		return (first->has_first_dongle > second->has_first_dongle);
	if (!heap->is_edf)
		return (first->request_seq < second->request_seq);
	if (first->time_to_die == second->time_to_die)
	{
		if (first->compile_count == second->compile_count)
			return (first->request_seq < second->request_seq);
		return (first->compile_count < second->compile_count);
	}
	return (first->time_to_die < second->time_to_die);
}

void	bubble_up(t_heap *heap, int index)
{
	int		parent;
	t_coder	*tmp;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!is_higher_priority(heap, heap->items[index], heap->items[parent]))
			break ;
		tmp = heap->items[index];
		heap->items[index] = heap->items[parent];
		heap->items[parent] = tmp;
		index = parent;
	}
}

void	bubble_down(t_heap *heap, int index, int size)
{
	int		left;
	int		right;
	int		smallest;
	t_coder	*tmp;

	while (1)
	{
		left = 2 * index + 1;
		right = 2 * index + 2;
		smallest = index;
		if (left < size && is_higher_priority(heap,
				heap->items[left], heap->items[smallest]))
			smallest = left;
		if (right < size && is_higher_priority(heap,
				heap->items[right], heap->items[smallest]))
			smallest = right;
		if (smallest == index)
			break ;
		tmp = heap->items[index];
		heap->items[index] = heap->items[smallest];
		heap->items[smallest] = tmp;
		index = smallest;
	}
}

void	heap_remove_at(t_heap *heap, int index)
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

t_heap	*heap_init(int max_size, int is_edf)
{
	t_heap	*heap;

	if (max_size <= 0)
		return (NULL);
	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->items = malloc(sizeof(t_coder *) * max_size);
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
