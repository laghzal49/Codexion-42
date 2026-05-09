/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 10:00:00 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/07 10:00:00 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_is_empty(t_scheduler *heap)
{
	if (!heap)
		return (1);
	return (heap->size == 0);
}

int	heap_is_full(t_scheduler *heap)
{
	if (!heap)
		return (1);
	return (heap->size >= heap->max_size);
}

void	heap_destroy(t_scheduler *heap)
{
	if (!heap)
		return ;
	free(heap->items);
	free(heap);
}

int	heap_find_index(t_scheduler *heap, t_dev *coder)
{
	int	i;

	if (!heap)
		return (-1);
	i = 0;
	while (i < heap->size)
	{
		if (heap->items[i] == coder)
			return (i);
		i++;
	}
	return (-1);
}

int	heap_insert(t_scheduler *heap, t_dev *coder)
{
	if (!heap || !coder || heap_is_full(heap))
		return (0);
	heap->items[heap->size] = coder;
	bubble_up(heap, heap->size);
	heap->size++;
	return (1);
}
