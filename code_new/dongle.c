/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlaghzal <tlaghzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:32:05 by tlaghzal          #+#    #+#             */
/*   Updated: 2026/04/24 14:48:03 by tlaghzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <pthread.h>

void	request_dongle(t_coder *coder, t_dongle *dongle);

static void	select_order(t_coder *coder, t_dongle **first, t_dongle **second)
{
	if (coder->left_dongle->id % 2 != 0)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
	else
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
}

static void	release_first_dongle(t_coder *coder, t_dongle *first)
{
	first->in_use = 0;
	coder->has_first_dongle = 0;
	pthread_cond_signal(&coder->all->req_cv);
}

int	request_second_dongle(t_coder *coder, t_dongle *first, t_dongle *second)
{
	long long	now;

	pthread_mutex_lock(&coder->all->req_mu);
	now = get_time_in_ms();
	if (first != second && (second->in_use != 0 || now < second->cooldown))
	{
		release_first_dongle(coder, first);
		pthread_mutex_unlock(&coder->all->req_mu);
		smart_sleep(1, coder->all);
		return (0);
	}
	pthread_mutex_unlock(&coder->all->req_mu);
	request_dongle(coder, second);
	if (should_stop(coder->all))
	{
		pthread_mutex_lock(&coder->all->req_mu);
		release_first_dongle(coder, first);
		pthread_mutex_unlock(&coder->all->req_mu);
		return (0);
	}
	return (1);
}

void	request_dongle(t_coder *coder, t_dongle *dongle)
{
	t_all	*all;

	all = coder->all;
	pthread_mutex_lock(&all->req_mu);
	coder->target_dongle = dongle;
	coder->granted = 0;
	if (coder->has_first_dongle == 0)
	{
		if (coder->compile_count == 0)
			coder->request_seq = coder->coder_id;
		else
			coder->request_seq = ++all->request_seq;
	}
	heap_insert(all->heap, coder);
	pthread_cond_signal(&all->req_cv);
	while (!coder->granted && !all->stop_flag)
	{
		pthread_cond_wait(&coder->cv, &all->req_mu);
	}
	pthread_mutex_unlock(&all->req_mu);
}

void	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	select_order(coder, &first, &second);
	while (!should_stop(coder->all))
	{
		request_dongle(coder, first);
		coder->has_first_dongle = 1;
		if (should_stop(coder->all))
		{
			coder->has_first_dongle = 0;
			return ;
		}
		if (request_second_dongle(coder, first, second) == 0)
			continue ;
		print_state(coder, "has taken a dongle");
		print_state(coder, "has taken a dongle");
		return ;
	}
}
