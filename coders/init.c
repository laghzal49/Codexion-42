/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 20:37:08 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/05/13 12:48:13 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_sync(t_app *all)
{
	if (pthread_mutex_init(&all->log_mutex, NULL) != 0)
		return (FAIL);
	all->log_mutex_ready = 1;
	if (pthread_mutex_init(&all->req_mu, NULL) != 0)
		return (FAIL);
	all->req_mu_ready = 1;
	if (pthread_cond_init(&all->req_cv, NULL) != 0)
		return (FAIL);
	all->req_cv_ready = 1;
	return (SUCCESS);
}

static int	init_alloc(t_app *all)
{
	all->dongles = malloc(sizeof(t_tool) * all->parms.num_coders);
	if (!all->dongles)
		return (FAIL);
	memset(all->dongles, 0, sizeof(t_tool) * all->parms.num_coders);
	all->coder = malloc(sizeof(t_dev) * all->parms.num_coders);
	if (!all->coder)
	{
		free(all->dongles);
		all->dongles = NULL;
		return (FAIL);
	}
	memset(all->coder, 0, sizeof(t_dev) * all->parms.num_coders);
	return (SUCCESS);
}

static int	init_dongles(t_app *all)
{
	int	index;

	index = 0;
	while (index < all->parms.num_coders)
	{
		all->dongles[index].in_use = 0;
		all->dongles[index].cooldown = 0;
		all->dongles[index].heap_mutex_ready = 0;
		all->dongles[index].mutex_ready = 0;
		pthread_mutex_init(&all->dongles[index].mutex, NULL);
		all->dongles[index].heap = heap_init(2, all->parms.is_edf);
		if (!all->dongles[index].heap)
			return (1);
		if (pthread_mutex_init(&all->dongles[index].heap_mutex, NULL) != 0)
			return (FAIL);
		all->dongles[index].mutex_ready = 1;
		all->dongles[index].heap_mutex_ready = 1;
		index++;
	}
	return (SUCCESS);
}

static int	init_coders(t_app *all)
{
	int	index;
	int	next_index;

	index = 0;
	while (index < all->parms.num_coders)
	{
		all->coder[index].cv_mu_ready = 0;
		next_index = (index + 1) % all->parms.num_coders;
		all->coder[index].coder_id = index + 1;
		all->coder[index].compile_count = 0;
		all->coder[index].right_dongle = &all->dongles[index];
		all->coder[index].left_dongle = &all->dongles[next_index];
		all->coder[index].request_time = 0;
		all->coder[index].all = all;
		if (pthread_mutex_init(&all->coder[index].cv_mu, NULL) != 0)
			return (FAIL);
		all->coder[index].cv_mu_ready = 1;
		index++;
	}
	return (SUCCESS);
}

int	init_all(t_app *all)
{
	if (init_sync(all) != SUCCESS)
		return (FAIL);
	if (init_alloc(all) != SUCCESS || init_dongles(all) != SUCCESS
		|| init_coders(all) != SUCCESS)
	{
		cleanup_dongles(all, all->parms.num_coders);
		free(all->coder);
		all->coder = NULL;
		return (FAIL);
	}
	return (SUCCESS);
}
