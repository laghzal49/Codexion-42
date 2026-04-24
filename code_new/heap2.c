/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 14:06:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 13:31:35 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_is_empty(t_heap *heap)
{
	if (!heap)
		return (1);
	return (heap->size == 0);
}

int	heap_is_full(t_heap *heap)
{
	if (!heap)
		return (1);
	return (heap->size >= heap->max_size);
}

int	heap_insert(t_heap *heap, t_coder *coder)
{
	if (!heap || !coder || heap_is_full(heap))
		return (0);
	heap->items[heap->size] = coder;
	bubble_up(heap, heap->size);
	heap->size++;
	return (1);
}

int	heap_pop(t_heap *heap, t_coder **out)
{
	if (!heap || !out || heap_is_empty(heap))
		return (0);
	*out = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		bubble_down(heap, 0, heap->size);
	}
	return (1);
}

void	heap_destroy(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->items);
	free(heap);
}
