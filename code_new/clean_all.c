/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_all.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 13:52:22 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:04:46 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_dongles(t_all *all, int num_coders)
{
	int	index;

	if (!all || !all->dongles)
		return ;
	index = 0;
	while (index < num_coders)
	{
		if (all->dongles[index].heap)
			heap_destroy(all->dongles[index].heap);
		if (all->dongles[index].heap_mutex_ready)
			pthread_mutex_destroy(&all->dongles[index].heap_mutex);
		pthread_mutex_destroy(&all->dongles[index].mutex);
		index++;
	}
}

static void	destroy_coder_sync(t_all *all)
{
	int	index;

	index = 0;
	while (index < all->parms.num_coders)
	{
		pthread_mutex_destroy(&all->coder[index].cv_mu);
		index++;
	}
}

void	cleanup_all(t_all *all)
{
	if (!all)
		return ;
	if (all->coder)
		destroy_coder_sync(all);
	cleanup_dongles(all, all->parms.num_coders);
	if (all->log_mutex_ready)
		pthread_mutex_destroy(&all->log_mutex);
	if (all->req_cv_ready)
		pthread_cond_destroy(&all->req_cv);
	if (all->req_mu_ready)
		pthread_mutex_destroy(&all->req_mu);
	free(all->dongles);
	all->dongles = NULL;
	free(all->coder);
	all->coder = NULL;
	free(all);
}
