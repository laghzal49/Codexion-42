/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 20:37:08 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/28 18:18:02 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_sync(t_app *all)
{
	if (pthread_mutex_init(&all->log_mutex, NULL) != 0)
		return (1);
	all->log_mutex_ready = 1;
	if (pthread_mutex_init(&all->req_mu, NULL) != 0)
		return (1);
	all->req_mu_ready = 1;
	if (pthread_cond_init(&all->req_cv, NULL) != 0)
		return (1);
	all->req_cv_ready = 1;
	return (0);
}

static int	init_alloc(t_app *all)
{
	all->dongles = malloc(sizeof(t_tool) * all->parms.num_coders);
	if (!all->dongles)
		return (1);
	all->coder = malloc(sizeof(t_dev) * all->parms.num_coders);
	if (!all->coder)
	{
		free(all->dongles);
		all->dongles = NULL;
		return (1);
	}
	return (0);
}

static int	init_dongles(t_app *all)
{
	int	index;

	index = 0;
	while (index < all->parms.num_coders)
	{
		all->dongles[index].in_use = 0;
		all->dongles[index].cooldown = 0;
		all->dongles[index].id = index;
		index++;
	}
	return (0);
}

static int	init_coders(t_app *all)
{
	int	index;
	int	next_index;

	index = 0;
	while (index < all->parms.num_coders)
	{
		next_index = (index + 1) % all->parms.num_coders;
		all->coder[index].coder_id = index + 1;
		all->coder[index].compile_count = 0;
		all->coder[index].right_dongle = &all->dongles[index];
		all->coder[index].left_dongle = &all->dongles[next_index];
		all->coder[index].target_dongle = NULL;
		all->coder[index].granted = 0;
		all->coder[index].has_first_dongle = 0;
		all->coder[index].request_seq = 0;
		all->coder[index].all = all;
		if (pthread_mutex_init(&all->coder[index].cv_mu, NULL) != 0)
			return (1);
		if (pthread_cond_init(&all->coder[index].cv, NULL) != 0)
			return (pthread_mutex_destroy(&all->coder[index].cv_mu), 1);
		index++;
	}
	return (0);
}

int	init_all(t_app *all)
{
	if (init_sync(all) != 0)
		return (1);
	all->heap = heap_init((int)all->parms.num_coders, all->parms.is_edf);
	if (!all->heap || init_alloc(all) != 0 || init_dongles(all) != 0
		|| init_coders(all) != 0)
	{
		cleanup_dongles(all, all->parms.num_coders);
		free(all->coder);
		all->coder = NULL;
		heap_destroy(all->heap);
		all->heap = NULL;
		return (1);
	}
	all->request_seq = 0;
	return (0);
}
